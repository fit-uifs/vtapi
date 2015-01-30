/*
 * File:    vtcli.cpp
 * Author:  Vojtěch Fröml
 *          xfroml00 (at) stud.fit.vutbr.cz
 *
 * VTCli is a command line tool for easy manipulation with VTApi structures.
 * For more detailed description see header vtcli.h
 */

#include <cstdlib>
#include <time.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "vtcli.h"

using namespace vtapi;
using namespace std;

VTCli::VTCli(const VTApi& api) {
    this->vtapi = new VTApi(api);
    initSuffixes();
}

VTCli::VTCli(int argc, char** argv){
    this->processArgs(argc, argv);
    this->vtapi = new VTApi(argc, argv);
    initSuffixes();
}

VTCli::~VTCli() {
    if (this->vtapi) delete this->vtapi;
}

int VTCli::run() {
    string line, command;
    bool do_help = false;

    // without arguments, VTCli will be interactive (cycle)
    this->interact = this->cmdline.empty();

    if (this->interact) {
        cout << "VTCli is running" << endl;
        //cout << "Commands: query, select, insert, update, delete, "
        //        "show, script, test, help, exit";
        cout << "Commands: query, select, insert, test, help, exit" << endl;
    }
    
    // command cycle
    do {
        // get command, if cli is empty then start interactive mode
        if (this->interact) {
            cout << endl << "> "; cout.flush();
            getline(cin, line);
        }
        else {
            line = this->cmdline;
        }
        if (cin.fail()) break;        // EOF detected

        command = cutWord(line);
        do_help = (line.substr(0,4).compare("help") == 0) ;

        // command-specific help
        if (do_help) {
            printHelp (command);
        }
        // commands
        else if (command.compare("select") == 0) {
            selectCommand (line);
        }
        else if (command.compare("insert") == 0) {
            insertCommand (line);
        }
        else if (command.compare("update") == 0) {
            updateCommand (line);
        }
        else if (command.compare("delete") == 0) {
            deleteCommand (line);
        }
        else if (command.compare("query") == 0) {
            queryCommand (line);
        }
        else if (command.compare("show") == 0) {
            showCommand (line);
        }
        else if (command.compare("load") == 0) {
            loadCommand (line);
        }
        else if (command.compare("test") == 0) {
            this->vtapi->test();
        }
        else if (command.compare("help") == 0) {
            this->printHelp();
        }
        else if (command.compare("exit") == 0) {
            break;
        }
        else {
            cerr << "Unknown command" << endl;
        }
    } while (this->interact);

    return 0;
}

void VTCli::queryCommand(string& line) {
    if (!line.empty()) {
        Query *query = new Query(*(this->vtapi->commons), line);
        query->execute();
        delete query;
    }
}

void VTCli::selectCommand(string& line) {
    string input;   // what to select (dataset/sequence/...)
    map<string,string> params; // select params

    // parse command line
    input = cutWord(line);
    while (!line.empty()) {
        string word = cutWord(line);
        params.insert(createKeyValue(word));
    }

    // select dataset
    if (!input.compare("dataset")) {
        Dataset* ds = new Dataset(*(this->vtapi->commons), params["name"]);
        ds->select->whereString("dslocation", params["location"]);
        ds->next();
        ds->printAll();
        delete ds;
    }
    // select sequence
    else if (!input.compare("sequence")) {
        Sequence* seq = new Sequence(*(this->vtapi->commons), params["name"]);
        if (params.count("num") > 0) seq->select->whereInt("seqnum", atoi(params["num"].c_str()));
        seq->select->whereString("seqlocation", params["location"]);
        seq->select->whereSeqtype("seqtyp", params["type"]);
        seq->next();
        seq->printAll();
        delete seq;
    }
    // select interval
    else if (!input.compare("interval")) {
        Interval* in = new Interval(*(this->vtapi->commons));
        in->select->whereString("seqname", params["seqname"]);
        if (params.count("t1") > 0) in->select->whereInt("t1", atoi(params["t1"].c_str()));
        if (params.count("t2") > 0) in->select->whereInt("t2", atoi(params["t2"].c_str()));
        in->select->whereString("imglocation", params["location"]);
        in->next();
        in->printAll();
        delete in;
    }
    // select process
    else if (!input.compare("process")) {
        Process* pr = new Process(*(this->vtapi->commons));
        pr->select->whereString("prsname", params["name"]);
        pr->select->whereString("mtname", params["method"]);
        pr->select->whereString("inputs", params["inputs"]);
        pr->select->whereString("outputs", params["outputs"]);
        pr->next();
        pr->printAll();
        delete pr;
    }
    // select method
    else if (!input.compare("method")) {
        Method* me = new Method(*(this->vtapi->commons));
        //TODO: methodkeys not implemented yet
        me->select->whereString("mtname", params["name"]);
        me->next();
        me->printAll();
        delete me;
    }
}

