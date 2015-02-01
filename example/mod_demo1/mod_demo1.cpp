/*
 * Demo1 modul VTApi
 * by: Vojtech Froml (ifroml[at]fit.vutbr.cz)
 * 
 * Dummy modul; pro kazde video v datasetu ulozi vektor 3 floatu, jehoz hodnoty
 * zavisi na 2 vstupnich parametrech.
*/

#include <cstdlib>
#include <vtapi.h>

using namespace vtapi;

void do_work(Process *process, Dataset *dataset);

int main(int argc, char** argv) {
    VTApi *vtapi = new VTApi(argc, argv);
    
    Process *process = vtapi->initProcess();
    if (process) {
        if (process->next()) {
            printf("mod_demo1: launched as process %s\n", process->getName().c_str());
            
            Dataset *dataset = vtapi->newDataset();
            dataset->next();
            
            // provedeme nejakou dummy praci
            do_work(process, dataset);
            
            delete dataset;
        }
        else {
            printf("mod_demo1: not launched\n");
        }
        
        delete process;
    }

    delete vtapi;
    
    return 0;
}

void do_work(Process *process, Dataset *dataset) {
    // ziskame parametry naseho procesu
    int param1 = process->getParamInt("param1");
    int param2 = process->getParamInt("param2");
    
    // vystupni data, do kterych budeme ukladat vysledky
    Interval *output = process->getOutputData();
    output->next();
    
    // vysledne pole floatu
    float vals[3] = {0};
    
    // projdeme vsechna videa datasetu
    Video *video = dataset->newVideo();
    while (video->next()) {
        // otevreni + zpracovani videa (zatim pouze cesta)
        //std::string path = video->getLocation();
        
        // anotace bude treba pro prvnich 10 framu
        output->add(video->getName(), 1, 10, "", "demouser", "random-generated");
        
        // spocitame dummy vysledek a ulozime
        vals[0] += param1 / 1000.0;
        vals[1] += param2 / 500.0 - vals[0];
        vals[2] = vals[0] / vals[1];
        output->addFloatA("vals", vals, 3);
        
        // potvrdime insert
        output->addExecute();
        
        printf("mod_demo1: video %s; {%.3f,%.3f,%.3f}\n",
            video->getName().c_str(), vals[0], vals[1], vals[2]);
    }
    delete video;
    
    delete output;
}
