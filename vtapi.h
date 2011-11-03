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


// virtual definitions
class Dataset;
class Sequence;
class Interval;
class Method;
class Process;


/**
 * This is to represent the keys and fields in queries
 * ... just for the feeling (and vectors, of course)
 * You can use size=-1 for for NULL :)
 */
class TKey {
public:
    String type;
    String key;
    int size;       // you can use -1 for NULL :)
    String from;

    TKey() : size(-1) {};
    TKey(const String& type, const String& key, const int size, const String& from = "")
            : type(type), key(key), from(from), size(size) {};

    String print();
};


/**
 * This is a virtual query class
 * TODO: It will be used for delayed queries (store())
 * @see http://libpqtypes.esilo.com/
 *
 * Error codes 20*
 */ // ********************************************************************** //
class Query : public Commons {
public:
    Query(const Commons& commons, const String& query = "", PGparam *param = NULL);
    ~Query();

    /**
     * This expands the query, so you can check it before the execution
     * @return
     */
    virtual String getQuery();

    /**
     * This will commit your query
     * @return 
     */
    bool execute();
    // TODO? virtual bool prepare();

    /** This is a flag wheather the query was executed after any change */
    bool executed;

    /**
     * This may be hazardeous for someone...
     * marked as deprecated, because there is no discouraged mark
     * @deprecated
     * @param key
     * @return success
     */
    bool keyValue(const TKey& key);

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times as:
     *
     *
     * @param key
     * @param value
     * @return success
     */
    bool keyString(const String& key, const String& value, const String& from = "");
    bool keyStringA(const String& key, const String* values, const int size, const String& from = "");
    bool keyInt(const String& key, const int& value, const String& from = "");
    bool keyIntA(const String& key, const int* values, const int size, const String& from = "");
    bool keyFloat(const String& key, const float& value, const String& from = "");
    bool keyFloatA(const String& key, const float* values, const int size, const String& from = "");
    
    /** This is where those keys are stored */
    std::vector<TKey> keys;
    PGparam* param;

    /** This is used for (direct) queries */
    String queryString;

    /** This is where results are (to be) not NULL */
    PGresult* res;
};

/**
 * This is a class where queries are (to be) constructed
 * Mechanism: TBD
 *
 * Errors 21*
 *
 */// *********************************************************************** //
class Select : public Query {
public:
    Select(const Commons& commons, const String& queryString = "", PGparam *param = NULL);
    ~Select();

    /**
     * This expands the query, so you can check it before the execution
     * @return
     */
    String getQuery();

    // TODO Tomas: implement

    /**
     * This is to specify the from clause and the select (column) list
     * It may be called more times as:
     * @code select->from("public.datasets", "*");
     *
     * @param table
     * @param column
     * @return
     */
    bool from(const String& table, const String& column);
    // this is a tuple table and column name
    // TODO: this->from["intervals"] = "*";
    // FIXME: use keys
    std::multimap<String, String> fromList;

    // FIXME: use keys instead
    /**
     * This is a persistent function to add where clauses
     * It can be called several times as:
     *
     * @param column
     * @param value
     * @return success
     */
    bool whereString(const String& key, const String& value, const String& table = "");
    String where;   // FIXME: see above :(

    String groupby;
    String orderby;

    int limit;
    int offset;
};

/**
 * This is a class where queries are (to be) constructed
 * Mechanism: TBD
 *
 * Error codes 22*
 *
 */// *********************************************************************** //
class Insert : public Query {
public:
    Insert(const Commons& commons, const String& insertString = "", PGparam *param = NULL);
    ~Insert();

    /**
     * This is to specify the (single) table to be inserted in
     * @param table
     * @return success
     */
    bool into(const String& table);
    String intoTable;

    /**
     * This expands the query, so you can check it before the execution
     * @return
     */
    String getQuery();
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
class CLIInsert {
public:
    enum InsertType {GENERIC, DATASET, SEQUENCE, INTERVAL, PROCESS, METHOD, SELECTION};