void VTCli::insertCommand(string& line) {

    Dataset* ds;     // active dataset
    string input;    // what to insert (sequence/interval/...)
    string filepath; // insert automatically from file/directory
    map<string,string> params; // insert params

    //initialize dataset object
    ds = this->vtapi->newDataset();
    ds->next();
    if (ds->getDataset().empty()) {
        cerr << "Insert failed; no dataset specified to insert into" << endl;
        delete ds;
        return;
    }
    // parse command line
    input = cutWord(line);
    while (!line.empty()) {
        string word = cutWord(line);
        if (isParam(word)) {
            pair<string,string> param = createKeyValue(word);
            if (!param.first.empty() && !param.second.empty()) {
                params.insert(param);
            }
        }
        else {
            pair<string,string> param = pair<string,string>("fullpath",word);
            params.insert(param);
        }
    }

    // insert sequence
    if (input.compare("sequence") == 0) {
        insertSequence(ds, &params);
    }
    // insert interval
    else if (input.compare("interval") == 0) {
        Sequence* seq = ds->newSequence(params["seqname"]);
        Interval* in = seq->newInterval(0, 0);
        in->add(params["seqname"], atoi(params["t1"].c_str()),
                atoi(params["t2"].c_str()), params["location"]);
        in->insert->execute();
        delete in;
        delete seq;
    }

    // insert process
    //TODO
    else if (input.compare("process") == 0) {
        cerr << "insert process not implemented" << endl;
    }
    else {
        cerr << "Only insert: sequence/interval/process" << endl;
    }

    delete ds;
}

//TODO
void VTCli::updateCommand(string& line) {
    cerr << "update command not implemented" << endl;
}
//TODO
void VTCli::deleteCommand(string& line) {
    cerr << "delete command not implemented" << endl;
}
//TODO
void VTCli::showCommand(string& line) {
    cerr << "show command not implemented" << endl;
}


void VTCli::loadCommand(string& line) {
    Dataset* ds;     // active dataset
    string filepath;


    //initialize dataset object
    ds = this->vtapi->newDataset();
    ds->next();
    if (ds->getDataset().empty()) {
        cerr << "Load failed; no dataset specified to insert into" << endl;
        delete ds;
        return;
    }

    filepath = ds->getBaseLocation() + ds->getDatasetLocation();
    fixSlashes(filepath);
    loadDirectory(ds, filepath);

    delete ds;
}

void VTCli::loadDirectory(Dataset *ds, const string& dirpath) {

    DIR *pDir = NULL;
    struct dirent *pEntry = NULL;

    if ((pDir = opendir(dirpath.c_str())) == NULL) {
        cerr << "Load failed; cannot open directory " << dirpath;
        delete ds;
        return;
    }
    while((pEntry = readdir(pDir)) != NULL) {
        string name (pEntry->d_name);
        string fullpath = dirpath + "/" + name;
        if(name.compare(".") == 0 || name.compare("..") == 0) continue;

        if (pEntry->d_type == DT_DIR) {
            if (isImageFolder(fullpath)) {
                insertImageFolder(ds, fullpath);
            }
            else {
                loadDirectory(ds, fullpath);
            }
        }
        else if (pEntry->d_type == DT_REG) {
            if (isVideoFile(fullpath)) {
                insertVideoFile(ds, fullpath);
            }
        }
    }

    closedir(pDir);
}

void VTCli::insertImageFolder(Dataset *ds, const string& dirpath) {
    map<string,string> params; // insert params
    pair<string,string> param;

    param = pair<string,string>("fullpath",dirpath);
    params.insert(param);
    param = pair<string,string>("type","images");
    params.insert(param);

    insertSequence(ds, &params);
}

void VTCli::insertVideoFile(Dataset *ds, const string& filepath) {
    map<string,string> params; // insert params
    pair<string,string> param;

    param = pair<string,string>("fullpath", filepath);
    params.insert(param);
    param = pair<string,string>("type","video");
    params.insert(param);

    insertSequence(ds, &params);
}

