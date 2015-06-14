/**
 * @file
 * @brief   Methods of VTApi class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include <common/vtapi_settings.h>
#include <vtapi.h>

using namespace std;

namespace vtapi {


VTApi::VTApi(int argc, char** argv)
{
    gengetopt_args_info args_info;
    struct cmdline_parser_params cli_params;
    bool warn = false;
    
    // Initialize parser parameters structure
    // Hold check for required arguments until config file is parsed
    cmdline_parser_params_init(&cli_params);
    cli_params.check_required = 0;
    
    // Parse cmdline first
    warn |= (cmdline_parser_ext (argc, argv, &args_info, &cli_params) != 0);
    
    // Get the rest of arguments from config file, don't override cmdline
    cli_params.initialize = 0;
    cli_params.override = 0;
    cli_params.check_required = 1;
    
    // Parse config file now
    warn |= (cmdline_parser_config_file (args_info.config_arg, &args_info, &cli_params) != 0);
    
    // Check if all args are specified
    if (cmdline_parser_required (&args_info, "VTApi") != EXIT_SUCCESS) {
        cerr << "Aborting: Database connection info missing. Use \"-h\" for help. " << endl;
        cerr << "Use config file (--config=\"/path/to/somefile.conf\") or check help for command line option \"-c\"." << endl;
        cmdline_parser_free (&args_info);
        throw new exception;
    }

    // Create commons class to store connection etc.
    commons = new Commons(args_info);
    if (warn) {
        commons->getLogger()->warning("Error parsing config arguments", "VTApi()");
    }
    cmdline_parser_free (&args_info);
}

VTApi::VTApi(const string& configFile)
{
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
: commons((&orig)->commons)
{}

VTApi::~VTApi()
{
    vt_destruct(commons);
}


Process *VTApi::initProcess(ProcessState &initState)
{
    Process * p = NULL;
    
    if (!this->commons->getProcess().empty()) {
        if (p = new Process(*this->commons)) {
            if (p->next()) {
                ProcessState *ps = p->getProcessState("state");
                if (ps) {
                    initState = *ps;
                    delete ps;
                }
            }
            else {
                vt_destruct(p);
            }
        }
    }

    return p;
}





















void VTApi::test() {
    cout << "BEGINNING testing..." << endl;
    cout << endl << "---------------------------------------------------------------" << endl << endl;    

    cout << "INITIALIZING dataset and sequence..." << endl << endl;
    Dataset *dataset    = this->newDataset();
    dataset->next();
    Sequence *sequence  = dataset->newSequence();
    sequence->next();
    cout << endl << "---------------------------------------------------------------" << endl << endl;

//    this->testPerformance();
    this->testDataset();
    this->testSequence(dataset);
    this->testInterval(sequence);
//    this->testImage(sequence);
    this->testVideo(dataset);
    //this->testMethod(dataset);
    this->testProcess(dataset);
    
    cout << "** CLEANUP" << endl;
    vt_destruct(sequence);
    vt_destruct(dataset);
    cout << endl << "---------------------------------------------------------------" << endl;

    cout << "DONE ALL ... see warnings." << endl;
}



void VTApi::testPerformance() {
//    int id      = 0;
//    int cnt1    = 0;
//    int cnt2    = 0;
//    Dataset *dataset    = this->newDataset();
//    dataset->next();
//    Sequence *sequence  = dataset->newSequence();
//    sequence->next();
//    Interval *interval  = sequence->newInterval();

//    srand(time(NULL));
//    TimExer* timex = new TimExer();

////pg        sl

// insert sekvence 10s
////4824      51
//    while(timex->getTime() < 10.0) {
//        id = (rand()%100000);
//        sequence->add("name"+toString(id), "loc/", "video","vfroml", "group", "pozn.");
//        sequence->addExecute();
//    }

// insert sekvence and 50 intervals for each 10s
//94        2
//4661      51
//while(timex->getTime() < 10.0) {
//        id = (rand()%100000);
//        sequence->add("name"+toString(id), "loc/", "video","vfroml", "group", "pozn.");
//        sequence->addExecute();
//        for (int i = 0; i < 50; i++) {
//            if (timex->getTime() >= 10) break;
//            interval->add("name"+toString(id), i, i, "loc/");
//            interval->addExecute();
//        }
//    }

 // insert 100 sequences with 100 intervals
//19.29s    1997.84s
//for (int i = 0; i < 100; i++) {
//        sequence->add("name"+toString(i), "loc/", "video","vfroml", "group", "pozn.");
//        sequence->addExecute();
//        for (int j = 0; j < 100; j++) {
//            interval->add("name"+toString(i), j, j, "loc/");
//            interval->addExecute();
//        }
//    }


//insert 10 sequences with 100 intervals big data
//3.49s     240.35s
//    for (int i = 0; i < 10; i++) {
//        sequence->add("name"+toString(i), "loc/", "video","vfroml", "group", "pozn.");
//        sequence->addExecute();
//        for (int j = 0; j < 100; j++) {
//            interval->add("name"+toString(i), j, j, "loc/");
//            interval->insert->keyFloatA("svm", svm, 100);
//            interval->addExecute();
//        }
//    }

// update
//4.82s     246.76s
//while (interval->next()) {
//        interval->updateFloatA("svm", svm, 150);
//    }

// select
// 0.28     0.26
//while (interval->next()) {
//        svm = interval->getFloatA("svm", size);
//        cout << toString(svm) << endl;
//        vt_destruct(svm);
//    }

}


void VTApi::testDataset()
{
    cout << "TESTING  Dataset..." << endl <<  endl;

    cout << "** NEW dataset" << endl;
    Dataset* dataset = this->newDataset();
    dataset->next();
    dataset->printAll();
    cout << "Testing count(): " << dataset->count() << endl;

    cout << "** COUNT sequences in dataset" << endl;
    KeyValues* kv = new KeyValues(*dataset);
    kv->select = new Select(*kv, "SELECT COUNT(*) FROM "+ dataset->getDataset() + ".sequences;");
    kv->next();
    kv->printAll();
    cout << "There are " << kv->getInt8(0) << " sequences in this dataset." << endl;
    
    cout << "** CLEANUP" << endl;
    vt_destruct(kv);
    vt_destruct(dataset);

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
    sequence->updateString("seqlocation", "/UPDATED");
    sequence->updateExecute();
    sequence->next();   // this will now return NULL, because we have only one sequence
    sequence->next();   // this will now get us our old sequence but with updated data
    sequence->print();

    cout << "** SHOWING all sequences" << endl;
    vt_destruct(sequence);
    sequence = dataset->newSequence();
    sequence->next();
    sequence->printAll();
    cout << "Testing count(): " << sequence->count() << endl;

    cout << "** DELETING Sequence " << sn << endl;
    Query* query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".sequences WHERE seqname='" + sn + "';");
    query->execute();

    cout << "** CLEANUP" << endl;
    vt_destruct(query);
    vt_destruct(sequence);

    cout << endl << "DONE testing sequence.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}

void VTApi::testInterval(Sequence *sequence) {
    cout << "TESTING Interval..." << endl <<  endl;
    cout << "USING sequence " << sequence->getSequence() << endl << endl;

    cout << "** SHOWING all intervals" << endl;
    Interval* interval = new Interval(*sequence, "test1out");
    interval->select->setLimit(10);
    // interval->select->from("intervals", "DESC_DENSE16_CSIFT_NoA_UNC_K32_o10_L01_KM_L12[1]");
    // interval->select->whereFloat("DESC_DENSE16_CSIFT_NoA_UNC_K32_o10_L01_KM_L12[1]", 0.0, ">");
    // interval->select->whereString("test", "NULL");
    interval->next();
    interval->print();
    cout << "Testing count(): " << interval->count() << endl;

    
//    cout << "** PRINTING interval TKeys" << endl;
//    TKeys* keys = interval->getKeys();
//    if (keys->empty())
//        cout << "(no keys)" << endl;
//    else for (int i = 0; i < keys->size(); ++i)
//        (*keys)[i].print();

    cout << "** CLEANUP" << endl;
    //vt_destruct(keys);
    vt_destruct(interval);
    
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
    vt_destruct(tmpSeq);
    int cnt = 0;

    // this is how to print arrays
    int* tags = image->getIntA("tags", cnt);
    for (int i=0; i<cnt && i<10; ++i) {
        if (i >= 9) cout << "...";
        else cout << tags[i] << ", ";
    }
    cout << endl;
    vt_destructall(tags);

    image->add(interval->getSequenceName(), t1, "nosuchimage.jpg");
    // image->insert->keyFloat("sizeKB", 100.3);

    float kf[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    image->insert->keyFloatA("test", kf, 5);

    image->insert->execute();     // or next() must be called after inserting all voluntary fields such as above
    vt_destruct(image);    // if not called execute() or next(), the insert destructor raises a warning

    image = sequence->newImage("nosuchimage.jpg");
    image->next();      // in case of update, the next() must be called
    // vypis pole floatu
    int kfl = 0;
    float* kf2 = image->getFloatA("test", kfl);
    for (int i=0; i < kfl; ++i) cout << kf2[i] << " ";
    cout << endl;
    vt_destructall(kf2);

    image->updateString("imglocation", "tudlenudle.png");
    image->updateExecute();  // NEWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

    // delete where t1 > 999999 to get rid of the testing value
    cout << "DELETING Image " << sn << endl;
    Query* query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".intervals WHERE t1=" + toString(t1) + ";");
    query->execute();
    vt_destruct(query);

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

    string vn = "MCTTR0201a-XXXX";
    cout << "** ADDING video " << vn << " to the dataset" << endl;
    video->add(vn, "MCT_TR_02/MCTTR02a/MCTTR0201a.mov.deint.mpeg");
    video->addExecute();

    cout << "** DELETING video " << vn << endl;
    Query* query = new Query(*video, "DELETE FROM "+ dataset->getDataset() + ".sequences WHERE seqname=\'" + vn + "\';");
    query->execute();

    cout << "** CLEANUP" << endl;
    vt_destruct(query);
    vt_destruct(video);

    cout << endl << "DONE testing video.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}

void VTApi::testMethod(Dataset *dataset) {
//    cout << "TESTING Method..." << endl <<  endl;
//
//    cout << "** SHOWING all methods" << endl;
//    Method* method = dataset->newMethod();
//    if (method->next()) {
//        method->printAll();
//
//        cout << "SHOWING method keys for method " << method->getName() << endl;
//        TKeys mk = method->getMethodKeys();
//        if (mk.empty()) {
//            cout << "(no keys)" << endl;
//        }
//        else {
//            for (int i = 0; i < mk.size(); i++) {
//                mk[i].print();
//            }
//        }
//    }
//
//    cout << "** CLEANUP" << endl;
//    vt_destruct(method);
//
//    cout << endl << "DONE testing method.";
//    cout << endl << "---------------------------------------------------------------" << endl << endl;
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
    vt_destruct(process);

    cout << endl << "DONE testing process.";
    cout << endl << "---------------------------------------------------------------" << endl << endl;
}


 /**
  * @endcode
  */

}
