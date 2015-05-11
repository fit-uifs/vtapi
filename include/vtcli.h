/**
 * @file
 * @brief VTCli is a command line tool for easy manipulation with data structures of a %VTApi library.
 *
 * @copyright Brno University of Technology &copy; 2011 &ndash; 2013
 *
 * VTCli is distributed under BUT OPEN SOURCE LICENCE (Version 1).
 * This licence agreement provides in essentials the same extent of rights as the terms of GNU GPL version 2 and Software fulfils the requirements of the Open Source software.
 *
 * @author
 * Vojtěch Fröml, xfroml00 (at) stud.fit.vutbr.cz
 *
 *
 * @todo @b code: dodelat
 */

#ifndef VTCLI_H
#define	VTCLI_H

#include <string>
#include <map>
#include <set>
#include <list>

#include "vtapi.h"

using namespace vtapi;

/**
 * @brief Main VTCli class
 */
class VTCli {
public:

    /**
     * VTCli constructor with command line arguments
     */
    VTCli();
    /**
     * Destructor
     */
    virtual ~VTCli();

    /**
     * Runs VTCli tool in one of the two modes:
     *  interactive - asks user for commands (default mode)
     *      eg.: ./vtcli --config=./vtapi.conf
     *  one-time    - runs one time command (toggled when command line arguments contain an unknown string)
     *      eg.: ./vtcli --config=./vtapi.conf insert sequence /mnt/data/seq/video.mpg
     * @param argc command line argument count
     * @param argv command line arguments
     * @return error code
     */
    int run(int argc, char** argv);


protected:
    typedef enum
    {
        CMD_NONE = 0,
        CMD_SELECT,
        CMD_INSERT,
        CMD_DELETE,
        CMD_LOAD,
        CMD_STATS,
        CMD_CONTROL,
        CMD_TEST,
        CMD_HELP,
        CMD_EXIT
    } VTCLI_COMMAND;
    
    typedef enum
    {
        OBJ_NONE        = 0,
        OBJ_DATASET     = (1),
        OBJ_SEQUENCE    = (1 << 1),
        OBJ_INTERVAL    = (1 << 2),
        OBJ_PROCESS     = (1 << 3),
        OBJ_METHOD      = (1 << 4),
        OBJ_METHODKEYS  = (1 << 5),
        OBJ_ALL         = 0xFF
    } VTCLI_OBJECT;
    
    typedef enum
    {
        PAR_NONE        = 0,
        PAR_ID          = (1),
        PAR_NAME        = (1 << 1),
        PAR_LOCATION    = (1 << 2),
        PAR_TYPE        = (1 << 3),
        PAR_REALTIME    = (1 << 4),
        PAR_T1          = (1 << 5),
        PAR_T2          = (1 << 6),
        PAR_DURATION    = (1 << 7),
        PAR_TIMERANGE   = (1 << 8),
        PAR_REGION      = (1 << 9),
        PAR_PROCESS     = (1 << 10),
        PAR_SEQUENCE    = (1 << 11),
        PAR_METHOD      = (1 << 12),
        PAR_INPUTS      = (1 << 13),
        PAR_OUTPUTS     = (1 << 14),
        PAR_BITMAP      = (1 << 15),
        PAR_COMMAND     = (1 << 16),
        PAR_ALL         = 0xFFFF        
    } VTCLI_PARAM;

    
    typedef struct
    {
        VTCLI_COMMAND cmd;
        const char *name;
        unsigned int objects;
        const char *desc;
        const char *example;
    } VTCLI_COMMAND_DEF;

    typedef struct
    {
        VTCLI_OBJECT obj;
        const char *name;
        unsigned int params;
    } VTCLI_OBJECT_DEF;
    
    typedef struct
    {
        VTCLI_PARAM par;
        const char *name;
        const char *desc;
        const char *example;
    } VTCLI_PARAM_DEF;
    
    typedef struct
    {
        const char *desc;
        const char *command;
    } VTCLI_EXAMPLE_DEF;
    
    typedef std::pair<std::string,std::string>  VTCLI_KEYVALUE;     // key=value OR just value
    typedef std::list<VTCLI_KEYVALUE>           VTCLI_KEYVALUE_LIST;
    
protected:
    VTApi *m_vtapi;
    
