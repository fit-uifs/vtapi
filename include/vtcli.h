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
     * @return error code
     */
    int run();
    /**
     * Prints basic help
     */
    void printHelp();
    /**
     * Prints command-specific help
     * @param what command with which to help
     */
    void printHelp(const std::string& what);

protected:

    VTApi* vtapi;
    bool interact;
    std::string cmdline;
    std::map<std::string,std::string> helpStrings;
    std::set<std::string> videoSuffixes;
    std::set<std::string> imageSuffixes;
    
    /**
     * Creates key/value std::pair from string in key=value format
     * @param word Input string
     * @return Key,Value std::pair
     */
    std::pair<std::string,std::string> createKeyValue(const std::string& word);
    /**
     * Creates sequence name (filename without suffix) from its full path
     * @param filepath full file(dir)path
     * @return sequence name
     */
    std::string createSeqnameFromPath(const std::string& filepath);
    /**
     * Creates sequence location (within dataset) from its full path
     * @param filepath full file(dir)path
     * @param baseLocation base data location
     * @param datasetLocation dataset data location
     * @return sequence location
     */
    std::string createLocationFromPath(const std::string& filepath, const std::string& baseLocation, const std::string& datasetLocation);
    /**
     * Checks if string is valid parameter key=value
     * @param word checked string
     * @return bool value
     */
    bool isParam(const std::string& word);
    /**
     * Checks if filepath points to a video file
     * @param filepath full filepath
     * @return bool value
     */
    bool isVideoFile(const std::string& filepath);
    /**
     * Checks if filepath points to a image file
     * @param filepath full filepath
     * @return bool value
     */
    bool isImageFile(const std::string& filepath);
    /**
     * Checks if dirpath points to a directory with at least one image in it
     * @param dirpath full path to directory
     * @return bool value
     */
    bool isImageFolder(const std::string& dirpath);
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
    void loadDirectory(Dataset *ds, const std::string& dirpath);
    /**
     * Inserts one sequence into db
     * @param ds dataset
     * @param params sequence name, location and type
     */
    void insertSequence(Dataset *ds, std::map<std::string,std::string> *params);
    /**
     * Inserts one sequence (image folder) into db
     * @param ds dataset
     * @param dirpath path to image folder
     */
    void insertImageFolder(Dataset *ds, const std::string& dirpath);
    /**
     * Inserts one sequence (video file) into db
     * @param ds dataset
     * @param dirpath path to image folder
     */
    void insertVideoFile(Dataset *ds, const std::string& filepath);
    /**
     * Initialize sets holding image/video suffixes
     */
    void initSuffixes();
    /**
     * Changes all slashes within a path to '/' and removes slashes at its end
     * @param path path to modify
     * @return success
     */
    bool fixSlashes(std::string& path);
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
    void queryCommand(std::string& line);
    void selectCommand(std::string& line);
    void insertCommand(std::string& line);
    void updateCommand(std::string& line);
    void deleteCommand(std::string& line);
    void showCommand(std::string& line);
    void loadCommand(std::string& line);
};

/*
 * Command syntax: (array values are comma-separated)
 * insert sequence name=.. [seqnum=.. location=.. seqtype=..]
 * insert interval sequence=... t1=.. t2=.. [location=.. tags=.. svm=..]
 * insert process name=.. method=.. inputs=.. outputs=..
 */
#endif	/* VTCLI_H */

