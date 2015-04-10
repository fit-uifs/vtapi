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
#include <list>

#include "vtcli.h"

using namespace vtapi;
using namespace std;

#define PVAL(name)      string name = getParamValue(params, #name);
#define PVAL_OK(name)   (!name.empty())
#define FPST(name)      fixPathSlashes(name, true)
#define FPSNT(name)     fixPathSlashes(name, false)


const VTCli::VTCLI_COMMAND_DEF VTCli::m_cmd[] =
{
    { CMD_SELECT,   "select",   OBJ_ALL,
        "selects data and prints them in specified format (default: CSV)",
        "select all videos from dataset:\n"
        "\tselect sequence type=video"},
    { CMD_INSERT,   "insert",   OBJ_DATASET | OBJ_SEQUENCE | OBJ_INTERVAL | OBJ_PROCESS,
        "inserts new data into database",
        "insert video from dataset location with given real-world start time:\n"
        "\tinsert sequence type=video location=video1.mpg realtime=1427976389"},
    { CMD_DELETE,   "delete",   OBJ_DATASET | OBJ_PROCESS,
        "deletes data from database",
        "delete data calculated by process:\n"
        "\tdelete process name=demo1p_5000_25"},
    { CMD_LOAD,     "load",     OBJ_DATASET,
        "loads entire dataset into database",
        "load dataset:\n"
        "\tload dataset"},
    { CMD_STATS,    "stats",     OBJ_INTERVAL,
        "calculates coverage of video intervals by events for one process\n"
        "\tformat: length(bits),bitmap(base64)\n"
        "\t(bitmap, where 1 = frame covered by event)",
        "get coverage for video1 and specified process:\n"
        "\tstats interval sequence=video1 process=demo1p_5000_video1" },
    { CMD_TEST,     "test",     OBJ_NONE,
        "runs VTApi unit tests",
        NULL},
    { CMD_HELP,     "help",     OBJ_NONE,
        "prints this help",
        NULL},
    { CMD_EXIT,     "exit",     OBJ_NONE,
        "exits interactive mode",
        NULL},
    { CMD_NONE,     NULL,       OBJ_NONE,
        NULL,
        NULL}
};

const VTCli::VTCLI_OBJECT_DEF VTCli::m_obj[] = 
{
    { OBJ_DATASET,      "dataset",      PAR_NAME|PAR_LOCATION },
    { OBJ_SEQUENCE,     "sequence",     PAR_NAME|PAR_LOCATION|PAR_TYPE|PAR_REALTIME },
    { OBJ_INTERVAL,     "interval",     PAR_ID|PAR_PROCESS|PAR_OUTPUTS|PAR_SEQUENCE|PAR_T1|PAR_T2|PAR_DURATION|PAR_LOCATION },
    { OBJ_PROCESS,      "process",      PAR_NAME|PAR_METHOD|PAR_INPUTS|PAR_OUTPUTS },
    { OBJ_METHOD,       "method",       PAR_NAME },
    { OBJ_METHODKEYS,   "methodkeys",   PAR_METHOD },
    { OBJ_NONE,         NULL }
};

const VTCli::VTCLI_PARAM_DEF VTCli::m_par[] = 
{
    { PAR_ID,       "id",       "numeric object identifier (interval only)" },
    { PAR_NAME,     "name",     "object name" },
    { PAR_LOCATION, "location", "object location (relative to base and dataset path)" },
    { PAR_TYPE,     "type",     "sequence type (video|images)" },
    { PAR_REALTIME, "realtime", "sequence real-world start time (POSIX time)" },
    { PAR_T1,       "t1",       "interval start frame" },
    { PAR_T2,       "t2",       "interval end frame" },
    { PAR_DURATION, "duration", "interval duration (in frames)" },
    { PAR_PROCESS,  "process",  "interval filter by process name" },
    { PAR_SEQUENCE, "sequence", "interval filter by sequence name" },
    { PAR_METHOD,   "method",   "process/methodkeys filter by method" },
    { PAR_INPUTS,   "inputs",   "process filter by input process name" },
    { PAR_OUTPUTS,  "outputs",  "process filter by output table" },
    { PAR_NONE,     NULL,       NULL }
};



////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    VTCli app;
    return app.run(argc, argv);
}

////////////////////////////////////////////////////////
// VTCli class implementation
////////////////////////////////////////////////////////

