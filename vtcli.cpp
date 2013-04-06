/**
 * @example vtcli.cpp
 * 
 * Toto je zkouska.
 * @see Commons
 *
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <dirent.h>

#include "vtapi.h"
#include "vtcli.h"
#include "vtapi_settings.h"


VTCli::VTCli(int argc, char** argv){
    this->processArgs(argc, argv);
    this->vtapi = new VTApi(argc, argv);
    initSufVectors();
}

VTCli::VTCli(const VTApi& api) {
    this->vtapi = new VTApi(api);
    initSufVectors();
}

VTCli::~VTCli() {
    destruct(this->vtapi);
}

int VTCli::run() {

    String line, command;
    bool do_help = false;

    // without arguments, VTCli will be interactive (cycle)
    this->interact = this->cmdline.empty();

    if (this->interact) {
        std::cout << "VTApi is running" << std::endl;
        //std::cout << "Commands: query, select, insert, update, delete, "
        //        "show, script, test, help, exit";
        std::cout << "Commands: query, select, insert, test, help, exit" << std::endl;
    }
    
    // command cycle
    do {
        // get command, if cli is empty then start interactive mode
        if (this->interact) {
            std::cout << std::endl << "> "; std::cout.flush();
            getline(std::cin, line);
        }
        else {
            line = this->cmdline;
        }
        if (std::cin.fail()) break;        // EOF detected

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
        else if (command.compare("install") == 0) {
            installCommand (line);
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
            this->vtapi->commons->warning("Unknown command");
        }
    } while (this->interact);

    return 0;
}

void VTCli::queryCommand(String& line) {
    if (!line.empty()) {
        PGresult* qres = PQparamExec(vtapi->commons->getConnector()->getConn(), NULL, line.c_str(), PGF);
        if (qres) {
            KeyValues* kv = new KeyValues(*(this->vtapi->commons));
            kv->select = new Select(*(this->vtapi->commons));
            kv->select->res = qres;
            kv->select->executed = true;
            kv->printAll();
            destruct(kv);
        }
        else {
            String errmsg = line + " : " + PQgeterror();
            this->vtapi->commons->warning(errmsg);
        }
    }
}

void VTCli::selectCommand(String& line) {
    String input;   // what to select (dataset/sequence/...)
    std::map<String,String> params; // select params

    // parse command line
    input = cutWord(line);
    while (!line.empty()) {
        String word = cutWord(line);
        params.insert(createKeyValue(cutWord(line)));
    }

    // TODO: udelat pres Tkey
    // select dataset
    if (!input.compare("dataset")) {
        Dataset* ds = new Dataset(*(this->vtapi->commons));
        ds->select->where.clear();
        ds->select->whereString("dsname", params["name"]);
        ds->select->whereString("dslocation", params["location"]);
        ds->next();
        ds->printAll();
        destruct(ds);
    }
    // select sequence
    else if (!input.compare("sequence")) {
        Sequence* seq = new Sequence(*(this->vtapi->commons));
        seq->select->where.clear();
        seq->select->whereString("seqname", params["name"]);
        //seq->select->whereInt("seqnum", atoi(params["num"]));
        seq->select->whereString("seqlocation", params["location"]);
        seq->select->whereString("seqtyp", params["type"]);
        seq->next();
        seq->printAll();
        destruct(seq);
    }
    // select interval
    else if (!input.compare("interval")) {
        Interval* in = new Interval(*(this->vtapi->commons));
        in->select->where.clear();
        in->select->whereString("seqname", params["seqname"]);
        //in->select->whereInt(t1, params["t1"]);
        //in->select->whereInt(t1, params["t1"]);
        in->select->whereString("imglocation", params["location"]);
        in->next();
        in->printAll();
        destruct(in);
    }
    // select process
    else if (!input.compare("process")) {
        Process* pr = new Process(*(this->vtapi->commons));
        pr->select->where.clear();
        pr->select->whereString("prsname", params["name"]);
        pr->select->whereString("mtname", params["method"]);
        pr->select->whereString("inputs", params["inputs"]);
        pr->select->whereString("outputs", params["outputs"]);
        pr->next();
        pr->printAll();
        destruct(pr);
    }
    // select method
    else if (!input.compare("method")) {
        Method* me = new Method(*(this->vtapi->commons));
        //TODO: methodkeys not implemented yet
        me->select->where.clear();
        me->select->whereString("mtname", params["name"]);
        me->next();
        me->printAll();
        destruct(me);
    }
}

void VTCli::insertCommand(String& line) {

    Dataset* ds;     // active dataset
    String input;    // what to insert (sequence/interval/...)
    String filepath; // insert automatically from file/directory
    std::map<String,String> params; // insert params
    
    //initialize dataset object
    ds = this->vtapi->newDataset();
    ds->next();
    if (ds->dataset.empty()) {
        ds->warning("Insert failed; no dataset specified to insert into");
        destruct(ds);
        return;
    }
    // parse command line
    input = cutWord(line);
    while (!line.empty()) {
        String word = cutWord(line);
        if (isParam(word)) {
            std::pair<String,String> param = createKeyValue(word);
            if (!param.first.empty() && !param.second.empty()) {
                params.insert(param);
            }
        }
        else {
            filepath = word;
        }
    }
    // insert sequence
    if (input.compare("sequence") == 0) {
        Sequence *seq = ds->newSequence();
        // gather params: name, location and type
        do {
            // param location and full file(dir)path
            if (filepath.empty()) {
                if (params.count("location") == 0) {
                    ds->warning("Insert sequence failed; no file specified.");
                    break;
                }
                else {
                    fixSlashes(params["location"]);
                    filepath = ds->baseLocation + ds->datasetLocation + params["location"];
                }
            }
            else {
                if (params.count("location") == 0) {
                    String location;
                    fixSlashes(filepath);
                    location = createLocationFromPath(filepath, ds->baseLocation, ds->datasetLocation);
                    params.insert(std::pair<String,String>("location", location));
                }
                else {
                    ds->warning("Insert sequence failed; can't specify both location and filepath");
                    break;
                }
            }
            // param name
            if (params.count("name") == 0) {
                String name = createSeqnameFromPath(filepath);
                if (name.empty()) {
                    ds->warning("Insert sequence failed; invalid sequence name.");
                    break;
                }
                else {
                    params.insert(std::pair<String,String>("name", name));
                }
            }
            // param type (and check validity)
            if (params.count("type") == 0) {
                if (isVideoFile(filepath)) {
                    params.insert(std::pair<String,String>("type", "video"));
                }
                else if (isImageFolder(filepath)) {
                    params.insert(std::pair<String,String>("type", "images"));
                    params["location"].append("/");                    
                }
                else {
                    ds->warning("Insert sequence " + params["name"] + " failed; sequence must be an image folder or a video file.");
                    break;
                }
            }
            // insert video sequence
            if (params["type"].compare("video") == 0) {
                seq->add(params["name"], params["location"], params["type"], seq->user, "", "");
                if (!seq->addExecute()) {
                    seq->warning("Insert sequence " + params["name"] + " failed; video not inserted.");
                    break;
                }
            }
            // insert image folder sequence, load list of images
            else if (params["type"].compare("images") == 0) {
                std::set<String> imagelist;
                int t = 1;
                if (loadImageList(filepath, imagelist) == 0) {
                    seq->warning("Insert sequence " + params["name"] + " failed; image files could not have been loaded.");
                    break;
                }
                else {
                    Interval *img = seq->newInterval();
                    seq->add(params["name"], params["location"], params["type"], seq->user, "", "");
                    if (!seq->addExecute()) {
                        seq->warning("Insert sequence " + params["name"] + " failed; image folder not inserted.");
                        break;
                    }
                    for (std::set<String>::iterator it = imagelist.begin(); it != imagelist.end(); ++it) {
                        img->add(params["name"], t, t, *it, img->user, "");
                        if (!img->addExecute()) {
                            img->warning("Insert interval " + (*it) + " failed; image not inserted");
                        }
                        t++;
                    }
                    img->select->executed = true;
                    destruct(img);
                }
            }            
        } while (2==3);

        seq->select->executed = true;
        destruct(seq);
/*
            if (params.count("location") > 0 || params.count("type") > 0) {
                do {
                    if (params.count("name") == 0) {
                        String seqname = createSeqnameFromPath(params['location']);
                        params.insert(std::pair<String,String>("name", seqname));
                    }
                    if (params['type'].compare("images") == 0) {
                        // check if it's image folder
                        // load imagelist
                    }
                    else if (params['type'].compare("video") == 0) {
                        // check if it's video
                    }
                    else if (params['type'].compare("data") > 0) {
                        seq->warning("Insert sequence - unknown sequence type.");
                        break;
                    }
                    seq->add(params["name"], params["location"], params["type"], seq->user, "", "");
                    seq->addExecute();
                } while (2=3);

            }
            else seq->warning("Insert sequence - missing arguments.");
        } while (2=3);
        seq->select->executed = true;
        destruct (seq);
        */
    }
    // insert interval
    else if (input.compare("interval") == 0) {
        Sequence* seq = ds->newSequence(params["seqname"]);
        Interval* in = seq->newInterval(0, 0);
        in->add(params["seqname"], atoi(params["t1"].c_str()),
                atoi(params["t2"].c_str()), params["location"]);
        in->insert->execute();
        destruct (in);
        destruct (seq);
    }

    // insert process
    //TODO
    else if (input.compare("process") == 0) {
        this->vtapi->commons->warning("insert process not implemented");
    }
    else {
        this->vtapi->commons->warning("Only insert: sequence/interval/process");
    }

    destruct (ds);
}