void VTCli::insertSequence(Dataset* ds, map<string,string>* params) {

    Sequence *seq = ds->newSequence();
    string filepath;

    // gather params: name, location and type
    do {

        filepath = params->count("fullpath") == 0 ? (*params)["fullpath"] : "";

        // param location and full file(dir)path
        if (filepath.empty()) {
            if (params->count("location") == 0) {
                cerr << "Insert sequence failed; no file specified." << endl;
                break;
            }
            else {
                fixSlashes((*params)["location"]);
                filepath = ds->getBaseLocation() + ds->getDatasetLocation() + (*params)["location"];
            }
        }
        else {
            if (params->count("location") == 0) {
                string location;
                fixSlashes(filepath);
                location = createLocationFromPath(filepath, ds->getBaseLocation(), ds->getDatasetLocation());
                params->insert(pair<string,string>("location", location));
            }
            else {
                cerr << "Insert sequence failed; can't specify both location and filepath" << endl;
                break;
            }
        }
        // param name
        if (params->count("name") == 0) {
            string name = createSeqnameFromPath(filepath);
            if (name.empty()) {
                cerr << "Insert sequence failed; invalid sequence name." << endl;
                break;
            }
            else {
                params->insert(pair<string,string>("name", name));
            }
        }
        // param type (and check validity)
        if (params->count("type") == 0) {
            if (isVideoFile(filepath)) {
                params->insert(pair<string,string>("type", "video"));
            }
            else if (isImageFolder(filepath)) {
                params->insert(pair<string,string>("type", "images"));
                (*params)["location"].append("/");
            }
            else {
                cerr << "Insert sequence " + (*params)["name"] + " failed; sequence must be an image folder or a video file." << endl;
                break;
            }
        }
        // insert video sequence
        if ((*params)["type"].compare("video") == 0) {
            seq->add((*params)["name"], (*params)["location"], (*params)["type"], seq->getUser(), "", "");
            if (!seq->addExecute()) {
                cerr << "Insert sequence " + (*params)["name"] + " failed; video not inserted." << endl;
                break;
            }
        }
        // insert image folder sequence, load list of images
        else if ((*params)["type"].compare("images") == 0) {
            set<string> imagelist;
            int t = 1;
            if (loadImageList(filepath, imagelist) == 0) {
                cerr << "Insert sequence " + (*params)["name"] + " failed; image files could not have been loaded." << endl;
                break;
            }
            else {
                Interval *img = seq->newInterval();
                seq->add((*params)["name"], (*params)["location"], (*params)["type"], seq->getUser(), "", "");
                if (!seq->addExecute()) {
                    cerr << "Insert sequence " + (*params)["name"] + " failed; image folder not inserted." << endl;
                    break;
                }
                for (set<string>::iterator it = imagelist.begin(); it != imagelist.end(); ++it) {
                    img->add((*params)["name"], t, t, *it, img->getUser(), "");
                    if (!img->addExecute()) {
                        cerr << "Insert interval " + (*it) + " failed; image not inserted" << endl;
                    }
                    t++;
                }
                img->select->executed = true;
                delete img;
            }
        }
    } while (0);

    seq->select->executed = true;
    delete seq;
}

bool VTCli::isVideoFile(const string& filepath) {
    ifstream video(filepath.c_str());
    size_t dotPos = 0;
    
    if (video) {       
        video.close();
        dotPos = filepath.find_last_of('.', string::npos);
        if (dotPos > 0 && dotPos < filepath.length()-1) {
            string suffix = filepath.substr(dotPos + 1, string::npos);
            if (this->videoSuffixes.count(suffix.c_str()) > 0) {
                return true;
            }
        }
    }
    return false;
}

bool VTCli::isImageFile(const string& filepath) {
    ifstream image(filepath.c_str());
    size_t dotPos = 0;

    if (image) {
        image.close();
        dotPos = filepath.find_last_of('.', string::npos);
        if (dotPos > 0 && dotPos < filepath.length()-1) {
            string suffix = filepath.substr(dotPos + 1, string::npos);
            if (this->imageSuffixes.count(suffix.c_str()) > 0) {
                return true;
            }
        }
    }
    return false;
}

bool VTCli::isImageFolder(const string& dirpath) {
    DIR *imagefolder = opendir(dirpath.c_str());

    if (imagefolder) {
        struct dirent *entry;
        while (entry = readdir(imagefolder)) {
            string filename (entry->d_name);
            if(filename.compare(".") != 0 && filename.compare("..") != 0) {
                string fullpath = dirpath + "/" + filename;
                if (isImageFile(fullpath)) {
                    closedir(imagefolder);
                    return true;
                }
            }
        }
        closedir (imagefolder);
    }
    return false;
}

