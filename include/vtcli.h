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

#define     VTCLI_OK    0
#define     VTCLI_FAIL  -1

#include <cstdlib>
#include <time.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <set>

#include "vtapi.h"

using namespace vtapi;

using std::ifstream;
using std::iostream;
using std::stringstream;
using std::string;
using std::map;
using std::set;
using std::pair;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

/**
 * @brief Main VTCli class
 */
class VTCli {
public:

    /**
     * VTCli constructor with arguments from a VTApi object
     * @param api initialized VTApi object
     */
    VTCli(const VTApi& api);
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
     * @return VTCLI_OK on success, VTCLI_FAIL on failure
     */
    int run();
    /**
     * Prints basic help
     * @return VTCLI_OK on success, VTCLI_FAIL on failure
     */
    int printHelp();
    /**
     * Prints command-specific help
     * @param what command with which to help
     * @return VTCLI_OK on success, VTCLI_FAIL on failure
     */
    int printHelp(const string& what);

protected:

    VTApi* vtapi;
    bool interact;
    string cmdline;
    map<string,string> helpStrings;
    set<string> videoSuffixes;
    set<string> imageSuffixes;
    
    /**
     * Creates key/value std::pair from string in key=value format
     * @param word Input string
     * @return Key,Value std::pair
     */
    std::pair<string,string> createKeyValue(const string& word);
    /**
     * Creates sequence name (filename without suffix) from its full path
     * @param filepath full file(dir)path
     * @return sequence name
     */
    string createSeqnameFromPath(const string& filepath);
    /**
     * Creates sequence location (within dataset) from its full path
     * @param filepath full file(dir)path
     * @param baseLocation base data location
     * @param datasetLocation dataset data location
     * @return sequence location
     */
    string createLocationFromPath(const string& filepath, const string& baseLocation, const string& datasetLocation);
    /**
     * Checks if string is valid parameter key=value
     * @param word checked string
     * @return bool value
     */
    bool isParam(const string& word);
    /**
     * Checks if filepath points to a video file
     * @param filepath full filepath
     * @return bool value
     */
    bool isVideoFile(const string& filepath);
    /**
     * Checks if filepath points to a image file
     * @param filepath full filepath
     * @return bool value
     */
    bool isImageFile(const string& filepath);
    /**
     * Checks if dirpath points to a directory with at least one image in it
     * @param dirpath full path to directory
     * @return bool value
     */
    bool isImageFolder(const string& dirpath);
    /**
     * Loads list of images within a folder
     * @param dirpath image folder
     * @param imagelist set of loaded image filenames
     * @return count of images loaded
     */
    int loadImageList(const string& dirpath, set<string>& imagelist);
    /**
     * Loads directory recursively within dataset
     * @param dirpath path to dir
     */
    void loadDirectory(Dataset *ds, const string& dirpath);
    /**
     * Inserts one sequence into db
     * @param ds dataset
     * @param params sequence name, location and type
     */
    void insertSequence(Dataset *ds, map<string,string> *params);
    /**
     * Inserts one sequence (image folder) into db
     * @param ds dataset
     * @param dirpath path to image folder
     */
    void insertImageFolder(Dataset *ds, const string& dirpath);
    /**
     * Inserts one sequence (video file) into db
     * @param ds dataset
     * @param dirpath path to image folder
     */
    void insertVideoFile(Dataset *ds, const string& filepath);
    /**
     * Initialize sets holding image/video suffixes
     */
    void initSuffixes();
    /**
     * Changes all slashes within a path to '/' and removes slashes at its end
     * @param path path to modify
     * @return VTCLI_OK on success, VTCLI_FAIL on failure
     */
    int fixSlashes(string& path);
    /**
     * Cuts and returns the first word from the input command line
     * @param line input command line
     * @return First word
     */
    string cutWord(string& line);
    /**
     * Cuts and returns the first CSV value from the input command line
     * @param line input command line
     * @return First CSV value
     */
    string cutCSV(string& word);
    /**
     * Reads if command was supplied from command line arguments and forms help string
     * @param argc command line argument count
     * @param argv command line arguments
     * @return VTCLI_OK on success, VTCLI_FAIL on failure
     */
    int processArgs(int argc, char** argv);

    /**
     * 
     * @param line
     */
    void queryCommand(string& line);
    void selectCommand(string& line);
    void insertCommand(string& line);
    void updateCommand(string& line);
    void deleteCommand(string& line);
    void showCommand(string& line);
    void loadCommand(string& line);
};

/*
 * Command syntax: (array values are comma-separated)
 * insert sequence name=.. [seqnum=.. location=.. seqtype=..]
 * insert interval sequence=... t1=.. t2=.. [location=.. tags=.. svm=..]
 * insert process name=.. method=.. inputs=.. outputs=..
 */
#endif	/* VTCLI_H */