//TODO
void VTCli::updateCommand(String& line) {
    this->vtapi->commons->warning("update command not implemented");
}
//TODO
void VTCli::deleteCommand(String& line) {
    this->vtapi->commons->warning("delete command not implemented");
}
//TODO
void VTCli::showCommand(String& line) {
    this->vtapi->commons->warning("show command not implemented");
}

void VTCli::installCommand(String& line) {

    String content;
    std::ifstream scriptfile (line.c_str(), std::ios::in);

    if (scriptfile)
    {
        scriptfile.seekg(0, std::ios::end);
        content.resize(scriptfile.tellg());
        scriptfile.seekg(0, std::ios::beg);
        scriptfile.read(&content[0], content.size());
        scriptfile.close();
    }
    else {
        this->vtapi->commons->warning("Failed to read file");
        return;
    }

    if (!content.empty()) {
        PGresult* qres = PQparamExec(vtapi->commons->getConnector()->getConn(),
                NULL, content.c_str(), PGF);
        if (qres) {
            KeyValues* kv = new KeyValues(*(this->vtapi->commons));
            kv->select = new Select(*(this->vtapi->commons));
            kv->select->res = qres;
            kv->select->executed = true;
            kv->printAll();
            destruct(kv);
        }
        else {
            String errmsg = line + " : " + PQgeterror();
            this->vtapi->commons->warning(errmsg);
        }
    }
}



