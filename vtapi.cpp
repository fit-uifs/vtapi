/* 
 * File:   VTApi.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:42
 */

#include <cstdlib>
#include <iostream>

#include "cli_settings.h"
#include "vtapi.h"


using namespace std;

VTApi::VTApi(int argc, char** argv) {
    gengetopt_args_info args_info;
    struct cmdline_parser_params *cli_params;
    bool warn = false;

    // Initialize parser parameters structure
    cli_params = cmdline_parser_params_create();
    // Hold check for required arguments until config file is parsed
    cli_params->check_required = 0;
    if (cmdline_parser_ext (argc, argv, &args_info, cli_params)) warn = true;
    // Get the rest of arguments from config file, don't override cmdline
    cli_params->initialize = 0;
    cli_params->override = 0;
    cli_params->check_required = 1;
    if (cmdline_parser_config_file (args_info.config_arg, &args_info, cli_params)) warn = true;

    // TODO: user authentization here

    // Create commons class to store connection etc.
    commons = new Commons(args_info);
    // Construct string from unnamed arguments
    for (int i = 0; i < args_info.inputs_num; i++)
        un_args.append(args_info.inputs[i]).append(" ");

    if (warn) commons->warning("Error parsing config arguments");
    cmdline_parser_free (&args_info);
    free (cli_params);
}

VTApi::VTApi(const String& configFile) {
    gengetopt_args_info args_info;
    struct cmdline_parser_params *cli_params;
    bool warn = false;

    // Initialize parser parameters structure and parse config file
    cli_params = cmdline_parser_params_create();
    if (cmdline_parser_config_file (configFile.c_str(), &args_info, cli_params)) warn = true;
    // TODO: user authentization here

    // Create commons class to store connection etc.
    commons = new Commons(args_info);
    // Construct string from unnamed arguments
    for (int i = 0; i < args_info.inputs_num; i++)
        un_args.append(args_info.inputs[i]).append(" ");

    if (warn) commons->warning("Error parsing config arguments");
    cmdline_parser_free (&args_info);
    free (cli_params);
}

VTApi::VTApi(const VTApi& orig)
        : commons((&orig)->commons) {
}

VTApi::~VTApi() {
}

Dataset* VTApi::newDataset(const String& name) {
    return (new Dataset(*commons, name));
}

void VTApi::test() {
    cout << "TESTING generic classes:" << endl;
    TKeyValue<float> tkvf ("float", "number", 32156.7, "test");
    cout << "TKeyValue<float> tkvf (32156.7) ... typeid: " << typeid(tkvf).name() << endl;
    tkvf.print();

    cout << "std::vector<TKey*> ..." << endl;
    std::vector<TKey*> v;
    v.push_back(&tkvf);
    TKey* kv2 = new  TKeyValue<String> ("varchar", "string", "whatever", "test");
    v.push_back(kv2);
    
    char* chs[] = {"1", "2", "3", "ctyri", "5"};
    TKeyValue<char*> tki ("varchar[]", "array", chs, 5);
    v.push_back(&tki);

    for (int i = 0; i < v.size(); ++i) (v[i])->print();

    cout << "static_cast< TKeyValue<char*>* >(v[2]);" << endl;
    TKeyValue<char*>* tkic = static_cast< TKeyValue<char*>* >(v[2]);
    tkic->print();
    cout << "DONE testing generic classes." << endl;

    // dataset usw.
    cout << endl << endl;
    cout << "TESTING  Dataset" << endl;
    Dataset* dataset = this->newDataset();
    dataset->next();
    dataset->printRes(dataset->select->res);

    cout << endl;
    cout << "TESTING Sequence" << endl;
    cout << "USING dataset " << dataset->getDataset() << endl;

    Sequence* sequence = dataset->newSequence();
    sequence->next();
    sequence->printAll();

    cout << "ADDING Sequence " << "just_a_test_sequence" << endl;
    sequence->add("just_a_test_sequence", "/test_location");
    sequence->next();

    free(sequence);
    sequence = dataset->newSequence();
    sequence->next();   // this can execute and commit (a suicide)
    sequence->printAll();

    cout << "DELETING Sequence " << "just_a_test_sequence" << endl;
    Query* query = new Query(*sequence, "DELETE FROM public.sequences WHERE seqname='just_a_test_sequence';");
    cout << "OK:" << query->execute() << endl;
    free(query);

    cout << endl;
    cout << "TESTING Interval" << endl;
    cout << "USING sequence " << sequence->getSequence() << endl;
    
    Interval* interval = sequence->newInterval();
    interval->select->limit = 10;
    interval->next();
    cout << "// FIXME: PQprint is deprecated" << endl;
    interval->print();

    // process usw.
    cout << endl;
    cout << "TESTING  Method" << endl;
    Method* method = dataset->newMethod();
    method->next();
    method->printAll();

    cout << endl;
    cout << "TESTING  MethodKeys" << endl;
    cout << "USING method " << method->getName() << endl;
    method->getMethodKeys();
    cout << "Number of method keys: " << method->methodKeys.size() << endl;
    for (int i = 0; i < method->methodKeys.size(); i++) {
        method->methodKeys[i].print();
    }

    cout << endl;
    cout << "TESTING  Process" << endl;
    Process* process = method->newProcess();
    process->next();
    process->printAll();


}
