/*
 * File:    vtcli.cpp
 * Author:  Vojtěch Fröml
 *          xfroml00 (at) stud.fit.vutbr.cz
 *
 * VTCli is a command line tool for easy manipulation with VTApi structures.
 * For more detailed description see header vtcli.h
 */

#include <cstdlib>
#include <cstring>
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
#define PVAL_WARN(name) printError("failed to parse parameter: " #name);
#define PVAL_PARSE_UINT(name) \
    unsigned int name ## _uint = 0;\
    if (!parseUintValue(name, &name ## _uint)) PVAL_WARN(name);
#define PVAL_PARSE_UINT_RANGE(name) \
    unsigned int name ## 1_uint = 0, name ## 2_uint = 0;\
    if (!parseUintRangeValue(name, &name ## 1_uint, &name ## 2_uint)) PVAL_WARN(name);
#define PVAL_PARSE_FLOAT(name) \
    double name ## _float = 0.0;\
    if (!parseFloatValue(name, &name ## _float)) PVAL_WARN(name);
#define PVAL_PARSE_FLOAT_RANGE(name) \
    double name ## 1_float = 0.0, name ## 2_float = 0.0;\
    if (!parseFloatRangeValue(name, &name ## 1_float, &name ## 2_float)) PVAL_WARN(name);
#define PVAL_PARSE_REGION(name) \
    IntervalEvent::box name ## _region;\
    if (!parseRegionValue(name, &name ## _region)) PVAL_WARN(name);
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
        "\tformat: length(bits),coverage(percents)[,bitmap(base64)]\n"
        "\t(bitmap output: 1 = frame covered by event)",
        "get coverage for video1 and specified process (with/without bitmap):\n"
        "\tstats interval sequence=video1 process=demo1p_5000_video1\n"
        "\tstats interval sequence=video1 process=demo1p_5000_video1 bitmap=true" },
    { CMD_CONTROL,  "control",  OBJ_PROCESS,
        "sends control message to running process\n",
        "\t control process name=demo1p_5000_video1 command=stop" },
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
    { OBJ_INTERVAL,     "interval",     PAR_ID|PAR_PROCESS|PAR_OUTPUTS|PAR_SEQUENCE|PAR_LOCATION|PAR_T1|PAR_T2|PAR_DURATION|PAR_TIMERANGE|PAR_REGION|PAR_BITMAP },
    { OBJ_PROCESS,      "process",      PAR_NAME|PAR_METHOD|PAR_INPUTS|PAR_OUTPUTS },
    { OBJ_METHOD,       "method",       PAR_NAME },
    { OBJ_METHODKEYS,   "methodkeys",   PAR_METHOD },
    { OBJ_NONE,         NULL }
};

const VTCli::VTCLI_PARAM_DEF VTCli::m_par[] = 
{
    { PAR_ID,           "id",           "object ID", "id=5" },
    { PAR_NAME,         "name",         "object name", "name=my_sequence" },
    { PAR_LOCATION,     "location",     "object location (relative to base and dataset path)", "location=video3.mpg" },
    { PAR_TYPE,         "type",         "sequence type (values=video|images)", "type=video" },
    { PAR_REALTIME,     "realtime",     "sequence real time start (UNIX time)", "realtime=1429531506" },
    { PAR_T1,           "t1",           "interval start frame", "t1=256" },
    { PAR_T2,           "t2",           "interval end frame", "t2=531" },
    { PAR_DURATION,     "duration",     "interval filter by duration in seconds", "duration=\"(0.5,3.5)\"" },
    { PAR_TIMERANGE,    "timerange",    "interval filter by time range overlap (UNIX time)", "timerange=\"(1429531506,1429617906)\"" },
    { PAR_REGION,       "region",       "interval filter by event region overlap(box corners)", "region=\"((0.1,0.31),(0.72,0.6))\"" },
    { PAR_PROCESS,      "process",      "interval filter by process name", "process=demo1p_5000_25" },
    { PAR_SEQUENCE,     "sequence",     "interval filter by sequence name", "sequence=video1" },
    { PAR_METHOD,       "method",       "process/methodkeys filter by method", "method=demo1" },
    { PAR_INPUTS,       "inputs",       "process filter by input process name", "inputs=demo1p_5000_25" },
    { PAR_OUTPUTS,      "outputs",      "process filter by output table", "outputs=demo1outs" },
    { PAR_BITMAP,       "bitmap",       "include bitmap in stats output", "bitmap=true" },
    { PAR_COMMAND,      "command",      "process control command(resume|suspend|stop)", "command=stop" },
    { PAR_NONE,         NULL,           NULL }
};

const VTCli::VTCLI_EXAMPLE_DEF VTCli::m_examples[] = 
{
    { "Select intervals of process demo1p_5000_25 which last between 0.5s-3.5s",
        "./vtcli select interval process=demo1p_5000_25 duration=\"(0.5,3.5)\"" },
    { "Select intervals of process demo1p_5000_25 and video3 overlapping with 1.4.2015",
        "./vtcli select interval process=demo1p_5000_25 sequence=video3 timerange=\"(1427846400,1427932800)\"" },
    { "Select intervals of process demo2p_video3_demo1p_11_50 and video3 with event overlapping video region",
        "./vtcli select interval process=demo2p_video3_demo1p_11_50 sequence=video3 region=\"((0.1,0.31),(0.72,0.6))\"" },
    { NULL, NULL }
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
            bRet = statsCommand(tokens);
            break;
        case CMD_CONTROL:
            bRet = controlCommand(tokens);
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
        if (! pbStop) {
	    printError(string("failed to parse line: ").append(line));
	}
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

bool VTCli::parseUintValue(const string& word, unsigned int *value)
{
    char *endptr = NULL;
    *value = strtol(word.c_str(), &endptr, 10);
    
    return (endptr && !*endptr);
}

bool VTCli::parseUintRangeValue(const string& word, unsigned int *value1, unsigned int *value2)
{
    return (2 == sscanf(word.c_str(), "(%u,%u)", value1, value2)) && (*value1 <= *value2);
}

bool VTCli::parseFloatValue(const std::string& word, double *value)
{
    char *endptr = NULL;
    *value = strtod(word.c_str(), &endptr);

    return (endptr && !*endptr);
}

bool VTCli::parseFloatRangeValue(const string& word, double *value1, double *value2)
{
    return (2 == sscanf(word.c_str(), "(%lf,%lf)", value1, value2)) && (*value1 <= *value2);
}

bool VTCli::parseRegionValue(const std::string& word, IntervalEvent::box *value)
{
    return (4 == sscanf(word.c_str(), "((%lf,%lf),(%lf,%lf))",
        &value->low.x, &value->low.y, &value->high.x, &value->high.y));
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
            PVAL(duration); PVAL(timerange); PVAL(region);

            // find table with intervals (directly by 'outputs' or by 'process' outputs)
            Interval* in = NULL;
            if (PVAL_OK(outputs)) {
                in = new Interval(*(m_vtapi->commons), outputs);
            }
            else if (PVAL_OK(process)) {
                Process *p = new Process(*(m_vtapi->commons), process);
                if (p->next()) {
                    in = p->getOutputData();
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
            
            if (PVAL_OK(id))        { PVAL_PARSE_UINT(id); in->filterById(id_uint); }
            if (PVAL_OK(process))   in->filterByProcess(process);
            if (PVAL_OK(sequence))  in->filterBySequence(sequence);
            if (PVAL_OK(duration))  { PVAL_PARSE_FLOAT_RANGE(duration); in->filterByDuration(duration1_float, duration2_float); }
            if (PVAL_OK(timerange)) { PVAL_PARSE_UINT_RANGE(timerange); in->filterByTimeRange(timerange1_uint, timerange2_uint); }
            if (PVAL_OK(region))    { PVAL_PARSE_REGION(region); in->filterByRegion(region_region); }

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
            PVAL(location); PVAL(type); PVAL(realtime); PVAL(name);

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
                    bRet = loadVideo(ds, name,FPSNT(location), PVAL_OK(realtime) ? atol(realtime.c_str()) : 0);
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
                    in = p->getOutputData();
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
                Process *pi = new Process(*(m_vtapi->commons));
                if (pi) {
                    pi->filterByInputProcessName(name);
                    while (pi->next()) {
                        pi->clearOutputData();
                        Query qi(*(m_vtapi->commons),
                            "DELETE FROM " + pi->getDataset() + ".processes WHERE prsname = '" + pi->getName() + "';");
                        qi.execute();
                    }
                    delete pi;
                } 
                pr->clearOutputData();
                Query q(*(m_vtapi->commons),
                    "DELETE FROM " + pr->getDataset() + ".processes WHERE prsname = '" + name + "';");
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
            PVAL(sequence);
            PVAL(process);
            PVAL(bitmap);

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
            
            bool do_bitmap = PVAL_OK(bitmap) && (bitmap.compare("true") == 0);

            Video *vid = new Video(*(m_vtapi->commons), sequence);
            Process *pr = new Process(*(m_vtapi->commons), process);
            Interval *outs = NULL;
            unsigned char *bmap = NULL;
            
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
                bmap = (unsigned char *)malloc(lenAlloc);
                if (!bmap) {
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
                memset(bmap, 0, lenAlloc);
                while (outs->next()) {
                    int t1 = outs->getStartTime() - 1;
                    int t2 = outs->getEndTime() - 1;
                    int x1 = t1 >> 3;
                    int x2 = t2 >> 3;
                    int y1 = (x1 << 3) & t1;
                    int y2 = (x2 << 3) & t2;
                    int bits = t2 - t1 + 1;
                    
                    // first byte
                    if (y1 > 0 && bits) {
                        for (int y = 7 - y1; y >= 0 && bits; y--, bits--)
                            bmap[x1] |= (1 << y);
                        x1++;
                    }
                    
                    // last byte
                    if (y2 > 0 && bits) {
                        for (int y = 7; y >= y2 && bits; y--, bits--)
                            bmap[x2] |= (1 << y);
                        x2--;
                    }
                    
                    // stuff in between
                    if (bits) memset(bmap + x1, 0xFF, x2-x1);
                }
                
                // calculate total coverage
                size_t x = 0, y = 7, total = 0;;
                for (size_t i = 0; i < len; i++) {
                    if (bmap[x] & (1 << y)) total++;
                    if (y-- == 0) {
                        x++;
                        y = 7;
                    }
                }
                
                // print length,coverage,bitmap
                cout << "length,coverage";
                if (do_bitmap) cout << ",bitmap";
                cout << endl;
                cout << len << ',' << (double)total/len;
                if (do_bitmap) cout << ',' << base64_encode(bmap, lenAlloc);
                cout << endl;
                
            } while(0);
            
            if (bmap) free(bmap);
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

bool VTCli::controlCommand(VTCLI_KEYVALUE_LIST& params)
{
    bool bRet = true;

    do {
        if (params.empty()) {
            printError("what object for control? type 'help' for list");
            bRet = false;
            break;
        }

        string param = params.front().second;
        params.pop_front();
        switch (getObject(param))
        {
        case OBJ_PROCESS:
        {
            PVAL(name);
            PVAL(command);

            if (!PVAL_OK(name)) {
                printError("process name not specified(name=<processname>)");
                bRet = false;
                break;
            }
            if (!PVAL_OK(command)) {
                printError("command not specified(command=resume|suspend|stop)");
                bRet = false;
                break;
            }

            Process *pr = new Process(*(m_vtapi->commons), name);
            ProcessControl *pc = NULL; 
            
            do {
                // check process for existence
                if (!pr->next()) {
                    printError(string("process doesn't exist: ").append(name).c_str());
                    bRet = false;
                    break;
                }

                // get command
                ProcessControl::COMMAND_T cmd = ProcessControl::toCommandValue(command);
                if (cmd == ProcessControl::COMMAND_NONE) {
                    printError(string("invalid command: ").append(command).c_str());
                    bRet = false;
                    break;
                }
                
                // get process control object
                pc = pr->getProcessControl();
                if (!pc) {
                    printError(string("failed to get process control: ").append(name).c_str());
                    bRet = false;
                    break;
                }

                // connect to server (process instance)
                bRet = pc->client();
                if (!bRet) {
                    printError(string("failed to connect to process: ").append(name).c_str());
                    break;
                }
                
                // try sending control command
                bRet = pc->control(cmd);
                if (!bRet) {
                    printError(string("failed to send command to process: ").append(name).c_str());
                    break;
                }

            } while (0);

            if (pc) delete pc;
            if (pr) delete pr;

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
#define SECTION_HEADER(h) \
    "---------------------------- " h " ----------------------------\n\n"
    
    static const char *usage_text = 
        SECTION_HEADER("VTCLI HELP")
        "VTCli - VTApi command line interface\n"
        "Using " CMDLINE_PARSER_PACKAGE_NAME " " CMDLINE_PARSER_VERSION "\n\n"
        "Usage:\n"
        "   ./vtcli [VTAPI_OPTIONS] COMMAND [OBJECT [PARAMS]]\n\n"
        "Interactive mode:\n"
        "   ./vtcli [VTAPI_OPTIONS]\n"
        "       > COMMAND [OBJECT [PARAMS]]\n"
        "       > ...\n"
        "       > exit\n";
    static const char *options_text =
        SECTION_HEADER("VTAPI OPTIONS")
        "These options modify VTApi behaviour, along with vtapi.conf\n";
    static const char *commands_text =
        SECTION_HEADER("COMMANDS")
        "VTCli supports these commands, some are valid only for certain OBJECTS\n";
    static const char *objects_text =
        SECTION_HEADER("OBJECTS")
        "Objects to select/insert/delete may be specified by PARAMS (format: PARAM=value)\n"
        "No PARAMS usually means command works on ALL objects (careful with delete)\n";
    static const char *params_text =
        SECTION_HEADER("PARAMS")
        "PARAMS are used either for specifying object parameters for INSERT\n"
        "or for SELECT filters. All of them are PARAM=VALUE pairs\n"
        "VALUE formatsmay be escaped (\"VALUE\"), deduce them from examples\n";
    static const char *examples_text =
        SECTION_HEADER("EXAMPLES")
        "A few chosen VTCli examples:\n";
    
    // usage
    cout << usage_text << endl;
    
    // vtapi options
    cout << options_text << endl;
    for (size_t i = 0; gengetopt_args_info_help[i]; i++) {
        cout << gengetopt_args_info_help[i] << endl;
    }
    cout << endl;
    
    //commands
    cout << commands_text << endl;
    for (size_t i = 0; m_cmd[i].name; i++) {
        if (i > 0) cout << ',';
        cout << m_cmd[i].name;
    }
    cout << endl << endl;

    // commands details
    for (size_t i = 0; m_cmd[i].name; i++) {
        if (i > 0) cout << endl;
        cout << i + 1 << ". " << m_cmd[i].name << endl <<
            "  * " << m_cmd[i].desc << endl;
        if (m_cmd[i].objects != OBJ_NONE) {
            cout << "  * OBJECTS: " << getObjectsString(m_cmd[i].objects) << endl;
        }
        if (m_cmd[i].example) {
            cout << "  * ex. " << m_cmd[i].example << endl;
        }
    }
    cout << endl;

    // objects
    cout << objects_text << endl;
    for (size_t i = 0; m_obj[i].name; i++) {
        if (i > 0) cout << ',';
        cout << m_obj[i].name;
    }
    cout << endl << endl;
    
    // objects details
    for (size_t i = 0; m_obj[i].name; i++) {
        if (i > 0) cout << endl;
        cout << i + 1 << ". " << m_obj[i].name << endl;
        if (m_obj[i].params != PAR_NONE) {
            cout << "  * PARAMS: " << getParamsString(m_obj[i].params) << endl;
        }
    }
    cout << endl;

    // params
    cout << params_text << endl;
    for (size_t i = 0; m_par[i].name; i++) {
        if (i > 0) cout << ",";
        cout << m_par[i].name;
    }
    cout << endl << endl;
    
    // params details
    for (size_t i = 0; m_par[i].name; i++) {
        if (i > 0) cout << endl;
        cout << i + 1 << ". " << m_par[i].name << endl <<
        "  * " << m_par[i].desc << endl;
        if (m_par[i].example) {
            cout << "  * ex. " << m_par[i].example << endl;
        }
    }
    cout << endl;

    // examples
    cout << examples_text << endl;
    for (size_t i = 0; m_examples[i].desc; i++) {
        if (i > 0) cout << endl;
        cout << i + 1 << ". " << m_examples[i].desc << endl <<
        '\t' << m_examples[i].command << endl;
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
        delete vid;
        
    } while(0);
    
    if (dir) closedir(dir);
    
    return bRet;
}

bool VTCli::loadVideo(Dataset *ds, const std::string &name, const std::string &filepath, const time_t& realtime)
{
    bool bRet = true;
    
    Video *vid = ds->newVideo();
    if (vid->add(name.empty() ? getFileNameNoExt(filepath) : name, filepath, realtime)) {
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

    if (im) delete im;
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