bool VTCli::isImageFile(const String& filepath) {
    std::ifstream image(filepath.c_str());
    size_t dotPos = 0;

    if (image) {
        image.close();
        dotPos = filepath.find_last_of('.', String::npos);
        if (dotPos > 0 && dotPos < filepath.length()-1) {
            String suffix = filepath.substr(dotPos + 1, String::npos);
            if (this->imageSuffixes.count(suffix.c_str()) > 0) {
                return true;
            }
        }
    }
    return false;
}
bool VTCli::isImageFolder(const String& dirpath) {
    DIR *imagefolder = opendir(dirpath.c_str());

    if (imagefolder) {
        struct dirent *entry;
        while (entry = readdir(imagefolder)) {
            String filename (entry->d_name);
            if(filename.compare(".") != 0 && filename.compare("..") != 0) {
                String fullpath = dirpath + "/" + filename;
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
bool VTCli::isVideoFile(const String& filepath) {
    std::ifstream video(filepath.c_str());
    size_t dotPos = 0;
    
    if (video) {       
        video.close();
        dotPos = filepath.find_last_of('.', String::npos);
        if (dotPos > 0 && dotPos < filepath.length()-1) {
            String suffix = filepath.substr(dotPos + 1, String::npos);
            if (this->videoSuffixes.count(suffix.c_str()) > 0) {
                return true;
            }
        }
    }
    return false;
}

int VTCli::loadImageList(const String& dirpath, std::set<String>& imagelist) {
    DIR *imagefolder = opendir(dirpath.c_str());
    int cntimg = 0;

    if (imagefolder) {
        struct dirent *entry;
        while (entry = readdir(imagefolder)) {
            String filename (entry->d_name);
            if(filename[0] != '.') {
                String fullpath = dirpath + "/" + filename;
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

 void VTCli::initSufVectors() {
     String imgsuf[] = {"bmp", "jpeg", "jpg", "gif", "png", "svg", "tiff"};
     String vidsuf[] = {"avi", "mkv", "mpeg", "mpg", "mp4", "rm", "wmv"};

     this->imageSuffixes = std::set<String> (imgsuf, imgsuf + sizeof(imgsuf) / sizeof(imgsuf[0]));
     this->videoSuffixes = std::set<String> (vidsuf, vidsuf + sizeof(vidsuf) / sizeof(vidsuf[0]));
 }


/**
 * Creates key/value std::pair from string in key=value format
 * @param word Input string
 * @return Key,Value std::pair
 */
std::pair<String,String> VTCli::createKeyValue(const String& word) {
    size_t pos = word.find('=');

    if (pos > 0 && pos < word.length()-1)
        return std::pair<String,String>
                (word.substr(0,pos), word.substr(pos+1, String::npos));
    else
        return std::pair<String,String>("","");
}
/**
 *
 * @param filepath
 * @return
 */
String VTCli::createSeqnameFromPath(const String& filepath) {
    String seqname;
    size_t startPos = 0, endPos = String::npos;
    size_t len = filepath.length();
    size_t nsPos = len - 1;

    if (filepath.empty()) return "";
    while (filepath[nsPos] == '/') {
        nsPos--;
        if (nsPos < 0) return "";
    }
    startPos = filepath.find_last_of('/', nsPos);
    if (startPos == String::npos) {
        startPos = 0;
    }
    else {
        startPos++;
    }
    endPos = filepath.find_last_of('.', nsPos);
    if (endPos == String::npos || endPos <= startPos) endPos = nsPos + 1;

    seqname = filepath.substr(startPos, endPos - startPos);
    return seqname;
}

String VTCli::createLocationFromPath(const String& filepath, const String& baseLocation, const String& datasetLocation) {
    String part, location;
    String tmpBase = baseLocation;
    String tmpDataset = datasetLocation;
     
    fixSlashes(tmpBase);
    fixSlashes(tmpDataset);
    part = tmpBase + "/" + tmpDataset + "/";
    if (filepath.find(part) == 0) {
        location = filepath.substr(part.length(), String::npos);
    }
    else {
        location = filepath;
    }
    return location;
}


bool VTCli::isParam(const String& word) {
    size_t pos = word.find('=');
    return (pos > 0 && pos < word.length()-1);
}

/**
 * Cut first word from input command line
 * @param line input line
 * @return word
 */
String VTCli::cutWord(String& line) {
    String word;
    size_t startPos = 0, pos, endPos = String::npos;

    if (line.empty()) return "";

    // word end is whitespace, skip quoted part
        pos = line.find_first_of(" \t\n\"", startPos);
        if (pos != String::npos && line.at(pos) == '\"'){
            if (pos + 1 >= line.length()) {
                line.clear();
                return "";
            }
            endPos = line.find('\"', pos + 1);
            if (endPos == String::npos)
                word = line.substr(pos + 1, String::npos);
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
    if (endPos != String::npos && endPos < line.length()){
        endPos = line.find_first_not_of(" \t\n", endPos);
        if (endPos != String::npos) line = line.substr(endPos, String::npos);
        else line.clear();
    }
    else
        line.clear();

    return word;
}
/**
 * Cuts first comma-separated value from string
 * @param word CSV string
 * @return first CSV
 */
String VTCli::cutCSV(String& word) {
    String csv;
    size_t pos = word.find(",", 0);

    csv = word.substr(0, pos);
    if (pos != String::npos && pos+1 < word.length())
        word = word.substr(pos+1, String::npos);
    else word.clear();
    return csv;
}

bool VTCli::fixSlashes(String& path) {
    size_t len = path.length();
    size_t slPos = 0;
    size_t nsPos = len;

    do {
        slPos = path.find('\\', slPos);
        if (slPos != String::npos) {
            path[slPos] = '/';
        }
        else {
            break;
        }
    } while (2==3);
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


/**
 * Prints basic help
 * @return 0 on success, -1 on failure
 */
int VTCli::printHelp() {
    return this->printHelp("all");
}
/**
 * Prints help string
 * @param what Help context ("all", "select", ...)
 * @return 0 on success, -1 on failure
 */
int VTCli::printHelp(const String& what) {
    std::stringstream hss;

    if (this->helpStrings.count(what)) {
        std::cout << this->helpStrings[what];
        return 0;
    }
    else if (!what.compare("query")) {
        hss << std::endl <<
            "query [SQLSTRING]" << std::endl << std::endl <<
            "     * executes custom SQLQUERY" << std::endl <<
            "     * ex.: list methods with active processes" << std::endl <<
            "              query SELECT DISTINCT mtname FROM public.processes" << std::endl ;
    }
    else if (!what.compare("select")) {
        hss << std::endl <<
            "select dataset|sequence|interval|process|method|selection [ARGS]" << std::endl << std::endl <<
            "Selects data and prints them in specified format (-f option)" << std::endl << std::endl <<
            "ARG format:      arg=value or arg=value1,value2,..." << std::endl << std::endl <<
            " Dataset ARGS:" << std::endl <<
            "      name       name of the dataset" << std::endl <<
            "  location       base location of the dataset data files (directory)" << std::endl << std::endl <<
            " Sequence ARGS:" << std::endl <<
            "      name       name of the sequence" << std::endl <<
            "  location       location of the sequence data file(s) (file/directory)" << std::endl <<
            "       num       unique number of the sequence" << std::endl <<
            "      type       type of the sequence [images, video]" << std::endl << std::endl <<
            "Interval ARGS:" << std::endl <<
            "  seqname        name of the sequence containing this interval" << std::endl <<
            "        t1       begin time of the interval" << std::endl <<
            "        t2       end time of the interval" << std::endl <<
            "  location       location of the interval data file (file)" << std::endl << std::endl <<
            "Method ARGS:" << std::endl <<
            "      name       name of the method" << std::endl << std::endl <<
            "Process ARGS:" << std::endl <<
            "      name       name of the process" << std::endl <<
            "    method       name of the method the process is instance of" << std::endl <<
            "    inputs       data type of inputs (database table)" << std::endl <<
            "   outputs       data type of outputs (database table)" << std::endl << std::endl <<
            "Selection ARGS:" << std::endl <<
            "   (not implemented)" << std::endl;
    }
    else if (!what.compare("insert")) {
        hss << std::endl <<
            "insert sequence|interval|process [ARGS]" << std::endl << std::endl <<
            "Inserts data into database" << std::endl << std::endl <<
            "ARG format:      arg=value or arg=value1,value2,..." << std::endl << std::endl <<
            " Sequence ARGS:" << std::endl <<
            "      name       name of the sequence" << std::endl <<
            "  location       location of the sequence data file(s) (file/directory)" << std::endl <<
            "       num       unique number of the sequence" << std::endl <<
            "      type       type of the sequence [images, video]" << std::endl << std::endl <<
            "Interval ARGS:" << std::endl <<
            "   seqname       name of the sequence containing this interval *REQUIRED*" << std::endl <<
            "        t1       begin time of the interval *REQUIRED*" << std::endl <<
            "        t2       end time of the interval *REQUIRED*" << std::endl <<
            "  location       location of the interval data file (file)" << std::endl << std::endl <<
            "Process ARGS:" << std::endl <<
            "      name       name of the process *REQUIRED*" << std::endl <<
            "    method       name of the method the process is instance of" << std::endl <<
            "    inputs       data type of inputs (database table)" << std::endl <<
            "   outputs       data type of outputs (database table)" << std::endl;
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
        std::cout << hss.str() << std::endl;
        return 0;
    }
    else return -1;
}

/**
 * Creates help and command strings from arguments
 * @param argc Argument count
 * @param argv Argument field
 * @return 0 on success, -1 on failure
 */
int VTCli::processArgs(int argc, char** argv) {
    gengetopt_args_info args_info;
    if (cmdline_parser2 (argc, argv, &args_info, 0, 1, 0) != 0) {
        helpStrings.insert(std::make_pair("all", ""));
        this->cmdline.clear();
        return -1;
    }
    else {
        // Construct help string
        std::stringstream hss;
        int i = 0;
        hss << std::endl << CMDLINE_PARSER_PACKAGE_NAME <<" "<< CMDLINE_PARSER_VERSION << std::endl;
        hss << std::endl << gengetopt_args_info_usage << std::endl << std::endl;
        while (gengetopt_args_info_help[i])
            hss << gengetopt_args_info_help[i++] << std::endl;
        helpStrings.insert(std::make_pair("all", hss.str()));
        // Construct command string from unnamed arguments
        for (int i = 0; i < args_info.inputs_num; i++)
            cmdline.append(args_info.inputs[i]).append(" ");
        cmdline_parser_free (&args_info);
        return 0;
    }
}



// ************************************************************************** //
CLIInsert::CLIInsert(Commons& orig) {
    this->connector = (&orig)->getConnector();
    this->dataset = (&orig)->getDataset();
    this->type = GENERIC;
}
CLIInsert::~CLIInsert() {
    this->params.clear();
}

void CLIInsert::setType(InsertType newtype){
    this->type = newtype;
}

int CLIInsert::addParam(std::pair<String,String> param) {
    std::pair<std::map<String,String>::iterator,bool> ret;

    if (!param.first.empty() && !param.second.empty()) {
	ret = this->params.insert(param);
        // key already existed
        if (!ret.second) return -1;
        else return 0;
    }
    else return -1;
}

int CLIInsert::addParam(String key, String value) {
    return this->addParam(std::pair<String, String> (key, value));
}

String CLIInsert::getParam(String pname) {
    return this->params[pname];
}

void CLIInsert::getIntArray(String arrayParam, PGarray* arr) {

    size_t startPos = 0, endPos = String::npos;
    int tag;
    arr->ndims = 0;
    arr->param = PQparamCreate(this->connector->getConn());

    while (startPos < arrayParam.length()) {
        endPos = arrayParam.find(',', startPos);
        tag = atoi(arrayParam.substr(startPos, endPos).c_str());
        PQputf(arr->param, "%int4", tag);
        if (endPos == String::npos) break;
        startPos = endPos + 1;
    }
}

void CLIInsert::getFloatArray(String arrayParam, PGarray* arr) {

    size_t startPos = 0, endPos = String::npos;
    float svm;
    arr->ndims = 0;
    arr->param = PQparamCreate(this->connector->getConn());

    while (startPos < arrayParam.length()) {
        endPos = arrayParam.find(',', startPos);
        svm = (float) atof(arrayParam.substr(startPos, endPos).c_str());
        PQputf(arr->param, "%float4", svm);
        if (endPos == String::npos) break;
        startPos = endPos + 1;
    }
}

bool CLIInsert::checkLocation(String seqname, String intlocation) {
// TODO: nejede mi PQexec
/*
    String location;
    PGresult* res;
    PGtext seqlocation = (PGtext) "";

    // get sequence locationPQcle
    res = PQexec(this->connector->getConn(),
        String("SELECT seqlocation FROM " + this->dataset +
            ".sequences WHERE seqname=\'" + seqname.c_str() + "\';").c_str());
    if(!res) {
        this->connector->getLogger()->write(PQgeterror());
        return false;
    }
    PQgetf(res, 0, "%varchar", 0, &seqlocation);

    // FIXME: check location using getDataLocation();
    location = this->dataset.append((String) seqlocation).append(intlocation);
    // TODO:

    PQclear(res);
    return true;
*/
}

bool CLIInsert::execute() {

    std::stringstream query;
    PGresult *res;
    PGarray arr;
    PGparam *param = PQparamCreate(this->connector->getConn());
    bool queryOK = true;
    PGtimestamp timestamp = getTimestamp();

    if (this->type == GENERIC) {
        this->connector->getLogger()->write("Error: insert command incomplete\n");
        return 0;
    }

    //TODO: datasets/methods/selections
    // insert sequence
    if (this->type == SEQUENCE) {
        PQputf(param, "%name", getParam("name").c_str());
        PQputf(param, "%int4", atoi(getParam("seqnum").c_str()));
        PQputf(param, "%varchar", getParam("location").c_str());
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".sequences " <<
            "(seqname, seqnum, seqlocation, seqtyp, created) " <<
            "VALUES ($1, $2, $3, \'" << getParam("seqtype") << "\', $4)";
    }
    // insert interval
    else if (this->type == INTERVAL) {
        PQputf(param, "%name", getParam("sequence").c_str());
        PQputf(param, "%int4", atoi(getParam("t1").c_str()));
        PQputf(param, "%int4", atoi(getParam("t2").c_str()));
        PQputf(param, "%varchar", getParam("location").c_str());
        getIntArray(getParam("tags"), &arr);
        PQputf(param, "%int4[]", &arr);
        PQparamReset(arr.param);
        getFloatArray(getParam("svm"), &arr);
        PQputf(param, "%float4[]", &arr);
        PQparamClear(arr.param);
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".intervals " <<
            "(seqname, t1, t2, imglocation, tags, svm, created) " <<
            "VALUES ($1, $2, $3, $4, $5, $6, $7)";
        queryOK = checkLocation(getParam("sequence"), getParam("location"));
    }
    // insert process
    if (this->type == PROCESS) {
        PQputf(param, "%name", getParam("name").c_str());
        PQputf(param, "%name", atoi(getParam("method").c_str()));
        PQputf(param, "%varchar", getParam("inputs").c_str());
        PQputf(param, "%varchar", getParam("outputs").c_str());
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".processes " <<
            "(prsname, mtname, inputs, outputs, created) VALUES ($1, $2, $3, $4, $5)";
    }

    if (queryOK) {
        res = PQparamExec(this->connector->getConn(), param, query.str().c_str(), 1);
        if(!res)
            this->connector->getLogger()->write(PQgeterror());
       // TODO: nejede mi PQclear 
        /* else PQclear(res); */
    }

    PQparamClear(param);

    return 1;
}

void CLIInsert::clear(){

    this->params.clear();
    this->type = GENERIC;
}

PGtimestamp CLIInsert::getTimestamp() {
    PGtimestamp timestamp;
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    timestamp.date.isbc  = 0;
    timestamp.date.year  = timeinfo->tm_year + 1900;
    timestamp.date.mon   = timeinfo->tm_mon;
    timestamp.date.mday  = timeinfo->tm_mday;
    timestamp.time.hour  = timeinfo->tm_hour;
    timestamp.time.min   = timeinfo->tm_min;
    timestamp.time.sec   = timeinfo->tm_sec;
    timestamp.time.usec  = 0;
    return timestamp;
}




























/**
 * The VTCli main function
 *
 * @param argc
 * @param argv
 * @return sucess
 */
int main(int argc, char** argv) {

    VTCli* vtcli = new VTCli(argc, argv);

    vtcli->run();

    destruct (vtcli);

    return 0;
}