VTCli::VTCli()
{
    m_vtapi = NULL;
}

VTCli::~VTCli()
{
    if (m_vtapi) delete m_vtapi;
}

int VTCli::run(int argc, char** argv)
{
    bool bRet = true;
    string line;

    do {
        // get line for non-interactive mode
        bRet = processArgs(argc, argv, line);
        if (!bRet) break;
        
        // main vtapi instance
        m_vtapi = new VTApi(argc, argv);
        if (!m_vtapi) { bRet = false; break; }
        
        // non-interactive => one command and exit
        if (!line.empty()) {
            bRet = handleCommandLine(line);
            break;
        }
        // interactive, wait for exit command
        else {
            cout << "VTCli is running" << endl;
            cout << "Type 'help' for list of available commands" << endl;

            bool bStop = false;
            do {
                cout << endl << "> ";
                cout.flush();
                
                getline(cin, line);
                if (cin.fail()) break;  // EOF detected

                bRet = handleCommandLine(line, &bStop);
            } while(/*bRet && */!bStop);
        }
        
    } while(0);
    
    return bRet ? 0 : 1;
}

bool VTCli::processArgs(int argc, char** argv, string& cmdline)
{
    gengetopt_args_info args_info = { 0 };

    bool bRet = (cmdline_parser2 (argc, argv, &args_info, 0, 1, 0) == 0);
    if (bRet) {
        // Construct command string from unnamed arguments
        for (int i = 0; i < args_info.inputs_num; i++) {
            cmdline.append(args_info.inputs[i]).append(" ");
        }
        cmdline_parser_free (&args_info);
    }
    else {
        printError("failed to process command line arguments");
        cmdline.clear();
    }

    return bRet;
}
bool VTCli::handleCommandLine(const std::string& line, bool *pbStop)
{
    VTCLI_KEYVALUE_LIST tokens;
    bool bRet = (tokenizeLine(line, tokens, ' ', true) > 0);
    
    if (bRet) {
        string token = tokens.front().second;
        tokens.pop_front();
        switch (getCommand(token))
        {
        case CMD_NONE:
            printError(string("unknown command: ").append(token));
            bRet = false;
            break;
        case CMD_SELECT:
            bRet = selectCommand(tokens);
            break;
        case CMD_INSERT:
            bRet = insertCommand(tokens);
            break;
        case CMD_DELETE:
            bRet = deleteCommand(tokens);
            break;
        case CMD_LOAD:
            bRet = loadCommand(tokens);
            break;
        case CMD_STATS:
            bRet= statsCommand(tokens);
            break;
        case CMD_TEST:
            bRet = testCommand();
            break;
        case CMD_HELP:
            bRet = helpCommand();
            break;
        case CMD_EXIT:
            if (pbStop) *pbStop = true;
            break;
        }
    }
    else {
        printError(string("failed to parse line: ").append(line));
    }
    
    return bRet;
}

size_t VTCli::tokenizeLine(const string& line, VTCLI_KEYVALUE_LIST& tokens, const char delimiter, bool skipMultipleDelimiters)
{
    bool inquotes = false;
    bool intoken = !skipMultipleDelimiters;
    size_t tok = 0;
    VTCLI_KEYVALUE kv;

    for (size_t i = 0; i < line.length(); i++) {
        if(line[i] == delimiter) {
            if (intoken && !inquotes) {
                tokens.push_back(kv);
                if (createKeyValue(line.substr(tok, i - tok), tokens.back())) {
                    if (skipMultipleDelimiters) {
                        intoken = false;
                    }
                    else {
                        tok = i + 1;
                    }
                }
                else {
                    tokens.clear();
                    break;
                }
            }
        }
        else {
            if (!intoken) {
                tok = i;
                intoken = true;
            }
            if (line[i] == '\"') {
                inquotes != inquotes;
            }
        }
    }
    
    if (intoken) {
        tokens.push_back(kv);
        
        string token;
        if (tok < line.length()) {
            token = line.substr(tok, string::npos);
        }
        if (!createKeyValue(token, tokens.back())) {
            tokens.clear(); 
        }
    }
    
    return tokens.size();
}

