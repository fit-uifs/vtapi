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
        srand(time(NULL));
        
        // objekt pro vyslednou udalost
        IntervalEvent event;
        event.class_id = rand() % 50;
        event.group_id = rand() % 50;
        event.region.high = IntervalEvent::point(11,11);
        event.region.low = IntervalEvent::point(22,22);
        
        // iterujeme pres vstupni data, vyfiltrujeme si pouze nase video
        Interval *input = p->getInputData();
        input->filterBySequence(videoName);
        while (input->next()) {
            // ziskame predvypocitany vektor floatu
            int size = 0;
            float *features_array = input->getFloatA("features_array", size);
            if (features_array) {
                // provedeme dummy zpracovani
                for (int i = 0; i < size; i++) {
                    event.score += (int)(features_array[i] * 10);
                }
                delete[] features_array;
            }
            // ziskame predvypocitanou matici
            cv::Mat1f *features_mat = (cv::Mat1f *)input->getCvMat("features_mat");
            if (features_mat) {
                // provedeme dummy zpracovani
                for (cv::Mat1f::iterator it = features_mat->begin(); it != features_mat->end(); it++) {
                    event.score += (int) (*it * 10.0);
                }
                delete features_mat;
            }
        }
        delete input;
        
        // ulozime vystupni udalost pro 1. frame s nejakym nazvem
        output->add(video->getName(), 1, 1, "", "demouser", "random-generated");
        output->addIntervalEvent("event", event);
        output->addExecute();
        
        printf("mod_demo2: new event; class=%d,group=%d,score=%.2f\n",
            event.class_id, event.group_id, event.score);
    }
    delete video;
    
    delete output;
}