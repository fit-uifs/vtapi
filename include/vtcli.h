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

#include "vtapi.h"

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
    int printHelp(const String& what);

protected:
    VTApi* vtapi;
    bool interact;
    String cmdline;
    std::map<String,String> helpStrings;
    std::set<String> videoSuffixes;
    std::set<String> imageSuffixes;
    
    /**
     * Creates key/value std::pair from string in key=value format
     * @param word Input string
     * @return Key,Value std::pair
     */
    std::pair<String,String> createKeyValue(const String& word);
    /**
     * Creates sequence name (filename without suffix) from its full path
     * @param filepath full file(dir)path
     * @return sequence name
     */
    String createSeqnameFromPath(const String& filepath);
    /**
     * Creates sequence location (within dataset) from its full path
     * @param filepath full file(dir)path
     * @param baseLocation base data location
     * @param datasetLocation dataset data location
     * @return sequence location
     */
    String createLocationFromPath(const String& filepath, const String& baseLocation, const String& datasetLocation);
    /**
     * Checks if string is valid parameter key=value
     * @param word checked string
     * @return bool value
     */
    bool isParam(const String& word);
    /**
     * Checks if filepath points to a video file
     * @param filepath full filepath
     * @return bool value
     */
    bool isVideoFile(const String& filepath);
    /**
     * Checks if filepath points to a image file
     * @param filepath full filepath
     * @return bool value
     */
    bool isImageFile(const String& filepath);
    /**
     * Checks if dirpath points to a directory with at least one image in it
     * @param dirpath full path to directory
     * @return bool value
     */
    bool isImageFolder(const String& dirpath);
    /**
     * Loads list of images within a folder
     * @param dirpath image folder
     * @param imagelist set of loaded image filenames
     * @return count of images loaded
     */
    int loadImageList(const String& dirpath, std::set<String>& imagelist);
    /**
     * Initialize sets holding image/video suffixes
     */
    void initSuffixSets();
    /**
     * Changes all slashes within a path to '/' and removes slashes at its end
     * @param path path to modify
     * @return VTCLI_OK on success, VTCLI_FAIL on failure
     */
    int fixSlashes(String& path);
    /**
     * Cuts and returns the first word from the input command line
     * @param line input command line
     * @return First word
     */
    String cutWord(String& line);
    /**
     * Cuts and returns the first CSV value from the input command line
     * @param line input command line
     * @return First CSV value
     */
    String cutCSV(String& word);
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
    void queryCommand(String& line);
    void selectCommand(String& line);
    void insertCommand(String& line);
    void updateCommand(String& line);
    void deleteCommand(String& line);
    void showCommand(String& line);
    void installCommand(String& line);
};

/*
 * Command syntax: (array values are comma-separated)
 * insert sequence name=.. [seqnum=.. location=.. seqtype=..]
 * insert interval sequence=... t1=.. t2=.. [location=.. tags=.. svm=..]
 * insert process name=.. method=.. inputs=.. outputs=..
 */
#endif	/* VTCLI_H */

