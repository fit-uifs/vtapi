/**
 * @file    vtapi.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of VTApi class
 */

#include <vector>

#include "vtapi.h"

using namespace vtapi;


VTApi::VTApi(int argc, char** argv) {
    gengetopt_args_info args_info;
    struct cmdline_parser_params *cli_params;
    bool warn = false;

    // Initialize parser parameters structure
    cli_params = cmdline_parser_params_create();
    // Hold check for required arguments until config file is parsed
    cli_params->check_required = 0;
	// Parse cmdline first
    if (cmdline_parser_ext (argc, argv, &args_info, cli_params) != 0) warn = true;
    // Get the rest of arguments from config file, don't override cmdline
    cli_params->initialize = 0;
    cli_params->override = 0;
    cli_params->check_required = 1;
	// Parse config file
    if (cmdline_parser_config_file (args_info.config_arg, &args_info, cli_params) != 0) warn = true;
        // Check if all args are specified
    if (cmdline_parser_required (&args_info, "VTApi") != EXIT_SUCCESS) {
        cerr << "Aborting: Database connection info missing. Use \"-h\" for help. " << endl;
        cerr << "Use config file (--config=\"/path/to/somefile.conf\") or check help for command line option \"-c\"." << endl;
        cmdline_parser_free (&args_info);
        destruct (cli_params);
        throw new std::exception();
    }
    // TODO: user authentization here

    // Create commons class to store connection etc.
    commons = new Commons(args_info);
    if (warn) {
        cerr << "Error parsing config arguments" << endl;
    }
    cmdline_parser_free (&args_info);
    destruct (cli_params);
}

VTApi::VTApi(const string& configFile) {
    gengetopt_args_info args_info;
    bool warn = false;

    // Parse config file
    if (cmdline_parser_configfile (configFile.c_str(), &args_info, 0, 1, 1) != 0) warn = true;
    // TODO: user authentization here

    // Create commons class to store connection etc.
    commons = new Commons(args_info);
    if (warn) {
        cerr << "Error parsing config arguments" << endl;
    }
    cmdline_parser_free (&args_info);
}

VTApi::VTApi(const VTApi& orig)
        : commons((&orig)->commons) {
}

VTApi::~VTApi() {
    destruct(commons);
}


Dataset* VTApi::newDataset(const string& name) {
    return (new Dataset(*commons, name));
}





















void VTApi::test() {
    cout << "BEGINNING testing..." << endl;
    
    cout << endl << "---------------------------------------------------------------" << endl;
    TimExer* timex = new TimExer();
#ifdef PROCPS_PROC_READPROC_H
    cout << "Process " << timex->getPID() << " consumes " << timex->getMemory() << " MB resident and " << timex->getVirtMemory() << " MB virtual memory.";
#else
    cout << "WAAARNIING: You should have libproc_dev installed to know how much memory this process consumes...";
#endif
    cout << endl << "---------------------------------------------------------------" << endl << endl;    

    cout << "INITIALIZING dataset and sequence..." << endl << endl;
    Dataset *dataset    = this->newDataset();
    dataset->next();
    Sequence *sequence  = dataset->newSequence();
    sequence->next();
    cout << endl << "---------------------------------------------------------------" << endl << endl;

    this->testGenericClasses();
    this->testDataset();
    this->testSequence(dataset);
    this->testInterval(sequence);
//    this->testImage(sequence);
    this->testVideo(dataset);
    this->testMethod(dataset);
    this->testProcess(dataset);
    
    cout << "** CLEANUP" << endl;
    destruct(sequence);
    destruct(dataset);
    cout << endl << "---------------------------------------------------------------" << endl;

#ifdef PROCPS_PROC_READPROC_H
    cout << "Process " << timex->getPID() << " consumes " << timex->getMemory() << " MB resident and " << timex->getVirtMemory() << " MB virtual memory." << endl;
#else
    cout << "WAAARNIING: You should have libproc_dev installed to know how much memory this process consumes...";
#endif
    cout << "This took " << timex->getClock() << " s of processor and " << timex->getTime()  << " s of real time.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
    destruct(timex);

    cout << "DONE ALL ... see warnings." << endl;
}

