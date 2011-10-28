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
#include <map>

// next, libraries - OpenCV, (libpq and) libpqtypes
#include "postgresql/libpqtypes.h"
#include <opencv2/core/core.hpp>


// virtual definition
class Dataset;
class Sequence;
class Interval;





/**
 * This is a class where queries will be constructed
 * Mechanism: TBD
 *
 * Errors 21*
 *
 *//***************************************************************************/
class Select : public Commons {
public:
    Select(const Commons& commons, QueryType type = GENERIC);
   
    /**
     * This expands the query, so you can check it before the execution
     * @return
     */
    String getQuery();

    bool prepareQuery();
    bool executeQuery();

    // this is a tuple table and column name
    // TODO: this->from["intervals"] = "*";
    // FIXME: AS
    std::multimap<String, String> fromList;

    /**
     * This is to specify the from clause and the select (column) list
     *
     * select->from("public.datasets", "*");
     *
     * @param table
     * @param column
     * @return
     */
    bool from(const String& table, const String& column);

    // FIXME: these tuples of WHERE should be tripples/quadruples including type
    // TODO: this->whereString["features"] = "NULL";
    // std::multimap<String, String> whereString;
    // std::multimap<String, int> whereInt;
    // std::multimap<String, float> whereFloat;

    /**
     *
     * @param column
     * @param value
     * @return
     */
    bool whereString(const String& table, const String& column, const String& value);
    String where;   // FIXME: see above :(
    String groupby;
    String orderby;
    
// protected:
    String query;

    QueryType type;
    PGresult* res;
    int pos;

    int limit;
    int offset;
};

/**
 * This is a class to construct and execute INSERT queries.
 *
 * Command syntax: (array values are comma-separated)
 * insert dataset name=.. location=..
 * insert sequence name=.. [seqnum=.. location=.. seqtype=..]
 * insert interval sequence=... t1=.. t2=.. [location=.. tags=.. svm=..]
 * insert method name=.. [key= type= inout=..]*
 * insert process name=.. method=.. inputs=.. outputs=..
 * insert selection name=.. --
 *
 */
class Insert {
public:
    enum InsertType {GENERIC, DATASET, SEQUENCE, INTERVAL, PROCESS, METHOD, SELECTION};

    Insert(Dataset* ds);
    ~Insert();
    /**
     * Set dataset (schema) on which the insert is executed
     * @param dsname Name of the dataset
     */
    void setDataset(Dataset* newdataset);
    /**
     * Set what to insert into dataset
     * @param newtype {NONE, DATASET, SEQUENCE, INTERVAL, PROCESS, METHOD, SELECTION}
     */
    void setType(InsertType newtype);
    /**
     * Add arguments to insert query in string form
     * @param param argument in 'key=value' format, eg.: 'name=process1'
     * @return returns 0 on success, -1 if argument already exists or format is wrong
     */
    int addParam(String param);
    /**
     * Add arguments to insert query in key/value form
     * @param key argument name, eg.: 'name'
     * @param value argument value, eg.: 'process1'
     * @return returns 0 on success, -1 if argument already exists
     */
    int addParam(String key, String value);
    /**
     * Clears query arguments
     */
    void clear();
    /**
     * Executes query
     * @return Success value
     */
    bool execute();

protected:
    Connector* connector;
    Dataset* dataset;
    InsertType type;
    std::map<String,String> params;

    /**
     * Gets value of argument from params map
     * @param pname Argument key
     * @return Argument value
     */
    String getParam(String pname);
    /**
     * Fills PGarray structure with array of int arguments in string form
     * @param arrayParam Comma-separated string of values
     * @param arr Array structure to fill
     */
    void getIntArray(String arrayParam, PGarray* arr);
    /**
     * Fills PGarray structure with array of float arguments in string form
     * @param arrayParam Comma-separated string of values
     * @param arr Array structure to fill
     */
    void getFloatArray(String arrayParam, PGarray* arr);
    /**
     * Checks whether interval location points to existing file
     * @param seqname Name of the sequence containing interval
     * @param intlocation Interval filename
     * @return
     */
    bool checkLocation(String seqname, String intlocation);
    PGtimestamp getTimestamp();

};


/**
 * KeyValues storage class
 *
 * Errors 50*
 */
class KeyValues : public Commons {
public:
    KeyValues(const Commons& orig);
    KeyValues(const KeyValues& orig);
    virtual ~KeyValues();

    /**
     * Select is (to be) pre-filled byt the constructor
     */
    Select* select;
    Insert* insert;

    /**
     * The most used function of the VTApi - next (row)
     * @return this or null
     */
    KeyValues* next();

    long getRowActual();
    long getRowNumber();

    // getters (Select)

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

    // setters (Update)
    // TODO: overit jestli a jak funguje... jako UPDATE?

    bool setString(String key, String value);
    bool setInt(String key, String value);
    bool setInt(String key, int value);
    bool setIntA(String key, int* value, size_t size);
    bool setFloat(String key, String value);
    bool setFloat(String key, float value);
    bool setFloatA(String key, float_t value, size_t size);

    // adders (Insert)
    // TODO: implement

    bool addString(String key, String value);
    bool addInt(String key, String value);
    bool addInt(String key, int value);
    bool addIntA(String key, int* value, size_t size);
    bool addFloat(String key, String value);
    bool addFloat(String key, float value);
    bool addFloatA(String key, float_t value, size_t size);


    // TODO: u kazde tridy add vytvori objekt te tridy se stejnymi keys, ale bez hodnot a ty se naplni jako set

//    void test(const KeyValues& orig);

protected:
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
 * This class should always be on the path of your programm...
 *
 * Error codes 30*
 */
class Dataset : public KeyValues {
public:
    /**
     * This is the recommended constructor.
     *
     * WARNING: you can ommit the @name only in these cases:
     *          1) Don't know the name -> use next
     *          2) The dataset is in your vtapi.conf
     *
     * @param orig
     * @param name
     */
    Dataset(const KeyValues& orig, const String& name = "");
    Dataset(const Dataset& orig);
    virtual ~Dataset();

    String getName();
    String getLocation();

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

    bool add(String name="");

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

    bool add(String name="");

protected:

};


/**
 * // TODO: zvazit jestli nechat + komentar nutny
 */
class MethodKeys : public KeyValues {
public:
    MethodKeys(const Dataset& orig);
    MethodKeys(const Dataset& orig, const String& methodName, const String& inout = "");
    MethodKeys(const MethodKeys& orig);
    virtual ~MethodKeys();

    /**
     * TODO: 
     * @param methodName
     * @param inout
     */
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

    bool add(String name="");
    
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
     * This is how to continue after creating the API class...
     * @return
     */
    Dataset* newDataset(const String& name = "");
    Method* newMethod(const String& name = "");
    Process* newProcess(const String& name = "");

    /**
     * Commons are common objects to the project.
     */
    Commons* commons;
    
protected:
    /**
     * Command entered through program arguments
     */
    String cmdline;

    String getWord(String& line);

};

#endif	/* VTAPI_H */

