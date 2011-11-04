/* 
 * File:   VTApi.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:42
 */

#include <cstdlib>
#include <iostream>

#include "vtapi_settings.h"
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
    if (warn) commons->warning("Error parsing config arguments");
    cmdline_parser_free (&args_info);
    free (cli_params);
}

VTApi::VTApi(const String& configFile) {
    gengetopt_args_info args_info;
    bool warn = false;

    // Parse config file
    if (cmdline_parser_configfile (configFile.c_str(), &args_info, 0, 1, 1)) warn = true;
    // TODO: user authentization here

    // Create commons class to store connection etc.
    commons = new Commons(args_info);
    if (warn) commons->warning("Error parsing config arguments");
    cmdline_parser_free (&args_info);
}

VTApi::VTApi(const VTApi& orig)
        : commons((&orig)->commons) {
}

VTApi::~VTApi() {
}

Dataset* VTApi::newDataset(const String& name) {
    return (new Dataset(*commons, name));
}


/**
 * This might be a HOW-TO function
 */
void VTApi::test() {
    // lines starting with cout should be ignored :)
    
    cout << "TESTING generic classes..." << endl;
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

    // dataset usw.
    cout << "DONE testing generic classes." << endl;
    cout << endl << endl;
    cout << "TESTING  Dataset..." << endl;

    Dataset* dataset = this->newDataset();
    dataset->next();
    dataset->printAll();

    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Sequence..." << endl;
    cout << "USING dataset " << dataset->getDataset() << endl;

    Sequence* sequence = dataset->newSequence();
    sequence->next();
    sequence->printRes(sequence->select->res); // equivalent to printAll()

    srand(time(NULL));
    String sn = "test_sequence" + toString(rand()%1000);
    cout << "ADDING Sequence " << sn << endl;
    sequence->add(sn, "/test_location");
    sequence->next();

    // FIXME: proc to tady pada????
    delete (sequence);
    sequence = dataset->newSequence();
    sequence->next();   // this can execute and commit (a suicide)
    sequence->printAll();

    cout << "DELETING Sequence " << sn << endl;
    Query* query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".sequences WHERE seqname='" + sn + "';");
    cout << "OK: " << query->execute() << endl;
    delete (query);

    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Interval..." << endl;
    cout << "USING sequence " << sequence->getSequence() << endl;
    
    Interval* interval = sequence->newInterval();
    interval->select->limit = 10;
    interval->next();
    interval->print();
    
    // this has no effect outside ...
    int t1 = 1000000 + rand()%1000;
    cout << "ADING Image on " << interval->getSequence() << " [" << t1 << ", " << t1 << "]" << endl;
    Image* image = new Image(*interval);
    image->next(); // do not forget this (again :), please
    image->add(interval->getSequence(), t1, "nosuchimage.jpg");
    // image->insert->keyFloat("sizeKB", 100.3);
    float kf[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    image->insert->keyFloatA("test", kf, 5);
    image->insert->execute();     // or next() must be called after inserting all voluntary fields such as above
    
    delete (image);    // if not called execute() or next(), the insert destructor raises a warning
        
    // delete where t1 > 999999 to get rid of the testing value
    cout << "DELETING Image " << sn << endl;
    query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".intervals WHERE t1=" + toString(t1) + ";");
    cout << "OK: " << query->execute() << endl;
    delete (query);

    delete (interval);
    delete (sequence);

    // process usw.
    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Method..." << endl;

    Method* method = dataset->newMethod();
    method->next();
    method->printAll();

    cout << endl;
    cout << "TESTING  MethodKeys" << endl;
    cout << "USING method " << method->getName() << endl;
    cout << "Printing methodKeys (" << method->methodKeys.size() << ")" << endl;

    for (int i = 0; i < method->methodKeys.size(); i++) {
        method->methodKeys[i].print();
    }

    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Process" << endl;

    Process* process = method->newProcess();
    process->next();
    process->printAll();

    delete (process);
    delete (method);
    cout << "DONE." << endl;

    delete (dataset);
    cout << "DONE ALL ... see warnings." << endl;
}
