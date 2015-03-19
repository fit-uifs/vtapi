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
    double param2 = process->getParamDouble("param2");
    
    // vystupni data, do kterych budeme ukladat vysledky
    Interval *output = process->getOutputData();
    output->next();
    
    // vysledne pole floatu a matice
    float features_array[3] = {0};
    cv::Mat1f features_mat(3, 2);
    float dummy_seed = 0.0;

    // projdeme vsechna videa datasetu
    Video *video = dataset->newVideo();
    while (video->next()) {
        // otevreni + zpracovani videa (zatim pouze cesta)
        //std::string path = video->getLocation();
        
        // anotace bude treba pro prvnich 10 framu
        output->add(video->getName(), 1, 10, "", "demouser", "random-generated");
        
        // spocitame dummy vysledky a ulozime
        
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
        
        printf("mod_demo1: video %s\n"\
            "  features array = %s\n"\
            "  features mat(%d,%d) = %s\n",
            video->getName().c_str(),
            toString(features_array, sizeof (features_array) / sizeof (float), 0).c_str(),
            features_mat.rows, features_mat.cols, toStringCvMat(features_mat).c_str());
    }
    delete video;
    
    delete output;
}