void VTApi::testGenericClasses() {
    cout << "TESTING generic classes..." << endl << endl;

    cout << "** NEW TKeyValue<float>" << endl;
    TKeyValue<float> kvFloat ("float", "my_number", 32156.7, "test");
    cout << "typeid: " << typeid(kvFloat).name() << endl;
    kvFloat.print();

    cout << "** NEW TKeyValue<string> (ptr)" << endl;
    TKeyValue<string> *kvStringPt = new TKeyValue<string> ("varchar", "my_text", "ladidada");
    cout << "typeid: " << typeid(kvStringPt).name() << endl;
    kvStringPt->print();

    cout << "** NEW TKeyValue<const char *>" << endl;
    const char* chs[] = {"1", "2", "3", "ctyri", "5"};
    TKeyValue<const char*> kvArray ("varchar[]", "my_array", chs, 5, "test");
    cout << "typeid: " << typeid(kvArray).name() << endl;
    kvArray.print();

    cout << "** NEW TKeyValues with everything above" << endl;
    TKeyValues kvAll;
    kvAll.push_back(&kvFloat);
    kvAll.push_back(kvStringPt);
    kvAll.push_back(&kvArray);

    cout << "** PRINT TKeyValues" << endl;
    for (int i = 0; i < kvAll.size(); ++i) (kvAll[i])->print();

    cout << "** TRY static cast of array" << endl;
    TKeyValue<char*>* tkic = static_cast< TKeyValue<char*>* >(kvAll[2]);
    tkic->print();

    cout << "** TRY deserialization of [1.23,2,3.8,XYZ,5]" << endl;
    const char serial[] = "[1.23,2,3.8,XYZ,5]\0";
    int size             = 0;
    char *arr_serial    = new char[30];
    std::copy(serial, serial+19, arr_serial);    
    int *int_deserial   = deserializeA<int>(arr_serial, size);
    TKeyValue<int> kvIntA("integer[]", "my_ints", int_deserial, size);
    float *fl_deserial  = deserializeA<float>(arr_serial, size);
    TKeyValue<float> kvFloatA("float[]", "my_floats", fl_deserial, size);
    kvIntA.print();
    kvFloatA.print();

    cout << "** CLEANUP" << endl;
    destruct(int_deserial);
    destruct(fl_deserial);
    destruct(kvStringPt);

    cout << endl << "DONE testing generic classes.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}

void VTApi::testDataset() {
    cout << "TESTING  Dataset..." << endl <<  endl;

    cout << "** NEW dataset" << endl;
    Dataset* dataset = this->newDataset();
    dataset->next();
    dataset->printAll();

    cout << "** COUNT sequences in dataset" << endl;
    KeyValues* kv = new KeyValues(*dataset);
    kv->select = new Select(*kv, "SELECT COUNT(*) FROM "+ dataset->getDataset() + ".sequences;");
    kv->next();
    kv->printAll();
    cout << "There are " << kv->getInt8(0) << " sequences in this dataset." << endl;

    cout << "** CLEANUP" << endl;
    destruct(kv);
    destruct(dataset);

    cout << endl << "DONE testing dataset.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}

void VTApi::testSequence(Dataset *dataset) {
    cout << "TESTING Sequence..." << endl <<  endl;
    cout << "USING dataset " << dataset->getDataset() << endl << endl;

    srand(time(NULL));
    string sn = "test_sequence" + toString(rand()%1000);

    cout << "** ADDING Sequence " << sn << endl;
    Sequence* sequence = dataset->newSequence(sn);
    sequence->add(sn, "/test_location", "video");
    sequence->next();
    sequence->print();

    cout << "** UPDATING sequence location" << sn << endl;
    sequence->setString("seqlocation", "/UPDATED");
    sequence->setExecute();
    sequence->next();   // this will now return NULL, because we have only one sequence
    sequence->next();   // this will now get us our old sequence but with updated data
    sequence->print();

    cout << "** SHOWING all sequences" << endl;
    destruct (sequence);
    sequence = dataset->newSequence();
    sequence->next();
    sequence->printAll();

    cout << "** DELETING Sequence " << sn << endl;
    Query* query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".sequences WHERE seqname='" + sn + "';");
    query->execute();

    cout << "** CLEANUP" << endl;
    destruct(query);
    destruct(sequence);

    cout << endl << "DONE testing sequence.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}

void VTApi::testInterval(Sequence *sequence) {
    cout << "TESTING Interval..." << endl <<  endl;
    cout << "USING sequence " << sequence->getSequence() << endl << endl;

    cout << "** SHOWING all intervals" << endl;
    Interval* interval = sequence->newInterval();
    interval->select->setLimit(10);
    // interval->select->from("intervals", "DESC_DENSE16_CSIFT_NoA_UNC_K32_o10_L01_KM_L12[1]");
    // interval->select->whereFloat("DESC_DENSE16_CSIFT_NoA_UNC_K32_o10_L01_KM_L12[1]", 0.0, ">");
    // interval->select->whereString("test", "NULL");
    interval->next();
    interval->print();

    
    cout << "** PRINTING interval TKeys" << endl;
    TKeys* keys = interval->getKeys();
    if (keys->empty()) cout << "(no keys)" << endl;
    else for (int i = 0; i < keys->size(); ++i) (*keys)[i].print();

    cout << "** CLEANUP" << endl;
    destruct(keys);
    destruct(interval);
    
    cout << endl << "DONE testing interval.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}

void VTApi::testImage(Sequence *sequence) {
/*
    // this has no effect outside ...
    int t1 = 1000000 + rand()%1000;
    cout << "ADING Image on " << interval->getSequenceName() << " [" << t1 << ", " << t1 << "]" << endl;
    Image* image = sequence->newImage("shot10000_1.jpg"); // new Image(*interval);
    image->next(); // do not forget this (again :), please
    cout << image->getLocation() << endl;
    Sequence* tmpSeq = image->getParentSequence();
    tmpSeq->print();
    destruct(tmpSeq);
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
    Query* query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".intervals WHERE t1=" + toString(t1) + ";");
    query->execute();
    destruct (query);

    cout << "DONE testing image.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
*/
}

void VTApi::testVideo(Dataset *dataset) {
    cout << "Testing Video..." << endl <<  endl;

    cout << "** SHOWING the first video" << endl;
    Video* video = dataset->newVideo();
    video->next();
    video->print();

#ifdef __OPENCV_HIGHGUI_HPP__
    cout << "** PLAYING video " << video->getName() << " ... press any key to exit." << endl;
    VideoPlayer* player = new VideoPlayer(*video);
    player->play();
    destruct(player);
#else
    cout << "** CANNOT PLAY VIDEO " << video->getName() << endl;
#endif

    string vn = "MCTTR0201a-XXXX";
    cout << "** ADDING video " << vn << " to the dataset" << endl;
    video->add(vn, "MCT_TR_02/MCTTR02a/MCTTR0201a.mov.deint.mpeg");
    video->addExecute();

    cout << "** DELETING video " << vn << endl;
    Query* query = new Query(*video, "DELETE FROM "+ dataset->getDataset() + ".sequences WHERE seqname=\'" + vn + "\';");
    query->execute();

    cout << "** CLEANUP" << endl;
    destruct(query);
    destruct(video);

    cout << endl << "DONE testing video.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}

void VTApi::testMethod(Dataset *dataset) {
    cout << "TESTING Method..." << endl <<  endl;

    cout << "** SHOWING all methods" << endl;
    Method* method = dataset->newMethod();
    method->next();
    method->printAll();

    
    cout << "SHOWING method keys for method " << method->getName() << endl;
    if (method->methodKeys.empty()) cout << "(no keys)" << endl;
    else for (int i = 0; i < method->methodKeys.size(); i++) {
        method->methodKeys[i].print();
    }

    cout << "** CLEANUP" << endl;
    destruct(method);

    cout << endl << "DONE testing method.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}

void VTApi::testProcess(Dataset *dataset) {
    cout << "TESTING Process..." << endl <<  endl;

    cout << "** SHOWING all processes" << endl;
    Process* process = dataset->newProcess();
    process->next();
    process->printAll();

    // tohle doma nezkousejte, nebo vam upadne zadecek
//    process->add("test", "test");

    cout << "** CLEANUP" << endl;
    destruct(process);

    cout << endl << "DONE testing process.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}


 /**
  * @endcode
  */