bool VTCli::createKeyValue(const string& word, VTCLI_KEYVALUE& keyValue)
{
    size_t pos = word.find('=');
    
    // single value
    if (pos == string::npos) {
        keyValue.first.clear();
        keyValue.second = word;
    }
    // key=value
    else if (pos > 0 && pos < word.length() - 1) {
        keyValue.first = word.substr(0, pos);
        if (word[pos+1] == '\"' && word[word.length()-1] == '\"') {
            keyValue.second = word.substr(pos+2, word.length() - pos - 3);
        }
        else {
            keyValue.second = word.substr(pos+1, string::npos);
        }
    }
    else {
        printError(string("failed to parse argument: ").append(word));
        return false;
    }
    
    return true;
}

string VTCli::getParamValue(VTCLI_KEYVALUE_LIST& keyValues, const string& key)
{
    string value;
    VTCLI_KEYVALUE_LIST::iterator it;
    
    for (it = keyValues.begin(); it != keyValues.end(); it++) {
        if ((*it).first.compare(key) == 0) {
            value = (*it).second;
            break;
        }
    }
    if (!value.empty()) keyValues.erase(it);
    
    return value;
}


VTCli::VTCLI_COMMAND VTCli::getCommand(const std::string& word)
{
    VTCLI_COMMAND cmd = CMD_NONE;
    
    for (size_t i = 0; m_cmd[i].name; i++) {
        if (word.compare(m_cmd[i].name) == 0) {
            cmd = m_cmd[i].cmd;
            break;
        }
    }

    return cmd;
}

VTCli::VTCLI_OBJECT VTCli::getObject(const std::string& word)
{
    VTCLI_OBJECT obj = OBJ_NONE;

    for (size_t i = 0; m_obj[i].name; i++) {
        if (word.compare(m_obj[i].name) == 0) {
            obj = m_obj[i].obj;
            break;
        }
    }

    return obj;
}

bool VTCli::selectCommand(VTCLI_KEYVALUE_LIST& params)
{
    bool bRet = true;
    
    do {
        if (params.empty()) {
            printError("what object to select? type 'help' for list");
            bRet = false;
            break;
        }

        string param = params.front().second;
        params.pop_front();
        switch (getObject(param))
        {
        case OBJ_DATASET:
        {
            PVAL(name); PVAL(location);
            
            Dataset* ds = new Dataset(*(m_vtapi->commons), name);
            if (PVAL_OK(location))  ds->select->whereString("dslocation", location);
            
            ds->next();
            ds->printAll();
            delete ds;
            break;
        }
        case OBJ_SEQUENCE:
        {
            PVAL(name); PVAL(location); PVAL(type);
            
            Sequence* seq = new Sequence(*(m_vtapi->commons), name);
            if (PVAL_OK(location))  seq->select->whereString("seqlocation", location);
            if (PVAL_OK(type))      seq->select->whereSeqtype("seqtyp", type);
            
            seq->next();
            seq->printAll();
            delete seq;
            break;
        }
        case OBJ_INTERVAL:
        {
            PVAL(id); PVAL(process); PVAL(outputs); PVAL(sequence);
            PVAL(t1); PVAL(t2); PVAL(duration); PVAL(location);

            // find table with intervals (directly by 'outputs' or by 'process' outputs)
            Interval* in = NULL;
            if (PVAL_OK(outputs)) {
                in = new Interval(*(m_vtapi->commons), outputs);
            }
            else if (PVAL_OK(process)) {
                Process *p = new Process(*(m_vtapi->commons), process);
                if (p->next()) {
                    in = new Interval(*(m_vtapi->commons), p->getOutputs());
                }
                else {
                    printError(string("process doesn't exist: ").append(process).c_str());
                    bRet = false;
                    break;
                }
                delete p;
            }
            // default table = 'intervals'
            else {
                in = new Interval(*(m_vtapi->commons));
            }
            
            if (PVAL_OK(id))        in->select->whereInt("id", atoi(id.c_str()));
            if (PVAL_OK(process))   in->select->whereString("prsname", process);
            if (PVAL_OK(sequence))  in->select->whereString("seqname", sequence);
            if (PVAL_OK(t1))        in->select->whereInt("t1", atoi(t1.c_str()), ">=");
            if (PVAL_OK(t2))        in->select->whereInt("t2", atoi(t2.c_str()), "<=");
            if (PVAL_OK(location))  in->select->whereString("imglocation", location);
            //TODO: duration

            in->next();
            in->printAll();
            delete in;
            break;
        }
        case OBJ_PROCESS:
        {
            PVAL(name); PVAL(method); PVAL(inputs); PVAL(outputs);

            Process* pr = new Process(*(m_vtapi->commons), name);
            if (PVAL_OK(method))    pr->select->whereString("mtname", method);
            if (PVAL_OK(inputs))    pr->select->whereString("inputs", inputs);
            if (PVAL_OK(outputs))   pr->select->whereString("outputs", outputs);
            
            pr->next();
            pr->printAll();
            delete pr;
            break;
        }
        case OBJ_METHOD:
        {
            PVAL(name);

            Method* me = new Method(*(m_vtapi->commons), name);
            
            me->next();
            me->printAll();
            delete me;
            break;
        }
        case OBJ_METHODKEYS:
        {
            PVAL(method);

            Method* me = new Method(*(m_vtapi->commons), method);
            
            me->next();
            me->printMethodKeys();
            delete me;
            break;
        }
        default:
        {
            printError(string("invalid object: ").append(param));
            bRet = false;
            break;
        }
        }
        
    } while(0);

    return bRet;
}

