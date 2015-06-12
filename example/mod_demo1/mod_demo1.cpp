/*
 * Demo1 modul VTApi
 * by: Vojtech Froml (ifroml[at]fit.vutbr.cz)
 * 
 * Dummy modul; ukazka instance VTApi procesu
 *  - pro kazde video v datasetu ulozi vektor 3 floatu, jehoz hodnoty
 *  - zavisi na 2 vstupnich parametrech
 *  - ukazka jednoduche synchronizace mezi kontrolnim a vypocetnim vlaknem
 * 
 * [CHANGE] oznacuje zmeny z commitu 29.4.2015
*/

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <vtapi.h>

using namespace vtapi;

// [CHANGE] synchronizacni pomucka
typedef struct _SYNC_T
{
    std::mutex mtxLock;
    std::condition_variable cvResume;
    bool bSuspended;
    bool bStopped;
    
    _SYNC_T(bool bSuspended)
    {
        this->bSuspended = bSuspended;
        this->bStopped = false;
    }
} SYNC_T;


// [CHANGE] callback pro reakce na kontrolni prikazy z launcheru (kontrolni vlakno)
void ControlCallback(ProcessControl::COMMAND_T command, void *context);
// [CHANGE] overeni, zda nebyl prijat kontroli prikaz
bool check_state(Process *process, ProcessControl *pctrl, SYNC_T& sync);
// [CHANGE] vypocetni funkce (vypocetni vlakno)
void do_work(Process *process, ProcessControl *pctrl, Dataset *dataset, SYNC_T& sync);



///////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    // instanciace VTApi, pripojeni k DB
    VTApi *vtapi = new VTApi(argc, argv);
    
    // [CHANGE] vstupni stav procesu
    ProcessState initState;
    
    // ziskani proces objektu, jehoz jsme instanci
    // [CHANGE] - odstranena nutnost nasledujici funkce next()
    Process *process = vtapi->initProcess(initState);
    if (process) {
        printf("mod_demo1: launched as process %s\n", process->getName().c_str());

        // synchronizacni objekty
        SYNC_T sync(initState.status == ProcessState::STATUS_SUSPENDED);
        
        // [CHANGE] objekt pro komunikaci
        ProcessControl *pctrl = process->getProcessControl();
        if (pctrl) {
            // [CHANGE] spustime jako server = prijimame prikazy, posilame notifikace o stavu
            if (pctrl->server(ControlCallback, (void *)&sync)) {
                printf("mod_demo1: started server\n");

                Dataset *dataset = vtapi->newDataset();
                dataset->next();

                // provedeme nejakou dummy praci
                do_work(process, pctrl, dataset, sync);

                delete dataset;
            }
            else {
                fprintf(stderr, "mod_demo1: failed to start server\n");
            }
            
            delete pctrl;
        }
        else {
            fprintf(stderr, "mod_demo1: failed to get ProcessControl object\n");
        }
        
        delete process;
    }
    else {
        fprintf(stderr, "mod_demo1: not launched\n");
    }

    delete vtapi;
    
    return 0;
}

// [CHANGE] callback pro reakce na kontrolni prikazy z launcheru (kontrolni vlakno)
void ControlCallback(ProcessControl::COMMAND_T command, void *context)
{
    SYNC_T *sync = (SYNC_T *)context;
    std::unique_lock<std::mutex> lk(sync->mtxLock);

    switch (command)
    {
    case ProcessControl::COMMAND_STOP:
        sync->bStopped = true;
    case ProcessControl::COMMAND_RESUME:
        sync->bSuspended = false;
        lk.unlock();
        sync->cvResume.notify_all();
        break;
    case ProcessControl::COMMAND_SUSPEND:
        sync->bSuspended = true;
        break;
    default:
        break;
    }
}