int VTCli::loadImageList(const string& dirpath, set<string>& imagelist) {
    DIR *imagefolder = opendir(dirpath.c_str());
    int cntimg = 0;

    if (imagefolder) {
        struct dirent *entry;
        while (entry = readdir(imagefolder)) {
            string filename (entry->d_name);
            if(filename[0] != '.') {
                string fullpath = dirpath + "/" + filename;
                if (isImageFile(fullpath)) {
                    imagelist.insert(filename);
                    cntimg++;
                }
            }
        }
        closedir (imagefolder);
    }
    return cntimg;
}

 void VTCli::initSuffixes() {
     string imgsuf[] = {"bmp", "jpeg", "jpg", "gif", "png", "svg", "tiff"};
     string vidsuf[] = {"avi", "mkv", "mpeg", "mpg", "mp4", "rm", "wmv"};

     this->imageSuffixes = set<string> (imgsuf, imgsuf + sizeof(imgsuf) / sizeof(imgsuf[0]));
     this->videoSuffixes = set<string> (vidsuf, vidsuf + sizeof(vidsuf) / sizeof(vidsuf[0]));
 }

pair<string,string> VTCli::createKeyValue(const string& word) {
    size_t pos = word.find('=');

    if (pos > 0 && pos < word.length()-1)
        return pair<string,string>
                (word.substr(0,pos), word.substr(pos+1, string::npos));
    else
        return pair<string,string>("","");
}

string VTCli::createSeqnameFromPath(const string& filepath) {
    string seqname;
    size_t startPos = 0, endPos = string::npos;
    size_t len = filepath.length();
    size_t nsPos = len - 1;

    if (filepath.empty()) return "";
    while (filepath[nsPos] == '/') {
        nsPos--;
        if (nsPos < 0) return "";
    }
    startPos = filepath.find_last_of('/', nsPos);
    if (startPos == string::npos) {
        startPos = 0;
    }
    else {
        startPos++;
    }
    endPos = filepath.find_last_of('.', nsPos);
    if (endPos == string::npos || endPos <= startPos) endPos = nsPos + 1;

    seqname = filepath.substr(startPos, endPos - startPos);
    return seqname;
}

string VTCli::createLocationFromPath(const string& filepath, const string& baseLocation, const string& datasetLocation) {
    string part, location;
    string tmpBase = baseLocation;
    string tmpDataset = datasetLocation;
     
    fixSlashes(tmpBase);
    fixSlashes(tmpDataset);
    part = tmpBase + "/" + tmpDataset + "/";
    if (filepath.find(part) == 0) {
        location = filepath.substr(part.length(), string::npos);
    }
    else {
        location = filepath;
    }
    return location;
}


bool VTCli::isParam(const string& word) {
    size_t pos = word.find('=');
    return (pos > 0 && pos < word.length()-1);
}

string VTCli::cutWord(string& line) {
    string word;
    size_t startPos = 0, pos, endPos = string::npos;

    if (line.empty()) return "";

    // word end is whitespace, skip quoted part
        pos = line.find_first_of(" \t\n\"", startPos);
        if (pos != string::npos && line.at(pos) == '\"'){
            if (pos + 1 >= line.length()) {
                line.clear();
                return "";
            }
            endPos = line.find('\"', pos + 1);
            if (endPos == string::npos)
                word = line.substr(pos + 1, string::npos);
            else {
                word = line.substr(pos + 1, endPos - pos - 1);
                endPos++;
            }
        }
        else {
            endPos = pos;
            word = line.substr(0, endPos);
        }
    // cut line
    if (endPos != string::npos && endPos < line.length()){
        endPos = line.find_first_not_of(" \t\n", endPos);
        if (endPos != string::npos) line = line.substr(endPos, string::npos);
        else line.clear();
    }
    else
        line.clear();

    return word;
}

string VTCli::cutCSV(string& word) {
    string csv;
    size_t pos = word.find(",", 0);

    csv = word.substr(0, pos);
    if (pos != string::npos && pos+1 < word.length())
        word = word.substr(pos+1, string::npos);
    else word.clear();
    return csv;
}

bool VTCli::fixSlashes(string& path) {
    size_t len = path.length();
    size_t slPos = 0;
    size_t nsPos = len;

    do {
        slPos = path.find('\\', slPos);
        if (slPos != string::npos) {
            path[slPos] = '/';
        }
        else {
            break;
        }
    } while (1);
    do {
        nsPos--;
        if (nsPos < 0) {
            path.clear();
            return false;
        }
    } while (path[nsPos] == '/' || path[nsPos] == '\\');
    if (nsPos < len - 1) {
        path = path.substr(0, nsPos + 1);   
    }
    
    return true;
}

void VTCli::printHelp() {
    this->printHelp("all");
}