bool VTCli::insertCommand(VTCLI_KEYVALUE_LIST& params)
{
    bool bRet = true;
    
    do {
        if (params.empty()) {
            printError("what object to insert? type 'help' for list");
            bRet = false;
            break;
        }

        string param = params.front().second;
        params.pop_front();
        switch (getObject(param))
        {
        case OBJ_DATASET:
        {
            printError("inserting datasets should be done manually (for now)");
            break;
        }
        case OBJ_SEQUENCE:
        {
            PVAL(location); PVAL(type); PVAL(realtime);

            if (!PVAL_OK(location)) {
                printError("sequence location not specified(location=<relativepath>)");
                bRet = false;
                break;
            }
            if (!PVAL_OK(type)) {
                printError("sequence type not specified(type=video|images)");
                bRet = false;
                break;
            }

            Dataset* ds = m_vtapi->newDataset();
            if (ds->next()) {
                // load one video
                if (type.compare("video") == 0) {
                    bRet = loadVideo(ds, FPSNT(location), PVAL_OK(realtime) ? atol(realtime.c_str()) : 0);
                }
                    // load directory of images
                else if (type.compare("images") == 0) {
                    bRet = loadImageFolder(ds, FPST(location));
                }
                else {
                    printError("invalid sequence type");
                    bRet = false;
                }
            }
            else {
                printError(string("failed to open dataset: ").append(ds->getDataset()));
                bRet = false;
            }
            delete ds;

            break;
        }
        case OBJ_INTERVAL:
        {
            PVAL(process); PVAL(outputs); PVAL(sequence);
            PVAL(t1); PVAL(t2); PVAL(location);

            if (!PVAL_OK(sequence)) {
                printError("parent sequence not specified(sequence=<sequencename>)");
                bRet = false;
                break;
            }
            if (!PVAL_OK(t1)) {
                printError("start time not specified(t1=<time>)");
                bRet = false;
                break;
            }
            
            // find table with intervals (directly by 'outputs' or by 'process' outputs)
            Interval* in = NULL;
            if (PVAL_OK(process)) {
                Process *p = new Process(*(m_vtapi->commons), process);
                if (p->next()) {
                    in = p->newInterval();
                }
                else {
                    printError(string("process doesn't exist: ").append(process).c_str());
                    bRet = false;
                    break;
                }
                delete p;
            }
            else if (PVAL_OK(outputs)) {
                in = new Interval(*(m_vtapi->commons), outputs);
            }
            // default table = 'intervals'
            else {
                in = new Interval(*(m_vtapi->commons));
            }
            
            // create end time
            if (!PVAL_OK(t2)) t2 = t1;
            
            // add interval and execute
            if (in->add(sequence, atoi(t1.c_str()), atoi(t2.c_str()), location)) {
                if (!in->addExecute()) {
                    printError("failed to insert interval");
                    bRet = false;
                }
            }
            else {
                printError("failed to add interval");
                bRet = false;
            }

            in->select->executed = true;
            delete in;
            break;
        }
        case OBJ_PROCESS:
        {
            printError("inserting processes should be done by launcher");
            break;
        }
        default:
        {
            printError(string("invalid object: ").append(param));
            bRet = false;
            break;
        }
        }
        
    } while(0);
    
    return bRet;
}

