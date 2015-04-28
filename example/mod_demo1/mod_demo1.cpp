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
#include <pthread.h>
#include <unistd.h>
#include <vtapi.h>

using namespace vtapi;

// [CHANGE] synchronizacni pomucka
typedef struct _SYNC_T
{
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    ProcessControl::COMMAND_T command;

    _SYNC_T()
    {
        pthread_mutex_init(&mtx, NULL);
        pthread_cond_init(&cond, NULL);
        command = ProcessControl::COMMAND_NONE;
    }

    ~_SYNC_T()
    {
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mtx);
    }
} SYNC_T;


// [CHANGE] callback pro reakce na kontrolni prikazy z launcheru (kontrolni vlakno)
void ControlCallback(ProcessControl::COMMAND_T command, void *context);
// [CHANGE] overeni, zda nebyl prijat kontroli prikaz
ProcessControl::COMMAND_T check_command(SYNC_T& sync);
// [CHANGE] vypocetni funkce (vypocetni vlakno)
void do_work(Process *process, ProcessControl *pctrl, Dataset *dataset, SYNC_T& sync);



///////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    // instanciace VTApi, pripojeni k DB
    VTApi *vtapi = new VTApi(argc, argv);
    
    // ziskani proces objektu, jehoz jsme instanci
    // [CHANGE] - odstranena nutnost nasledujici funkce next()
    Process *process = vtapi->initProcess();
    if (process) {
        printf("mod_demo1: launched as process %s\n", process->getName().c_str());

        // synchronizacni objekty
        SYNC_T sync;
        
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
    
    pthread_mutex_lock(&sync->mtx);
    sync->command = command;
    pthread_cond_signal(&sync->cond);
    pthread_mutex_unlock(&sync->mtx);
}

// [CHANGE] overeni, zda nebyl prijat kontroli prikaz
ProcessControl::COMMAND_T check_command(SYNC_T& sync)
{
    ProcessControl::COMMAND_T cmd = ProcessControl::COMMAND_NONE;
    struct timespec tw = {0};
    
    pthread_mutex_lock(&sync.mtx);
    if (pthread_cond_timedwait(&sync.cond, &sync.mtx, &tw) == 0) {
        cmd = sync.command;
        sync.command = ProcessControl::COMMAND_NONE;
    }
    pthread_mutex_unlock(&sync.mtx);

    switch (cmd)
    {
    case ProcessControl::COMMAND_RESUME:
        printf("mod_demo1: RESUME command received\n");
        break;
    case ProcessControl::COMMAND_SUSPEND:
        printf("mod_demo1: SUSPEND command received\n");
        break;
    case ProcessControl::COMMAND_STOP:
        printf("mod_demo1: STOP command received\n");
        break;
    default:
        break;
    }
    
    return cmd;
}

// vypocetni funkce (vypocetni vlakno)
void do_work(Process *process, ProcessControl *pctrl, Dataset *dataset, SYNC_T& sync)
{
    // ziskame parametry naseho procesu
    int param1 = process->getParamInt("param1");
    double param2 = process->getParamDouble("param2");

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
    //size_t cntTotal = video->count(); // zatim nefunguje
    size_t cntTotal = 10;
    size_t cntDone  = 0;
    bool error = false;

    // [CHANGE] prvni notifikace
    process->updateStateRunning(0, video->getName(), pctrl);

    // projdeme vsechna videa datasetu
    while (video->next()) {
        
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
        
        // potvrdime insert
        output->addExecute();
        
        // [CHANGE] update stavu procesu
        ++cntDone;
        float progress = cntTotal ? ((float)cntDone)/cntTotal * 100.0 : 100.0;
        process->updateStateRunning(progress, video->getName(), pctrl);
        
        printf("mod_demo1: %.2f%%\n"
            "video %s\n"
            "  features array = %s\n"
            "  features mat(%d,%d) = %s\n",
            cntDone/(float)cntTotal,
            video->getName().c_str(),
            toString(features_array, sizeof (features_array) / sizeof (float), 0).c_str(),
            features_mat.rows, features_mat.cols, toStringCvMat(features_mat).c_str());
    }
    
    delete video;
    delete output;

    // [CHANGE] update koncoveho stavu procesu
    if (!error) {
        float progress = cntTotal ? ((float) cntDone) / cntTotal * 100.0 : 100.0;
        process->updateStateFinished(progress, pctrl);
    }
    else {
        process->updateStateError("this is error message", pctrl);
    }

}
