/**
 * @file    method.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Method class
 */

#include <vtapi_global.h>
#include <common/vtapi_tkey.h>
#include <data/vtapi_process.h>
#include <data/vtapi_method.h>

using namespace vtapi;


Method::Method(const KeyValues& orig, const string& name) : KeyValues(orig) {
    thisClass = "Method";
    
    select = new Select(orig);
    select->from("public.methods", "*");
    select->whereString("mtname", name);
}

bool Method::next() {
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        this->method = this->getName();
        this->getMethodKeys();
    }

    return kv;
}

string Method::getName() {
    return this->getString("mtname");
}

Process* Method::newProcess(const string& name) {
    return (new Process(*this, name));
}

TKeys Method::getMethodKeys() {
    if (methodKeys.empty()) {
        // TODO: pretypovani na TKeyValues
        TKeyValues* parameters = NULL;
        
        KeyValues* kv = new KeyValues(*this);
        kv->select = new Select(*this);
        kv->select->from("public.methods_keys", "keyname");
        kv->select->from("public.methods_keys", "typname");
        kv->select->from("public.methods_keys", "inout");
        kv->select->whereString("mtname", this->method);

        while (kv->next()) {
            TKey mk;
            mk.key = kv->getString("keyname");
            mk.type = kv->getString("typname");
            mk.size = 0;   // 0 is for the definition
            mk.from = kv->getString("inout");
            methodKeys.push_back(mk);
        }

        delete (kv);
    }
    return methodKeys;
}









//
//
//
//
//
//
//
//
//    #include <vtapi.h>
//
//    /**
//     * Method Test1
//     */
//    class Test1 : public Method {
//    public:
//        int i = 1;
//
//        /** 
//         * Constructor
//         */
//        Test1();
//
//        /**
//         * Run - the method code
//         * @return 
//         */
//        bool run();
//    };
//
//
//    #include <methods/Test1/Test1.h>
//
//    Test1::Test1() : Method("Test1") {
//        /* enter your code here */
//    }
//
//    bool Test1::run() {
//        this->init();
//        /* enter your code here */
//
//        this->exit();
//    }
//
//    
//    
//
//void test() {
//
//    
//    // vytvoreni nove metody
//    Method m = Method->add("Test1");
//    // pridani implicitniho parametru
//    m->addInt("i", 1, "out");
//    // ulozeni zmen
//    m->addExecute();
//    
//    TKeyValues kv = m->getKeys();
//    
//    // VTApi vytvorilo soubor ./methods/test1/test1.h
//    m->loadCode();
//
//    // vytvoreni noveho procesu a jeho spusteni
//    Process p = m->newProcess("test");
//    p->run();
//    
//    // vyzvednuti navratove hodnoty
//    int i = p->getInt("i");
//
//}
//    
//int main(int argc, char **argv) {    
//
//    // VTApi entry point, using Dataset "search"
//    VTApi* vtapi = new VTApi(argc, argv);
//    Dataset* dataset = vtapi->newDataset("search");
//    dataset->next();
//    
//    Sequence* seq = dataset->newSequence("images");
//    
//    // code of the ColorLayout Method, using Selection "image"
//    Image* image = seq->newImage();
//    while (image->next()) {
//        int* color = colorLayout(image->getDataLocation());
//        image->setIntA("color", color, 32);
//    }
//    
//    // retrieve Image(s) according to their similarity to "Q.jpg"
//    Image* nearest = Image* image = new Image(&dataset, "img123");
//    nearest->select->from("image", "distance_square_int4(color, "
//            + toString(colorLayout("Q.jpg")) + ")");
//    nearest->select->orderby("distance_square_int4");
//    nearest->next(); // is the most similar image according to color
//    
//
//    VTApi* vtapi = new VTApi(argc, argv);
//    Dataset* dataset = vtapi->newDataset("train");
//    dataset->next();
//    
//
//    Mat samples; // cv::Mat of training feature vectors
//    Sequence* sequence = dataset->newSequence();
//    while (sequence->next()) { // for each video
//        Interval* track = new Interval(*sequence, "tracks");
//        while (track->next()) { // for each trajectory
//            Mat sample; // cv::Mat for feature vector of trajectory
//            float* features = track->getFloatA("features");
//            // ... read features and fill feature vector
//            samples.push_back(sample);
//        }
//    }
//    
//    // GMM-EM model and cluster labels
//    CvEM model, labels;
//    CvEMParams params; // EM parameters
//    // ... set EM parameters including number of clusters
//    model.train(samples, Mat(), params, labels);
//    // ... choose dataset and sequences according to code above
//    Interval* track = sequence->newInterval(1, 100);
//    while (track->next()) {
//        Mat sample;
//        // ... read features and fill feature vector
//        int cluster = (int) model.predict(sample); //get cluster label
//        track->setInt("cluster", cluster); // store cluster label
//    }    
//    
//}
//    
//