bool VTCli::deleteCommand(VTCLI_KEYVALUE_LIST& params)
{
    bool bRet = true;

    do {
        if (params.empty()) {
            printError("what object to delete? type 'help' for list");
            bRet = false;
            break;
        }

        string param = params.front().second;
        params.pop_front();
        switch (getObject(param))
        {
        case OBJ_DATASET:
        {
            printError("deleting datasets should be done manually (for now)");
            break;
        }
        case OBJ_PROCESS:
        {
            PVAL(name);
            
            if (!PVAL_OK(name)) {
                printError("process name not specified (name=<processname>)");
                bRet = false;
                break;
            }
            
            Process *pr = new Process(*(m_vtapi->commons), name);
            if (pr->next()) {
                pr->deleteOutputData();
                Query q(*(m_vtapi->commons), "DELETE FROM " + pr->getDataset() + ".processes WHERE prsname = '" + name + "';");
                q.execute();
            }
            else {
                printError(string("process doesn't exist: ").append(name));
                bRet = false;
            }
            
            delete pr;
            break;
        }
        default:
        {
            printError(string("invalid object: ").append(param));
            bRet = false;
            break;
        }
        }

    } while (0);

    return bRet;
}

bool VTCli::loadCommand(VTCLI_KEYVALUE_LIST& params)
{
    bool bRet = true;

    do {
        if (params.empty()) {
            printError("what object to load? type 'help' for list");
            bRet = false;
            break;
        }

        string param = params.front().second;
        params.pop_front();
        switch (getObject(param))
        {
        case OBJ_DATASET:
        {
            Dataset* ds = m_vtapi->newDataset();
            
            if (ds->next()) {
                bRet = loadDirectory(ds, ds->getBaseLocation() + ds->getDatasetLocation(), "");
            }
            else {
                printError("failed to open dataset");
                bRet = false;
            }
            
            delete ds;
            break;
        }
        default:
        {
            printError(string("invalid object: ").append(param));
            bRet = false;
            break;
        }
        }

    } while (0);

    return bRet;
}

bool VTCli::statsCommand(VTCLI_KEYVALUE_LIST& params)
{
    bool bRet = true;

    do {
        if (params.empty()) {
            printError("what object for stats? type 'help' for list");
            bRet = false;
            break;
        }

        string param = params.front().second;
        params.pop_front();
        switch (getObject(param))
        {
        case OBJ_INTERVAL:
        {
            PVAL(sequence); PVAL(process);

            if (!PVAL_OK(sequence)) {
                printError("parent sequence not specified(sequence=<sequencename>)");
                bRet = false;
                break;
            }
            if (!PVAL_OK(process)) {
                printError("process not specified(process=<processname>)");
                bRet = false;
                break;
            }

            Video *vid = new Video(*(m_vtapi->commons), sequence);
            Process *pr = new Process(*(m_vtapi->commons), process);
            Interval *outs = NULL;
            unsigned char *bitmap = NULL;
            
            do {
                // check video and process for existence
                if (!vid->next()) {
                    printError(string("sequence doesn't exist: ").append(sequence).c_str());
                    bRet = false;
                    break;
                }
                if (!pr->next()) {
                    printError(string("process doesn't exist: ").append(process).c_str());
                    bRet = false;
                    break;
                }

                // get video length in frames
                size_t len = vid->getLength();
                if (!len) {
                    printError(string("failed to get video length: ").append(sequence).c_str());
                    bRet = false;
                    break;
                }
                
                // allocate bitmap
                size_t lenAlloc = ((len + 7) >> 3) << 3;
                bitmap = (unsigned char *)malloc(lenAlloc);
                if (!bitmap) {
                    stringstream ss;
                    ss << "failed to allocate bitmap of size: " << lenAlloc;
                    printError(ss.str());
                    bRet = false;
                    break;
                }
                
                // get process outputs and filter by sequence
                Interval* outs = pr->getOutputData();
                outs->filterBySequence(sequence);
                
                // go through outputs and fill bitmap
                memset(bitmap, 0, lenAlloc);
                while (outs->next()) {
                    int t1 = outs->getStartTime() - 1;
                    int x1 = (t1 >> 3) << 3;
                    int y1 = 7 - (t1 & 0x00000007);
                    int bits = outs->getEndTime() - t1;
                    
                    for (int x = x1; bits; x++) {
                        for (int y = 7; y >= y1 && bits; y++) {
                            bitmap[x] |= (1 << y);
                            bits--;
                        }
                        y1 = 0;
                    }
                }
                
                // print length,bitmap
                cout << len << ',' << base64_encode(bitmap, lenAlloc) << endl;

                
            } while(0);
            
            if (bitmap) free(bitmap);
            if (outs) delete outs;
            if (pr) delete pr;
            if (vid) delete vid;

            break;
        }
        default:
        {
            printError(string("invalid object: ").append(param));
            bRet = false;
            break;
        }
        }

    } while (0);

    return bRet;
}

