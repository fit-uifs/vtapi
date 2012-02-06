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
	// Parse cmdline first
    if (cmdline_parser_ext (argc, argv, &args_info, cli_params)) warn = true;
    // Get the rest of arguments from config file, don't override cmdline
    cli_params->initialize = 0;
    cli_params->override = 0;
    cli_params->check_required = 1;
	// Parse config file
    if (cmdline_parser_config_file (args_info.config_arg, &args_info, cli_params)) warn = true;

    // TODO: user authentization here

    // Create commons class to store connection etc.
    commons = new Commons(args_info);
    if (warn) commons->warning("Error parsing config arguments");
    cmdline_parser_free (&args_info);
    destruct (cli_params);
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
 * @code
 */
void VTApi::test() {
    // lines starting with cout should be ignored :)
    TimExer* timex = new TimExer();
#ifdef PROCPS_PROC_READPROC_H
    cout << "Process " << timex->getPID() << " consumes " << timex->getMemory() << " MB resident and " << timex->getVirtMemory() << " MB virtual memory." << endl << endl;
#else
    cout << "WAAARNIING: You should have libproc_dev installed to know how much memory this process consumes..." << endl;
#endif

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

    this->commons->logger->debug("No bugs so far... ");
    cout << endl << endl;
    cout << "TESTING  Dataset..." << endl;

    Dataset* dataset = this->newDataset();
    dataset->next();
    dataset->printAll();


    // how many intervals are in a sequence???
    KeyValues* kv = new KeyValues(*dataset);
    kv->select = new Select(*kv, "SELECT COUNT(*) FROM "+ dataset->getDataset() + ".sequences;");
    kv->next();
    kv->printAll();
    int count = kv->getInt(0);
    cout << "There are " << count << " sequences in this dataset." << endl;

    cout << "DONE." << endl;
    cout << endl;

    cout << "TESTING Sequence..." << endl;
    cout << "USING dataset " << dataset->getDataset() << endl;

    Sequence* sequence = dataset->newSequence("s2"); // s2
    sequence->next();
    //sequence->printRes(sequence->select->res); // equivalent to printAll()

    srand(time(NULL));
    String sn = "test_sequence" + toString(rand()%1000);
    cout << "ADDING Sequence " << sn << endl;
    sequence->add(sn, "/test_location");
    sequence->next();

    destruct (sequence);
    sequence = dataset->newSequence("iacc.1.tv11.devel.kf");
    sequence->next();   // this can execute and commit (a suicide)
    sequence->printAll();

  
    cout << "DELETING Sequence " << sn << endl;
    Query* query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".sequences WHERE seqname='" + sn + "';");
    cout << "OK: " << query->execute() << endl;
    destruct (query);

    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Interval..." << endl;
    cout << "USING sequence " << sequence->getSequence() << endl;
    
    Interval* interval = sequence->newInterval();
    interval->select->limit = 10;
    // interval->select->from("intervals", "DESC_DENSE16_CSIFT_NoA_UNC_K32_o10_L01_KM_L12[1]");
    // interval->select->whereFloat("DESC_DENSE16_CSIFT_NoA_UNC_K32_o10_L01_KM_L12[1]", 0.0, ">");
    // interval->select->whereString("test", "NULL");
    interval->next();
    interval->print();

    // get a list of keys in the result
    cout << endl << "Printing interval TKeys" << endl;
    std::vector<TKey>* vtk = interval->getKeys();
    for (int i = 0; i < vtk->size(); ++i) (*vtk)[i].print();
    destruct(vtk);
    cout << endl;

/*
    // this has no effect outside ...
    int t1 = 1000000 + rand()%1000;
    cout << "ADING Image on " << interval->getSequenceName() << " [" << t1 << ", " << t1 << "]" << endl;
    Image* image = sequence->newImage("shot10000_1.jpg"); // new Image(*interval);
    image->next(); // do not forget this (again :), please
    cout << image->getLocation() << endl;
    Sequence* tmpSeq = image->getSequence();
    tmpSeq->print();
    int cnt = 0;

    // this is how to print arrays
    int* tags = image->getIntA("tags", cnt);
    for (int i=0; i<cnt && i<10; ++i) {
        if (i >= 9) cout << "...";
        else cout << tags[i] << ", ";
    }
    cout << endl;
    destructall (tags);

    image->add(interval->getSequenceName(), t1, "nosuchimage.jpg");
    // image->insert->keyFloat("sizeKB", 100.3);

    float kf[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    image->insert->keyFloatA("test", kf, 5);

    image->insert->execute();     // or next() must be called after inserting all voluntary fields such as above
    destruct (image);    // if not called execute() or next(), the insert destructor raises a warning
        
    image = sequence->newImage("nosuchimage.jpg");
    image->next();      // in case of update, the next() must be called    
    // vypis pole floatu
    int kfl = 0;
    float* kf2 = image->getFloatA("test", kfl);
    for (int i=0; i < kfl; ++i) cout << kf2[i] << " ";
    cout << endl;
    destructall(kf2);

    image->setString("imglocation", "tudlenudle.png");
    image->setExecute();  // NEWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

    // delete where t1 > 999999 to get rid of the testing value
    cout << "DELETING Image " << sn << endl;
    query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".intervals WHERE t1=" + toString(t1) + ";");
    cout << "OK: " << query->execute() << endl;
    destruct (query);
*/
    destruct (interval);
    destruct (sequence);

    // TEST VIDEO //////////////////////////////////////////////////////////
    cout << endl << "Testing Video" << endl;
    Video* video = dataset->newVideo();
    video->next();
    video->print();

    cout << endl << "Should be playing video " << video->getName() << " ... press any key to exit." << endl;
    VideoPlayer* player = new VideoPlayer(*video);
    player->play();

    cout << endl << "Adding video 'MCTTR0201a-XXXX' to the dataset" << endl;
    video->add("MCTTR0201a-XXXX", "MCT_TR_02/MCTTR02a/MCTTR0201a.mov.deint.mpeg");
    video->addExecute();
    // FIXME: Vojta kdyz tam je, tak to pise std::badalloc... asi zvalgrindovat
    cout << "OK, deleting the video... " << endl;
    query = new Query(*video, "DELETE FROM "+ dataset->getDataset() + ".sequences WHERE seqname='MCTTR0201a-XXXX';");
    cout << "OK: " << query->execute() << endl;
    destruct (query);

    // TEST PROCESS usw. ////////////////////////////////////////////////////////////
    cout << endl << "DONE." << endl;
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

    // tohle doma nezkousejte, nebo vam upadne zadecek
    // process->add("test", "test");


#ifdef PROCPS_PROC_READPROC_H
    cout << endl << "Process " << timex->getPID() << " consumes " << timex->getMemory() << " MB resident and " << timex->getVirtMemory() << " MB virtual memory." << endl;
#else
    cout << "WAAARNIING: You should have libproc_dev installed to know how much memory this process consumes..." << endl;
#endif
    cout << "This took " << timex->getClock() << " s of processor and " << timex->getTime()  << " s of real time." << endl << endl;
    destruct(timex);


    destruct (process);
    destruct (method);
    cout << "DONE." << endl;

    destruct (dataset);
    cout << "DONE ALL ... see warnings." << endl;
}

 /**
  * @endcode
  */