    CLIInsert(Commons& orig);
    ~CLIInsert();
    /**
     * Set what to insert into dataset
     * @param newtype {NONE, DATASET, SEQUENCE, INTERVAL, PROCESS, METHOD, SELECTION}
     */
    void setType(InsertType newtype);
    /**
     * Adds argument to insert query in pair <Key,Value> form
     * @param param pair of argument key and value
     * @return 0 on success, -1 if key/value is empty or key already exists
     */
    int addParam(String param);
    /**
     * Adds argument to insert query
     * @param key argument name, eg.: 'name'
     * @param value argument value, eg.: 'process1'
     * @return 0 on success, -1 if key/value is empty or key already exists
     */
    int addParam(std::pair<String,String> param);
    /**
     * Adds argument to insert query
     * @param key argument name, eg.: 'name'
     * @param value argument value, eg.: 'process1'
     * @return 0 on success, -1 if key/value is empty or key already exists
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
    String dataset;
    Connector* connector;
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
 * Errors 30*
 */
class KeyValues : public Commons {
public:
    // FIXME: proc jsou tu nutne 2 stejne konstruktory?
    KeyValues(const Commons& orig);
    KeyValues(const KeyValues& orig);

    /**
     * This destroys the KeyValues
     * It raises warning when there was something left not-excecuted (some collaborants left)
     */
    virtual ~KeyValues();

    /**
     * The most used function of the VTApi - next (row)
     * @return this or null
     */
    KeyValues* next();

    // get a list of all possible columns
    std::map<String,String> getKeys();

    void print();
    void printAll();

    // getters (Select)
    String getString(String key);
    String getString(int pos);

    int getInt(String key);
    int getInt(int pos);
    int* getIntA(String key, int& size);
    int* getIntA(int pos, int& size);
    std::vector<int> getIntV(int pos);
    std::vector<int> getIntV(String key);

    float getFloat(String key);
    float getFloat(int pos);
    float* getFloatA(String key, int& size);
    float* getFloatA(int pos, int& size);

    int getOid(String key);
    String getName(String key);

    // setters (Update)
    // TODO: overit jestli a jak funguje... jako UPDATE?

    bool setString(String key, String value);
    bool setInt(String key, String value);
    bool setInt(String key, int value);
    bool setIntA(String key, int* value, int size);
    bool setFloat(String key, String value);
    bool setFloat(String key, float value);
    bool setFloatA(String key, float_t value, int size);

    // adders (Insert)
    // TODO: implement
    bool addString(String key, String value);
    bool addInt(String key, String value);
    bool addInt(String key, int value);
    bool addIntA(String key, int* value, int size);
    bool addFloat(String key, String value);
    bool addFloat(String key, float value);
    bool addFloatA(String key, float_t value, int size);


    // TODO Tomas: u kazde tridy add vytvori objekt te tridy se stejnymi keys, ale bez hodnot a ty se naplni jako set

    /**
     * Select is (to be) pre-filled byt the constructor
     */
    Select* select;
    int pos;       // initialized to -1 by default

    Insert* insert;
    // some other inherited from Commons
};


/**
 * This class should always be on the path of your programm...
 *
 * Error codes 31*
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

    /**
     * Over-loading next();
     * @return
     */
    bool next();

    String getName();
    String getLocation();

    Sequence* newSequence(const String& name = "");
    Method* newMethod(const String& name = "");
    Process* newProcess(const String& name = "");

protected:

};


/**
 * A Sequence class manages videos and images
 *
 * Error codes 32*
 */
class Sequence : public KeyValues {
public:
    Sequence(const KeyValues& orig, const String& name = "");

    bool next();

    String getName();
    String getLocation();

    bool add(String name, String location);

    Interval* newInterval(const int t1 = -1, const int t2 = -1);
    Process* newProcess(const String& name = "");

    cv::Mat getImage();

protected:
    bool openVideo(const String& name);
    bool openImage(const String& name);

    String file_name_video;
    String file_name_image;

};


/**
 * Interval is equivalent to an interval of images
 *
 * Error codes 33*
 */
class Interval : public KeyValues {
public:
    Interval(const KeyValues& orig, const String& selection = "intervals");