bool VTCli::testCommand()
{
    m_vtapi->test();
    return true;
}

bool VTCli::helpCommand()
{
    cout <<
    "VTCli - VTApi command line interface" << endl <<
    "Using " CMDLINE_PARSER_PACKAGE_NAME " " CMDLINE_PARSER_VERSION << endl << endl <<
    "Usage:" << endl <<
    "   ./vtcli [VTAPI_OPTIONS] COMMAND [OBJECT [PARAMS]]" << endl << endl <<
    "Interactive mode:" << endl <<
    "   ./vtcli [VTAPI_OPTIONS]" << endl <<
    "       > COMMAND [OBJECT [PARAMS]]" << endl <<
    "       > ..." << endl <<
    "       > exit" << endl << endl <<
    "--------------------- VTAPI OPTIONS ---------------------" << endl << endl;
    
    for (size_t i = 0; gengetopt_args_info_help[i]; i++) {
        cout << gengetopt_args_info_help[i] << endl;
    }
    cout << endl;
    
    cout << 
    "--------------------- COMMANDS ---------------------" << endl << endl;
    
    for (size_t i = 0; m_cmd[i].name; i++) {
        if (i > 0) cout << ",";
        cout << m_cmd[i].name;
    }
    
    cout << endl << endl <<
    "Some commands are valid only for certain OBJECTS" << endl << endl;

    for (size_t i = 0; m_cmd[i].name; i++) {
        cout << i + 1 << ". " << m_cmd[i].name << endl <<
            "  * " << m_cmd[i].desc << endl;
        if (m_cmd[i].objects != OBJ_NONE) {
            cout << "  * OBJECTS: " << getObjectsString(m_cmd[i].objects) << endl;
        }
        if (m_cmd[i].ex) {
            cout << "  * ex. " << m_cmd[i].ex << endl << endl;
        }
        else {
            cout << endl;
        }
    }

    cout <<
    "--------------------- OBJECTS ---------------------" << endl << endl;

    for (size_t i = 0; m_obj[i].name; i++) {
        if (i > 0) cout << ",";
        cout << m_obj[i].name;
    }
    
    cout << endl << endl <<
    "Objects to select/insert/delete may be specified by PARAMS (format: PARAM=value)" << endl << 
    "No PARAMS usually means ALL objects (careful with delete)" << endl << endl;
    
    for (size_t i = 0; m_obj[i].name; i++) {
        cout << i + 1 << ". " << m_obj[i].name << endl;
        if (m_obj[i].params != PAR_NONE) {
            cout << "  * PARAMS: " << getParamsString(m_obj[i].params) << endl << endl;
        }
    }

    cout <<
    "--------------------- PARAMS ---------------------" << endl << endl;

    for (size_t i = 0; m_par[i].name; i++) {
        cout << left << setw(12) << m_par[i].name << m_par[i].desc  << endl;
    }
    cout << endl;
}

bool VTCli::loadDirectory(Dataset *ds, const string& basepath, const string& dirpath)
{   
    bool bRet = true;
    DIR *dir = NULL;
    
    do {
        string fulldirpath = basepath + dirpath;

        dir = opendir(fulldirpath.c_str());
        if (!dir) {
            printError(string("cannot open directory: ").append(fulldirpath));
            bRet = false;
            break;
        }

        Video *vid = ds->newVideo();
        struct dirent *pEntry;
        while (bRet && ((pEntry = readdir(dir)) != NULL)) {
            if (pEntry->d_type == DT_DIR) {
                if (pEntry->d_name[0] == '.' && (pEntry->d_name[1] == '\0' || pEntry->d_name[1] == '.')) continue;
                bRet = loadDirectory(ds, basepath, FPST(dirpath + pEntry->d_name));
            }
            else if (pEntry->d_type == DT_REG) {
                std::string fullpath = dirpath + pEntry->d_name;
                bRet = vid->add(getFileNameNoExt(pEntry->d_name), fullpath);
                if (bRet) {
                    cout << fullpath << endl;
                    bRet = vid->addExecute();
                }
                else {
                    printError(string("failed to open video: ").append(fullpath));
                }
                
            }
        }
        vid->select->executed = true;
        delete vid;
        
    } while(0);
    
    if (dir) closedir(dir);
    
    return bRet;
}