// [CHANGE] overeni, zda nebyl prijat kontroli prikaz
bool check_state(Process *process, ProcessControl *pctrl, SYNC_T& sync)
{
    std::unique_lock<std::mutex> lk(sync.mtxLock);
    
    if (sync.bSuspended) {
        process->updateStateSuspended(pctrl);
        sync.cvResume.wait(lk);
    }

    return !sync.bStopped;
}

// vypocetni funkce (vypocetni vlakno)
void do_work(Process *process, ProcessControl *pctrl, Dataset *dataset, SYNC_T& sync)
{
    // [CHANGE] initial wait if process was started suspended
    // false = stop proces
    if (!check_state(process, pctrl, sync)) return;
    
    // ziskame parametry naseho procesu
    int param1 = 0;
    double param2 = 0.0;
    ProcessParams *params = process->getParams();
    if (params) {
        if (params->getInt("param1", param1))
            printf("mod_demo1 : param1=%d\n", param1);
        if (params->getDouble("param2", param2))
            printf("mod_demo1 : param2=%g\n", param2);
        delete params;
    }
    
    // vysledne pole floatu a matice
    float features_array[3] = { 0 };
    cv::Mat1f features_mat(3, 2);
    float dummy_seed = 0.0;
    
    // vystupni data, do kterych budeme ukladat vysledky
    Interval *output = process->getOutputData();
    output->next();

    // objekt pro iteraci videi
    Video *video = dataset->newVideo();
    
    // [CHANGE] pocet videi celkove
    size_t cntTotal = video->count();
    size_t cntDone  = 0;
    bool error = false;

    // [CHANGE] prvni notifikace
    process->updateStateRunning(0, video->getName(), pctrl);

    // projdeme vsechna videa datasetu
    // [CHANGE] overeni pozastaveni/stopnuti procesu
    while (video->next() && check_state(process, pctrl, sync)) {

        // [CHANGE] ukazka otevreni, zpracovani videa
        if (video->openVideo()) {
            // ukazka: vlastni cv capture
            cv::VideoCapture& cap = video->capture;
            (cap);
            
            // ukazka: iterace pres framy
            for (cv::Mat frame = video->getData(); frame.data; frame = video->getData()) {
                (frame);
                break;
            }
        }
        else {
            error = true;
            fprintf(stderr, "mod_demo1: failed to open video %s\n", video->getDataLocation().c_str());
            break;
        }
        
        // ukazka ulozeni vysledku
        
        // anotace bude napr. pro prvnich 10 framu
        output->add(video->getName(), 1, 10, "", "demouser", "random-generated");
        
        // takto napr. pole floatu
        features_array[0] += param1 / 1000.0;
        features_array[1] += param2 / 500.0 - features_array[0];
        features_array[2] = features_array[0] / features_array[1];
        output->addFloatA("features_array", features_array, sizeof(features_array)/sizeof(float));

        // takto float cv::Mat
        for (cv::Mat1f::iterator it = features_mat.begin(); it != features_mat.end(); it++) {
            *it = dummy_seed;
            dummy_seed += 0.1;
        }
        output->addCvMat("features_mat", features_mat);
        
        // [CHANGE] update stavu procesu
        ++cntDone;
        float progress = cntTotal ? ((float)cntDone)/cntTotal * 100.0 : 100.0;
        process->updateStateRunning(progress, video->getName(), pctrl);
        
        printf("mod_demo1: %.2f%%\n"
            "video %s\n"
            "  features array = %s\n"
            "  features mat(%d,%d) = %s\n",
            progress,
            video->getName().c_str(),
            toString(features_array, sizeof (features_array) / sizeof (float), 0).c_str(),
            features_mat.rows, features_mat.cols, toStringCvMat(features_mat).c_str());
    }

    // potvrdime vsechny inserty
    output->addExecute();

    delete video;
    delete output;

    // [CHANGE] update koncoveho stavu procesu
    if (!error) {
        process->updateStateFinished(pctrl);
    }
    else {
        process->updateStateError("this is error message", pctrl);
    }
}