    static const VTCLI_COMMAND_DEF m_cmd[];
    static const VTCLI_OBJECT_DEF m_obj[];
    static const VTCLI_PARAM_DEF m_par[];
    static const VTCLI_EXAMPLE_DEF m_examples[];
    
protected:
    /**
     * Reads if command was supplied from command line arguments and forms help string
     * @param argc command line argument count
     * @param argv command line arguments
     * @param cmdline output command line for non-interactive mode
     * @return success
     */
    bool processArgs(int argc, char** argv, std::string& cmdline);
    /**
     * Handles command
     * @param line command line
     * @return success
     */
    bool handleCommandLine(const std::string& line, bool *pbStop = NULL);
    /**
     * Tokenizes command line into key/value pairs like this
     * command key=val file.cmd     =>      [""=>"command", "key"=>"val", ""=>"file.cmd"]
     * @param line input command line
     * @param tokens output token list
     * @param delimiter default token delimiter
     * @param skipMultipleDelimiters skips multiple delimiters in sequence
     * @return count of tokens
     */
    size_t tokenizeLine(const std::string& line, VTCLI_KEYVALUE_LIST& tokens, const char delimiter, bool skipMultipleDelimiters);
    /**
     * Creates key/value pair from string in key=value format.
     * If string is a simple value, key is empty string.
     * @param word input string
     * @param keyValue output key/value pair
     * @return success
     */
    bool createKeyValue(const std::string& word, VTCLI_KEYVALUE& keyValue);
    /**
     * Removes param from key/value list
     * @param keyValues input key/value list
     * @param key input key
     * @return value
     */
    std::string getParamValue(VTCLI_KEYVALUE_LIST& keyValues, const std::string& key);

    bool parseUintValue(const std::string& word, unsigned int *value);
    bool parseUintRangeValue(const std::string& word, unsigned int *value1, unsigned int *value2);
    bool parseFloatValue(const std::string& word, double *value);
    bool parseFloatRangeValue(const std::string& word, double *value1, double *value2);
    bool parseRegionValue(const std::string& word, IntervalEvent::box *value);

    VTCLI_COMMAND getCommand(const std::string& word);
    VTCLI_OBJECT getObject(const std::string& word);
    
    bool selectCommand(VTCLI_KEYVALUE_LIST& params);
    bool insertCommand(VTCLI_KEYVALUE_LIST& params);
    bool deleteCommand(VTCLI_KEYVALUE_LIST& params);
    bool loadCommand(VTCLI_KEYVALUE_LIST& params);
    bool statsCommand(VTCLI_KEYVALUE_LIST& params);
    bool controlCommand(VTCLI_KEYVALUE_LIST& params);
    bool testCommand();
    bool helpCommand();
    
    
    /**
     * Recursively loads video/images from dataset directory
     * @param ds dataset object
     * @param basepath base data directory
     * @param dirpath directory path relative to basepath
     */
    bool loadDirectory(Dataset *ds, const std::string& basepath, const std::string& dirpath);
    /**
     * Loads video into dataset
     * @param ds dataset object
     * @param filepath video path relative to dataset base path
     * @param realtime real time of video start
     * @return success
     */
    bool loadVideo(Dataset *ds, const std::string &filepath, const time_t& realtime = 0);
    /**
     * Loads image folder with all images into dataset
     * @param ds dataset object
     * @param dirpath director with images, relative to dataset base path
     * @return  success
     */
    bool loadImageFolder(Dataset *ds, const std::string &dirpath);
    /**
     * Loads image into specified imagefolder
     * @param ifld image folder object
     * @param filepath image file path relative to dataset base path
     * @return success
     */
    bool loadImage(ImageFolder *ifld, const std::string& filepath);

    /**
     * Fixes backwards slasehes and removes trailing slashes
     * @param path path to modify
     * @param bTrailing path should have trailing slash
     * @return fixed path
     */
    std::string fixPathSlashes(const std::string& path, bool bTrailing);
    /**
     * Gets file name without extension from path
     * @param path
     * @return name
     */
    std::string getFileNameNoExt(const std::string& path);
    /**
     * Gets objects string serialized from flags
     * @param objects OBJ_xxx values
     * @return string
     */
    std::string getObjectsString(unsigned int objects);
    /**
     * Gets params string serialized from flags
     * @param params PAR_xxx values
     * @return string
     */
    std::string getParamsString(unsigned int params);
    /**
     * Prints error
     * @param errmsg error message
     */
    void printError(const char* errmsg);
    /**
     * Prints error
     * @param errmsg error message
     */
    void printError(const std::string& errmsg);
};

/*
 * Command syntax: (array values are comma-separated)
 * insert sequence name=.. [seqnum=.. location=.. seqtype=..]
 * insert interval sequence=... t1=.. t2=.. [location=.. tags=.. svm=..]
 * insert process name=.. method=.. inputs=.. outputs=..
 */
#endif	/* VTCLI_H */

