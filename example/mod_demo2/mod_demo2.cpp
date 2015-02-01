/*
 * Demo2 modul VTApi
 * by: Vojtech Froml (ifroml[at]fit.vutbr.cz)
 * 
 * Dummy modul; pro vstupni hodnoty (= vystupy demo1 modulu) a dane video
 * ulozi 1 udalost definovanou nazvem a cislem.
*/

#include <cstdlib>
#include <vtapi.h>

using namespace vtapi;

void do_work(Process *p, Dataset *ds);

int main(int argc, char** argv) {
    VTApi *vtapi = new VTApi(argc, argv);
    
    Process *process = vtapi->initProcess();
    if (process) {
        if (process->next()) {
            printf("mod_demo2: launched as process %s\n", process->getName().c_str());
            
            Dataset *dataset = vtapi->newDataset();
            dataset->next();
            
            // provedeme nejakou dummy praci
            do_work(process, dataset);
            
            delete dataset;
        }
        else {
            printf("mod_demo2: not launched\n");
        }
        
        delete process;
    }

    delete vtapi;
    
    return 0;
}

void do_work(Process *p, Dataset *ds) {
    // ziskame parametr naseho procesu
    std::string videoName = p->getParamString("video");
    
    // vystupni data, do kterych budeme ukladat vysledky
    Interval *output = p->getOutputData();
    output->next();
    
    // overime, ze zadane video je v datasetu
    Video *video = ds->newVideo(videoName);
    if (video->next()) {
        // dummy nazev vysledne udalosti
        std::string event_name = std::string("udalost_") + videoName;
        int event_arg = 0;
        
        // iterujeme pres vstupni data, vyfiltrujeme si pouze nase video
        Interval *input = p->getInputData();
        input->filterBySequence(videoName);
        while (input->next()) {
            // ziskame vypocitany vektor
            int size = 0;
            float *vals = input->getFloatA("vals", size);
            if (vals) {
                // provedeme dummy zpracovani
                for (int i = 0; i < size; i++) {
                    event_arg += (int)(vals[i] * 10);
                }
                delete[] vals;
            }
        }
        delete input;
        
        // ulozime vystupni udalost pro 1. frame s nejakym nazvem
        output->add(video->getName(), 1, 1, "", "demouser", "random-generated");
        output->addString("event_name", event_name.c_str());
        output->addInt("event_arg", event_arg);
        output->addExecute();
        
        printf("mod_demo2: event %s, arg = %d\n",
            event_name.c_str(), event_arg);
    }
    delete video;
    
    delete output;
}