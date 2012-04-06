/* 
 * File:   VTCli.h
 * Author: vojca
 *
 * Created on November 3, 2011, 7:12 PM
 */

#ifndef VTCLI_H
#define	VTCLI_H

#include "vtapi.h"
#include <map>

class VTCli {
public:
    VTCli();
    VTCli(const VTCli& orig);
    VTCli(const VTApi& api);
    VTCli(int argc, char** argv);

    virtual ~VTCli();

    int run();
    int printHelp();
    int printHelp(const String& what);

protected:
    VTApi* vtapi;
    bool interact;
    String cmdline;
    std::map<String,String> helpStrings;

    int processArgs(int argc, char** argv);
    String getWord(String& line);
    String getCSV(String& word);
    std::pair<String,String> createParam(String word);

    void queryCommand(String& line);
    void selectCommand(String& line);
    void insertCommand(String& line);
    void updateCommand(String& line);
    void deleteCommand(String& line);
    void showCommand(String& line);

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

#endif	/* VTCLI_H */

