/* 
 * File:   VTApi.h
 * Author: chmelarp
 *
 * Created on 29. sep 2011, 10:42
 */

#ifndef VTAPI_H
#define	VTAPI_H

// first, include internal classes
#include "commons.h"

// next, libraries - OpenCV, (libpq and) libpqtypes
#include "postgresql/libpqtypes.h"
#include <opencv2/core/core.hpp>


// virtual definition
class Dataset;
class Sequence;
class Interval;


/**
 * KeyValues storage class
 */
class KeyValues : public Commons {
public:
    KeyValues(const Commons& orig);
    KeyValues(const KeyValues& orig);
    virtual ~KeyValues();

    /**
     * The most used function of the VTApi - nextRow or simply next
     * @return this or null
     */
    KeyValues* next();
    KeyValues* rewind();

    long getRowActual();
    long getRowNumber();

    // getters

    String getString(String key);
    String getString(int pos);

    int getInt(String key);
    int getInt(int pos);
    int* getIntA(String key, size_t& size);
    int* getIntA(int pos, size_t& size);
    std::vector<int> getIntV(int pos);
    std::vector<int> getIntV(String key);

    float getFloat(String key);
    float getFloat(int pos);
    float* getFloatA(String key, size_t& size);
    float* getFloatA(int pos, size_t& size);

    // setters

    bool setString(String key, String value);
    bool setInt(String key, String value);
    bool setIntA(String key, int* value, size_t size);
    bool setFloat(String key, String value);
    bool setFloatA(String key, float_t value, size_t size);


//    void test(const KeyValues& orig);

protected:
    // this should be overriden (type) where possible
    KeyValues* parent;
    // maintain a list of all possible elements
    std::map<String,String> keys;
    // TODO: discuss map, recursion etc.

    int position;       // initialized to -1 by default
    // this should be it here or should it be there?
    PGresult* res;

    // Inherited from Commons:
    // Connector* connector; // this was most probably inherited
    // Logger* logger;

    // bool isDoom;         // true

};


/**
 *
 */
class Dataset : public KeyValues {
public:
    Dataset(const KeyValues& orig);
    Dataset(const Dataset& orig);
    virtual ~Dataset();

    String getName();
    String getLocation();

    int getSize();

    Sequence* newSequence();
protected:

};


/**
 * A Sequence class manages videos and images
 */
class Sequence : public KeyValues {
public:
    Sequence(const KeyValues& orig);
    Sequence(const Sequence& orig);
    virtual ~Sequence();

    String getName();
    String getLocation();

    Interval* newInterval();

    cv::Mat getImage();

protected:
    bool openVideo(const String& name);
    bool openImage(const String& name);

    String file_name_video;
    String file_name_image;

};


/**
 * Interval is equivalent to an interval of images
 */
class Interval : public KeyValues {
public:
    Interval(const KeyValues& orig);
    Interval(const Interval& orig);
    virtual ~Interval();

    String getSequence();
    int getStartTime();
    int getEndTime();
    String getLocation();
    std::vector<int> getTags();

protected:

};


/**
 */
class MethodKeys : public KeyValues {
public:
    MethodKeys(const Dataset& orig);
    MethodKeys(const Dataset& orig, const String& methodName, const String& inout);
    MethodKeys(const MethodKeys& orig);
    virtual ~MethodKeys();

    void getMethodKeyData(const String& methodName, const String& inout);
    int getMethodKeyDataSize();

    String getKeyname();
    String getTypname();
};

/**
 */
class Method : public KeyValues {
private:
    MethodKeys* methodkeys;

public:
    Method(const Dataset& orig);
    Method(const Method& orig);
    virtual ~Method();

    String getMtname();

    void getInputData();
    void getOutputData();
private:
    void printData(const String& inout);
};

/**
 *
 */
class Process : public KeyValues {
public:
    Process(const Dataset& orig);
    Process(const Process& orig);
    virtual ~Process();

    String getPrsname();
    String getInputs();
    String getOutputs();
    
    void printProcesses();
};


class Test {
    Dataset* dataset;
    Sequence* sequence;
    Interval* interval;
public:
    Test(Dataset& orig);
    virtual ~Test();
    
    void testDataset();
    void testSequence();
    void testInterval();
    void testKeyValues();
    void testMethod();
    void testProcess();
    void testAll();
};


/**
 * VTApi class manages Commons and processes args[]
 * This is how to begin
 *
 * TODO: include http://www.gnu.org/s/gengetopt/gengetopt.html
 *       special interest to the configuration files is needed
 */
class VTApi {
public:
    /**
     * Constructor recomended (in the future)
     * @param argc
     * @param argv
     */
    VTApi(int argc, char** argv);
    VTApi(const String& connStr, const String& location, const String& user, const String& password);
    VTApi(const Commons& orig);
    VTApi(const VTApi& orig);
    virtual ~VTApi();


    /**
     * An command-line driven api interface (just basic so far)
     * @param argc
     * @param argv
     * @return
     */
    int main(int argc, char** argv);

    /**
     * Use this function instead of main(int, char**) only in case constructor was
     * VTApi(int argc, char** argv);
     *
     * @param name
     * @return
     */
    int run();

    /**
     * For testing KeyValues
     */
    void testKeyValues();


    /**
     * This is how to continue...
     * @return
     */
    Dataset* newDataset(const String& name = "");

    Commons* commons;
    
protected:
    String getWord(String& line);
};

#endif	/* VTAPI_H */

