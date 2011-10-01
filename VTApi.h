/* 
 * File:   VTApi.h
 * Author: chmelarp
 *
 * Created on 29. sep 2011, 10:42
 */

#ifndef VTAPI_H
#define	VTAPI_H

// first, include internal classes
#include "settings.h"
#include "commons.h"

// next, libraries - OpenCV, (libpq and) libpqtypes
#include "postgresql/libpqtypes.h"
#include <opencv2/core/core.hpp>



/**
 * KeyValues storage class
 */
class KeyValues : Commons {
public:
    KeyValues(const Commons& other);
    KeyValues(const KeyValues& orig);
    virtual ~KeyValues();

    /**
     * The most used function of the VTApi - nextRow or simply next
     * @return
     */
    bool next();

    long getRowActual();
    long getRowNumber();

    // getters

    String getString(String key);
    String getString(int pos);

    int getInt(String key);
    int getInt(int pos);
    int* getIntA(String key, size_t& lenght);
    int* getIntA(int pos, size_t& lenght);

    float getFloat(String key);
    float getFloat(int pos);
    float* getIntA(String key, size_t& lenght);
    float* getIntA(int pos, size_t& lenght);

    // setters
    
    bool setString(String key, String value);
    bool setInt(String key, String value);
    bool setIntA(String key, int* value, size_t size);
    bool setFloat(String key, String value);
    bool setFloatA(String key, float_t value, size_t size);

private:
    // Inherited from Commons:
    // Connector* connector; // this was most probably inherited
    // Logger* logger;

    // bool isDoom;         // true
};


/**
 *
 */
class Interval : KeyValues {
public:
    Interval(const KeyValues& other);
    Interval(const Interval& orig);
    virtual ~Interval();
private:

};


/**
 * A Sequence class manages videos and images
 */
class Sequence : KeyValues {
public:
    Sequence(const KeyValues& other);
    Sequence(const Sequence& orig);
    virtual ~Sequence();

    bool openVideo(const String& name);
    bool openImage(const String& name);
    cv::Mat getImage();

private:
    String file_name_video;
    String file_name_image;

};


/**
 *
 */
class Dataset : KeyValues {
public:
    Dataset(const KeyValues& other);
    Dataset(const Dataset& orig);
    virtual ~Dataset();

    String nextDataset();
    Sequence getSequences();

    String getDsName();
    String getDsLocation();

private:

};


/**
 *
 */
class Process : KeyValues {
public:
    Process(const KeyValues& other);
    Process(const Process& orig);
    virtual ~Process();

    String nextProcess();

private:

};


/**
 * VTApi class manages Commons and processes args[]
 */
class VTApi : Commons {
public:
    VTApi(int argc, char** argv);
    VTApi(const String& connStr);
    VTApi(const Commons& other);
    VTApi(const VTApi& orig);
    virtual ~VTApi();

    int main(int argc, char** argv);

    Dataset* newDataset(String name);


private:

};

#endif	/* VTAPI_H */

