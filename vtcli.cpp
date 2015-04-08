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

////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    VTCli app(argc, argv);

    return app.run();
}

////////////////////////////////////////////////////////
// VTCli class implementation
////////////////////////////////////////////////////////

VTCli::VTCli(int argc, char** argv)
{
    this->processArgs(argc, argv);
    m_vtapi = new VTApi(argc, argv);
}

VTCli::~VTCli()
{
    if (m_vtapi) delete m_vtapi;
}

bool VTCli::processArgs(int argc, char** argv)
{
    gengetopt_args_info args_info = { 0 };

    bool bRet = (cmdline_parser2 (argc, argv, &args_info, 0, 1, 0) == 0);
    if (bRet) {
        // Construct command string from unnamed arguments
        for (int i = 0; i < args_info.inputs_num; i++) {
            m_cmdline.append(args_info.inputs[i]).append(" ");
        }
        m_bInteract = (args_info.inputs_num == 0);
        cmdline_parser_free (&args_info);
    }
    else {
        m_bInteract = false;
        m_cmdline.clear();
    }

    return bRet;
}

void VTCli::printHelp()
{
    cout << CMDLINE_PARSER_PACKAGE_NAME << " " << CMDLINE_PARSER_VERSION << endl << endl
    << gengetopt_args_info_usage << endl << endl;
    for (int i = 0; gengetopt_args_info_help[i]; i++) {
        cout << gengetopt_args_info_help[i] << endl;
    }
}

int VTCli::run() {
    string line, command;

    if (this->m_bInteract) {
        cout << "VTCli is running" << endl;
        cout << "Commands: select, insert, load, test, help, exit" << endl;
    }
    
    // command cycle
    do {
        // get command, if cli is empty then start interactive mode
        if (m_bInteract) {
            cout << endl << "> ";
            cout.flush();
            getline(cin, line);
        }
        else {
            line = this->m_cmdline;
        }
        
        if (cin.fail()) break;        // EOF detected

        command = cutWord(line);

        // commands
        if (command.compare("select") == 0) {
            selectCommand (line);
        }
        else if (command.compare("insert") == 0) {
            insertCommand (line);
        }
        else if (command.compare("load") == 0) {
            loadCommand (line);
        }
        else if (command.compare("test") == 0) {
            m_vtapi->test();
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
    } while (m_bInteract);

    return 0;
}

void VTCli::selectCommand(string& line)
{
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
        Dataset* ds = new Dataset(*(m_vtapi->commons), params["name"]);
        ds->select->whereString("dslocation", params["location"]);
        ds->next();
        ds->printAll();
        delete ds;
    }
    // select sequence
    else if (!input.compare("sequence")) {
        Sequence* seq = new Sequence(*(m_vtapi->commons), params["name"]);
        if (params.count("num") > 0) seq->select->whereInt("seqnum", atoi(params["num"].c_str()));
        seq->select->whereString("seqlocation", params["location"]);
        seq->select->whereSeqtype("seqtyp", params["type"]);
        seq->next();
        seq->printAll();
        delete seq;
    }
    // select interval
    else if (!input.compare("interval")) {
        Interval* in = NULL;
        if (params.count("process") > 0) {
            Process *p = new Process(*(m_vtapi->commons), params["process"]);
            if (p->next()) {
                in = new Interval(*(m_vtapi->commons), p->getOutputs());
            }
            else {
                cerr << "Select failed : process " << params["process"] << " doesn't exist" << endl;
            }
            delete p;
        }
        else {
            in = new Interval(*(m_vtapi->commons));
        }
        if (in) {
            in->select->whereString("seqname", params["seqname"]);
            in->select->whereString("prsname", params["process"]);
            if (params.count("t1") > 0) in->select->whereInt("t1", atoi(params["t1"].c_str()));
            if (params.count("t2") > 0) in->select->whereInt("t2", atoi(params["t2"].c_str()));
            in->select->whereString("imglocation", params["location"]);
            in->next();
            in->printAll();
            delete in;
        }
    }
    // select process
    else if (!input.compare("process")) {
        Process* pr = new Process(*(m_vtapi->commons));
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
        Method* me = new Method(*(m_vtapi->commons), params["name"]);
        me->next();
        me->printAll();
        delete me;
    }
    // select method keys
    else if (!input.compare("methodkeys")) {
        Method* me = new Method(*(m_vtapi->commons), params["method"]);
        me->next();
        me->printMethodKeys();
        delete me;
    }
}