bool VTCli::loadVideo(Dataset *ds, const std::string &filepath, const time_t& realtime)
{
    bool bRet = true;
    
    Video *vid = ds->newVideo();
    if (vid->add(getFileNameNoExt(filepath), filepath, realtime)) {
        if (!vid->addExecute()) {
            printError(string("failed to insert video: ").append(filepath).c_str());
            bRet = false;
        }
    }
    else {
        string fullpath = ds->getBaseLocation() + ds->getDatasetLocation() + filepath;
        printError(string("failed to open video: ").append(fullpath).c_str());
        bRet = false;
    }

    vid->select->executed = true;
    delete vid;
    
    return bRet;
}

bool VTCli::loadImageFolder(Dataset *ds, const string &dirpath)
{
    bool bRet = true;
    DIR *dir = NULL;
    ImageFolder *ifld = NULL;
    Image *im = NULL;
    
    do {
        string fullpath = ds->getBaseLocation() + ds->getDatasetLocation() + dirpath;
       
        ifld = ds->newImageFolder();
        if (ifld->add(getFileNameNoExt(dirpath), dirpath)) {
            if (!ifld->addExecute()) {
                printError(string("failed to insert image folder: ").append(dirpath).c_str());
                bRet = false;
                break;
            }
        }
        else {
            printError(string("failed to open image folder: ").append(fullpath).c_str());
            bRet = false;
            break;
        }

        dir = opendir(fullpath.c_str());
        if (!dir) {
            printError(string("failed to browse image folder: ").append(fullpath).c_str());
            bRet = false;
            break;
        }
        
        ifld->next();
        im = ifld->newImage();
        
        size_t cnt = 0;
        struct dirent *entry;
        while (entry = readdir(dir)) {
            if (entry->d_type == DT_REG) {
                if (!im->add(ifld->getName(), ++cnt, entry->d_name)) {
                    printError(string("failed to open image: ").append(entry->d_name).c_str());
                    bRet = false;
                    break;
                }
            }
        }
        
        if (cnt > 0) {
            if (!im->addExecute()) {
                printError(string("failed to insert image(s) from folder: ").append(dirpath).c_str());
                bRet = false;
                break;
            }
        }
    } while(0);

    if (im) { im->select->executed = true; delete im; }
    if (ifld) delete ifld;
    if (dir) closedir(dir);

    return bRet;
}

string VTCli::fixPathSlashes(const string& path, bool bTrailing)
{
    string ret;
    
    size_t pos = path.find_last_not_of("/\\");
    if (pos != string::npos) {
        ret = path.substr(0, pos+1);
        if (bTrailing) ret += '/';
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
    
    size_t len = (end != string::npos) ? end - start : string::npos;
    return path.substr(start, len);
}

string VTCli::getObjectsString(unsigned int objects)
{
    string ret;

    for (size_t i = 0; m_obj[i].name; i++) {
        if (objects & m_obj[i].obj) {
            if (!ret.empty()) ret += ',';
            ret += m_obj[i].name;
        }
    }
    
    if (ret.empty()) ret = "(none)";

    return ret;
}

string VTCli::getParamsString(unsigned int params)
{
    string ret;

    for (size_t i = 0; m_par[i].name; i++) {
        if (params & m_par[i].par) {
            if (!ret.empty()) ret += ',';
            ret += m_par[i].name;
        }
    }
    
    if (ret.empty()) ret = "(none)";
    
    return ret;
}

void VTCli::printError(const char* errmsg)
{
    cerr << "ERROR: " << errmsg << endl;
}

void VTCli::printError(const string& errmsg)
{
    printError(errmsg.c_str());
}