void VTCli::printHelp(const string& what) {
    stringstream hss;

    if (this->helpStrings.count(what)) {
        cout << this->helpStrings[what];
        return;
    }
    else if (!what.compare("query")) {
        hss << endl <<
            "query [SQLSTRING]" << endl << endl <<
            "     * executes custom SQLQUERY" << endl <<
            "     * ex.: list methods with active processes" << endl <<
            "              query SELECT DISTINCT mtname FROM public.processes" << endl ;
    }
    else if (!what.compare("select")) {
        hss << endl <<
            "select dataset|sequence|interval|process|method|selection [ARGS]" << endl << endl <<
            "Selects data and prints them in specified format (-f option)" << endl << endl <<
            "ARG format:      arg=value or arg=value1,value2,..." << endl << endl <<
            " Dataset ARGS:" << endl <<
            "      name       name of the dataset" << endl <<
            "  location       base location of the dataset data files (directory)" << endl << endl <<
            " Sequence ARGS:" << endl <<
            "      name       name of the sequence" << endl <<
            "  location       location of the sequence data file(s) (file/directory)" << endl <<
            "       num       unique number of the sequence" << endl <<
            "      type       type of the sequence [images, video]" << endl << endl <<
            "Interval ARGS:" << endl <<
            "  seqname        name of the sequence containing this interval" << endl <<
            "        t1       begin time of the interval" << endl <<
            "        t2       end time of the interval" << endl <<
            "  location       location of the interval data file (file)" << endl << endl <<
            "Method ARGS:" << endl <<
            "      name       name of the method" << endl << endl <<
            "Process ARGS:" << endl <<
            "      name       name of the process" << endl <<
            "    method       name of the method the process is instance of" << endl <<
            "    inputs       data type of inputs (database table)" << endl <<
            "   outputs       data type of outputs (database table)" << endl << endl <<
            "Selection ARGS:" << endl <<
            "   (not implemented)" << endl;
    }
    else if (!what.compare("insert")) {
        hss << endl <<
            "insert sequence|interval|process [FULLPATH] [ARGS]" << endl << endl <<
            "Inserts data into database" << endl << endl <<
            "   [FULLPATH]:      full path to file/directory to insert  " << endl <<
            "[ARGS] format:      arg=value or arg=value1,value2,..." << endl << endl <<
            " Sequence ARGS:" << endl <<
            "      name       name of the sequence" << endl <<
            "  location       location of the sequence data file(s) within dataset (don't combine with FULLPATH arg)" << endl <<
            "      type       type of the sequence [images, video]" << endl << endl <<
            "Interval ARGS:" << endl <<
            "   seqname       name of the sequence containing this interval *REQUIRED*" << endl <<
            "        t1       begin time of the interval *REQUIRED*" << endl <<
            "        t2       end time of the interval *REQUIRED*" << endl <<
            "  location       location of the interval data file (file)" << endl << endl <<
            "Process ARGS:" << endl <<
            "      name       name of the process *REQUIRED*" << endl <<
            "    method       name of the method the process is instance of" << endl <<
            "    inputs       data type of inputs (database table)" << endl <<
            "   outputs       data type of outputs (database table)" << endl;
    }
    else if (!what.compare("update")) {
        hss << "update command not implemented";
    }
    else if (!what.compare("delete")) {
        hss << "delete command not implemented";
    }
    else if (!what.compare("show")) {
        hss << "show command not implemented";
    }
    else hss.clear(std::_S_failbit);
    if (!hss.fail()) {
        this->helpStrings.insert(std::make_pair(what,hss.str()));
        cout << hss.str() << endl;
    }
}

bool VTCli::processArgs(int argc, char** argv) {
    gengetopt_args_info args_info;
    if (cmdline_parser2 (argc, argv, &args_info, 0, 1, 0) != 0) {
        helpStrings.insert(std::make_pair("all", ""));
        this->cmdline.clear();
        return false;
    }
    else {
        // Construct help string
        stringstream hss;
        int i = 0;
        hss << endl << CMDLINE_PARSER_PACKAGE_NAME <<" "<< CMDLINE_PARSER_VERSION << endl;
        hss << endl << gengetopt_args_info_usage << endl << endl;
        while (gengetopt_args_info_help[i])
            hss << gengetopt_args_info_help[i++] << endl;
        helpStrings.insert(std::make_pair("all", hss.str()));
        // Construct command string from unnamed arguments
        for (int i = 0; i < args_info.inputs_num; i++)
            cmdline.append(args_info.inputs[i]).append(" ");
        cmdline_parser_free (&args_info);
        return true;
    }
}







/**
 * The VTCli main function
 *
 * @param argc
 * @param argv
 * @return sucess
 */
int main(int argc, char** argv) {
    VTCli app(argc, argv);

    return app.run();
}










