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
protected:
    VTApi* vtapi;
    bool interact;
    String cmdline;
    std::map<String,String> helpStrings;

    int processArgs(int argc, char** argv);
    String getWord(String& line);
    String getCSV(String& word);
    std::pair<String,String> createParam(String word);

};

#endif	/* VTCLI_H */