    String getSequence();
    int getStartTime();
    int getEndTime();

    /**
     * This is most probably what you always wanted...
     * @return
     */
    String getDataLocation();
    /** This is here just for image name */
    String getLocation();

    bool add(const String& sequence, const int t1, const int t2 = -1, const String& location = "");

protected:

};

/**
 * This represents images
 *
 * Error codes 339*
 */
class Image : public Interval {
public:
    Image(const KeyValues& orig, const String& selection = "intervals");

    int getTime();
    bool add(const String& sequence, const int t, const String& location);

protected:

};


/**
 *
 * Error codes 35*
 */
class Method : public KeyValues {
public:
    Method(const KeyValues& orig, const String& name = "");
    virtual ~Method();

    String getName();

    bool next();

    Process* newProcess(const String& name = "");

    /**
     * This is to refresh the methodKeys vector
     * @return vector<TKey>
     */
    std::vector<TKey> getMethodKeys();
    std::vector<TKey> methodKeys;

private:
    void printData(const String& inout);
};

/**
 *
 * Error codes 36*
 */
class Process : public KeyValues {
public:
    Process(const KeyValues& orig, const String& name = "");
    virtual ~Process();

    String getName();
    String getInputs();
    String getOutputs();

    // TODO: o tohle bych se ani nepokousel
    // bool add(String name="");

    bool next();

    Interval* newInterval(const int t1 = -1, const int t2 = -1);
    Sequence* newSequence(const String& name = "");

// TODO:    void print();
};


/**
 * VTApi class manages Commons and processes args[]
 * This is how to begin
 *
 * TODO: include http://www.gnu.org/s/gengetopt/gengetopt.html
 *       special interest to the configuration files is needed
 *
 * Error codes 60*
 */ // ********************************************************************** //
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
     * For testing and learning purposes
     */
    void test();


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

    bool interact;
    String getWord(String& line);
    String getCSV(String& word);
    std::pair<String,String> createParam(String word);
};


// this is just a development branch...
/**
 * A generic class for storing a single keyvalue type
 * It uses std::copy (memcpy) to maintain the object data (except pointer targets)
 * WARNING: use PDOs only ... @see http://en.wikipedia.org/wiki/Plain_old_data_structure
 * WARNING: if you use pointers, you shouldn't free them
 * You can use size=-1 for NULL :)
 *
 * @see http://www.cplusplus.com/doc/tutorial/templates/
 * @see http://stackoverflow.com/questions/2627223/c-template-class-constructor-with-variable-arguments
 * @see http://www.cplusplus.com/reference/std/typeinfo/type_info/
 */
template <class T>
class TKeyValue : public TKey {
public:
    /** This attribute is there for validation */
    String typein;
    T* values;

    TKeyValue() : TKey(), values(NULL) {};
    TKeyValue(const String& type, const String& key, const T& value, const String& from = "")
            : TKey(type, key, 1, from) {
        values = new T[1];
        values[0] = value;
        typein = typeid(this->values).name();
    }
    TKeyValue (const String& type, const String& key, const T* values, const int size, const String& from = "")
            : TKey(type, key, size, from) {
        this->values = new T[this->size];
        std::copy(values, values+size, this->values);
        // memcpy(this->values, values, size*sizeof(values));
        typein = typeid(values).name();
    }

    ~TKeyValue () {
        destructall(values);
    }

    String print();
};


// FIXME: tohle kdyz dam jinam, tak je to v haji - proc?
template <class T>
String TKeyValue<T>::print() {
    String ret = "TKeyValue<" + String(typeid(values).name()) + "> type=" + type +
            ", key=" + key + ", from=" + from + ", size=" + toString(size) + ", values=\n";
    if (values && size > 0) {
        for(int i=0; i < size; ++i) {
            ret += toString(values[i]) + ", ";
        }
    }
    else ret += "NULL  ";
    ret = ret.erase(ret.length()-2) + "\n";

    std::cout << ret;
    return (ret);
}

#endif	/* VTAPI_H */