void VTCli::insertCommand(string& line) {

    Dataset* ds;     // active dataset
    string input;    // what to insert (sequence/interval/...)
    string filepath; // insert automatically from file/directory
    map<string,string> params; // insert params

    //initialize dataset object
    ds = m_vtapi->newDataset();
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

void VTCli::loadCommand(string& line) {
    Dataset* ds = m_vtapi->newDataset();
    string filepath;

    //initialize dataset object
    ds->next();
    if (ds->getDataset().empty()) {
        cerr << "Load failed; no dataset specified to insert into" << endl;
        delete ds;
        return;
    }

    loadDirectory(ds, ds->getBaseLocation() + ds->getDatasetLocation(), "");

    delete ds;
}

void VTCli::loadDirectory(Dataset *ds, const string& basepath, const string& dirpath)
{   
    string fulldirpath = basepath + dirpath;
    
    DIR *pDir = opendir(fulldirpath.c_str());
    if (!pDir) {
        cerr << "Load failed; cannot open directory " << fulldirpath;
        return;
    }

    struct dirent *pEntry;
    while((pEntry = readdir(pDir)) != NULL) {
        if (pEntry->d_type == DT_DIR) {
            if (pEntry->d_name[0] == '.' && (pEntry->d_name[1] == '\0' || pEntry->d_name[1] == '.')) continue;
            loadDirectory(ds, basepath, dirpath + pEntry->d_name + '/');
        }
        else if (pEntry->d_type == DT_REG) {
            std::string fullpath = dirpath + pEntry->d_name;
            Video *vid = ds->newVideo();
            if (vid->add(getFileNameNoExt(pEntry->d_name), fullpath)) {
                cout << fullpath << endl;
                vid->addExecute();
            }
            else {
                cerr << "Cannot open video " + fullpath + "; video not inserted." << endl;
            }
            vid->select->executed = true;
            delete vid;
        }
    }

    closedir(pDir);
}

void VTCli::insertSequence(Dataset* ds, map<string,string>* params)
{
    string filepath;

    do {

        filepath = params->count("fullpath") == 0 ? (*params)["fullpath"] : "";

        // param location and full file(dir)path
        if (filepath.empty()) {
            if (params->count("location") == 0) {
                cerr << "Insert sequence failed; no file specified." << endl;
                break;
            }
            else {
                string location = fixPathSlashes((*params)["location"]);
                filepath = ds->getBaseLocation() + ds->getDatasetLocation() + location;
            }
        }
        else {
            if (params->count("location") == 0) {
                string location = createLocationFromPath(filepath, ds->getBaseLocation() + '/' + ds->getDatasetLocation());
                params->insert(pair<string,string>("location", location));
            }
            else {
                cerr << "Insert sequence failed; can't specify both location and filepath" << endl;
                break;
            }
        }
        // param name
        if (params->count("name") == 0) {
            params->insert(pair<string,string>("name", getFileNameNoExt(filepath)));
        }
        // insert video sequence
        if ((*params)["type"].compare("video") == 0) {
            Video *vid = ds->newVideo();
            if (vid->add((*params)["name"], (*params)["location"])) {
                if (!vid->addExecute()) {
                    cerr << "Insert sequence " + (*params)["name"] + " failed; video not inserted." << endl;
                    break;
                }
            }
            else {
                cerr << "Cannot open video " + (*params)["name"] + "; video not inserted." << endl;
                break;
            }

            vid->select->executed = true;
            delete vid;
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
                Sequence *seq = ds->newSequence();
                Interval *img = seq->newInterval();
                seq->add((*params)["name"], (*params)["location"], (*params)["type"]);
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
                seq->select->executed = true;
                delete img;
                delete seq;
            }
        }
    } while (0);

}

int VTCli::loadImageList(const string& dirpath, set<string>& imagelist) {
    DIR *imagefolder = opendir(dirpath.c_str());
    int cntimg = 0;

    if (imagefolder) {
        struct dirent *entry;
        while (entry = readdir(imagefolder)) {
            if (entry->d_type == DT_REG) {
                imagelist.insert(entry->d_name);
                cntimg++; 
            }
        }
        closedir (imagefolder);
    }
    return cntimg;
}

pair<string,string> VTCli::createKeyValue(const string& word) {
    size_t pos = word.find('=');

    if (pos > 0 && pos < word.length()-1)
        return pair<string,string>
                (word.substr(0,pos), word.substr(pos+1, string::npos));
    else
        return pair<string,string>("","");
}

string VTCli::createLocationFromPath(const string& filepath, const string& basepath)
{
    string fullpath = fixPathSlashes(filepath);
    string partpath = fixPathSlashes(basepath);

    if (fullpath.compare(0, partpath.length(), partpath) == 0) {
        return fullpath.substr(partpath.length(), string::npos);
    }
    else {
        return fullpath;
    }
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

string VTCli::fixPathSlashes(const string& path)
{
    string ret;
    
    size_t pos = path.find_last_not_of("/\\");
    if (pos != string::npos) {
        ret = path.substr(0, pos);
    }

    pos = 0;
    while ((pos = ret.find('\\', pos)) != string::npos) {
        ret[pos] = '/';
    }
    
    return ret;
}

string VTCli::getFileNameNoExt(const std::string& path)
{
    size_t start = path.find_last_of('/');
    size_t end = path.find_last_of('.');
    
    start = (start == string::npos) ? 0 : start + 1;
    if (end <= start) end = string::npos;
    
    size_t len = (end != string::npos) ? end - start - 1 : string::npos;
    return path.substr(start, len);
}

