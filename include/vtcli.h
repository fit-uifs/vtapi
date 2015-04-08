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

#include "vtapi.h"

using namespace vtapi;

/**
 * @brief Main VTCli class
 */
class VTCli {
public:

    /**
     * VTCli constructor with command line arguments
     * @param argc command line argument count
     * @param argv command line arguments
     */
    VTCli(int argc, char** argv);
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
     * @return error code
     */
    int run();
    /**
     * Prints help
     */
    void printHelp();

protected:

    VTApi* m_vtapi;
    bool m_bInteract;
    std::string m_cmdline;
    
    /**
     * Creates key/value std::pair from string in key=value format
     * @param word Input string
     * @return Key,Value std::pair
     */
    std::pair<std::string,std::string> createKeyValue(const std::string& word);
    /**
     * Checks if string is valid parameter key=value
     * @param word checked string
     * @return bool value
     */
    bool isParam(const std::string& word);
    /**
     * Loads list of images within a folder
     * @param dirpath image folder
     * @param imagelist set of loaded image filenames
     * @return count of images loaded
     */
    int loadImageList(const std::string& dirpath, std::set<std::string>& imagelist);
    /**
     * Loads directory recursively within dataset
     * @param dirpath path to dir
     */
    void loadDirectory(Dataset *ds, const std::string& basepath, const std::string& dirpath);
    /**
     * Inserts one sequence into db
     * @param ds dataset
     * @param params sequence name, location and type
     */
    void insertSequence(Dataset *ds, std::map<std::string,std::string> *params);
    /**
     * Changes all slashes within a path to '/' and removes slashes at its end
     * @param path path to modify
     * @return success
     */
    std::string fixPathSlashes(const std::string& path);
    /**
     * Gets file name without extension from path
     * @param path
     * @return name
     */
    std::string getFileNameNoExt(const std::string& path);
    /**
     * Create relative location
     * @param filepath
     * @param basepath
     * @return 
     */
    std::string createLocationFromPath(const std::string& filepath, const std::string& basepath);
    /**
     * Cuts and returns the first word from the input command line
     * @param line input command line
     * @return First word
     */
    std::string cutWord(std::string& line);
    /**
     * Cuts and returns the first CSV value from the input command line
     * @param line input command line
     * @return First CSV value
     */
    std::string cutCSV(std::string& word);
    /**
     * Reads if command was supplied from command line arguments and forms help string
     * @param argc command line argument count
     * @param argv command line arguments
     * @return success
     */
    bool processArgs(int argc, char** argv);

    /**
     * 
     * @param line
     */
    void selectCommand(std::string& line);
    void insertCommand(std::string& line);
    void loadCommand(std::string& line);
};

/*
 * Command syntax: (array values are comma-separated)
 * insert sequence name=.. [seqnum=.. location=.. seqtype=..]
 * insert interval sequence=... t1=.. t2=.. [location=.. tags=.. svm=..]
 * insert process name=.. method=.. inputs=.. outputs=..
 */
#endif	/* VTCLI_H */

