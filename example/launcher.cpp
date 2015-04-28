/*
 * Demo VTApi pro launcher modulu
 * by: Vojtech Froml (ifroml[at]fit.vutbr.cz)
 * 
 * 1. Spusti demo1 modul nad celym datasetem videi s 2 nahodnymi ciselnymi parametry.
 * 2. Nad vysledky demo1 pro video "video3.mpg" spusti demo2 modul
 * 3. Vypise vysledek modulu demo2
 *
 * 4. Ziska parametry modulu a jejich typy
 * 5. Ziska hodnoty parametru pro jeden proces
 * 6. Nalezne vysledky pro urcity modul s urcitymi parametry a vypise je
 * 
 * 
 * Oba moduly jsou pro synchronni beh jedne instance modulu.
 * U prvniho modulu jsou komentare, co je treba vzdy ve tride zmenit/doimplementovat.
 *
 * [CHANGE] oznacuje zmeny z commitu 29.4.2015
*/

// format process ID
#define GET_PROCESS_ID(id,m,p1,p2) \
    { std::stringstream ss; ss << m.getName() << "p_" << p1 << "_" << p2; id = ss.str(); }


///////////////////////////////////////////////////////////
// Implementace
///////////////////////////////////////////////////////////

#include <cstdio>
#include <vtapi.h>

using namespace vtapi;

class CDemo1Module;
class CDemo2Module;

int main(int argc, char *argv[]);
static void Demo1Callback(const ProcessState& state, void *context);
static void Demo2Callback(const ProcessState& state, void *context);


///////////////////////////////////////////////////////////
// Trida demo1 modulu
// - dedi ze tridy Method VTApi
///////////////////////////////////////////////////////////
class CDemo1Module : public Method
{
public:
    Process *m_process;         // instance beziciho procesu
    ProcessControl *m_pctrl;    // [CHANGE] ovladani procesu a prijimani notifikaci o zmene stavu
    
public:
    // v konstruktoru treba upravit nazev modulu ("demo1")
    CDemo1Module(VTApi *vtapi) : Method(*vtapi->commons, "demo1")
    {
        m_process = NULL;
        m_pctrl = NULL;
        next();
    };
    virtual ~CDemo1Module()
    {
        if (m_pctrl) delete m_pctrl;
        if (m_process) delete m_process;
    };
    
    // Nejakou funkci SetParams() je treba implementovat vzdy, kdyz chceme modul
    // spoustet s nedefaultnimi parametry
    void SetParams(int param1, double param2)
    {
        // timto zpusobem se vytvari novy proces
        // [CHANGE] odebrany argumenty
        if (!m_process) m_process = this->addProcess();
        
        // k dispozici zatim setParamInt, setParamDouble, setParamString, setParamInputs
        m_process->setParamInt("param1", param1);
        m_process->setParamDouble("param2", param2);
    }

    // Kombinace SetParams() a Run(), pouze kosmeticky ucel
    void Run(int param1, double param2)
    {
        SetParams(param1, param2);
        Run();
    }
    
    // Funkce pro synchronni beh modulu
    void Run()
    {
        printf("starting process of mod_demo1 (asynchronous, suspended)\n");

        // timto zpusobem se vytvari novy proces
        // [CHANGE] odebrany argumenty
        if (!m_process) m_process = this->addProcess();

        // [CHANGE] asynchronni volani, vytvoreni pozastaveneho procesu
        if (m_process->run(true, true)) {
            printf("%s started\n", getOutputID().c_str());
            
            // [CHANGE] inicializujeme objekt pro komunikaci s procesem
            if (!m_pctrl) m_pctrl = m_process->getProcessControl();
            if (m_pctrl->client(2500, Demo1Callback, this)) {
                
                // odpauzujeme proces
                if (m_process->controlResume(m_pctrl)) {
                    printf("%s unpaused\n", getOutputID().c_str());
                }
                else {
                    fprintf(stderr, "failed to unpause process\n");
                }
            }
            else {
                fprintf(stderr, "failed to connect to server process\n");
            }
        }
        
        // zde dalsi mozne operace nad skoncenym procesem
        
        // cekani na koncovou notifikaci finished/error
        // ...
    }
        
    // Pomocna funkce pro ziskani ID vystupnich dat, pouze kosmeticky ucel
    std::string getOutputID()
    {
        return m_process->getName();
    }
    
    // Callback volany z procesu behem funkce run(), pozor - jine vlakno
    void ProcessCallback(const ProcessState& state)
    {
        switch (state.status)
        {
            // proces reportuje progres
            case ProcessState::STATUS_RUNNING:
            {
                printf("%s progress: %f%%\n", getOutputID().c_str(), state.progress);
                break;
            }
            // proces byl pozastaven
            case ProcessState::STATUS_SUSPENDED:
            {
                printf("%s has been suspended\n", getOutputID().c_str());
                break;
            }
            // proces skoncil uspesne
            case ProcessState::STATUS_FINISHED:
            {
                printf("%s finished succesfully\n", getOutputID().c_str());
                break;
            }
            // proces skoncil s chybou
            case ProcessState::STATUS_ERROR:
            {
                fprintf(stderr, "%s finished with ERROR\n", getOutputID().c_str());
                break;
            }
        }
    }
};

///////////////////////////////////////////////////////////
// Trida demo2 modulu
// - analogicky k demo1
///////////////////////////////////////////////////////////
class CDemo2Module : public Method
{
public:
    Process *m_process;
    ProcessControl *m_pctrl;
    
public:
    CDemo2Module(VTApi *vtapi) : Method(*vtapi->commons, "demo2") {
        m_process = NULL;
        m_pctrl = NULL;
        next();
    };
    virtual ~CDemo2Module()
    {
        if (m_pctrl) delete m_pctrl;
        if (m_process) delete m_process;
    }
    
    void SetParams(const std::string& inputID, const std::string& videoName)
    {
        if (!m_process) m_process = this->addProcess();

        if (!m_pctrl) m_pctrl = m_process->getProcessControl();

        m_process->setInputs(inputID);
        m_process->setParamString("video", videoName);
    }
    
    void Run(const std::string& inputID, const std::string& videoName)
    {
        SetParams(inputID, videoName);
        Run();
    }
    
    void Run()
    {
        printf("starting process of mod_demo2 (synchronous)\n");

        if (!m_process) m_process = this->addProcess();

        if (m_process->run()) {
            printf("%s started\n", m_process->getName().c_str());

            if (!m_pctrl) m_pctrl = m_process->getProcessControl();
            if (m_pctrl->client(2500, Demo1Callback, this)) {
                Interval *outputs = m_process->getOutputData();
                printf("\nprocess %s new outputs:\n", m_process->getName().c_str());
                while (outputs->next()) {
                    IntervalEvent *event = outputs->getIntervalEvent("event");
                    if (event) {
                        printf("event: %s\n", toString(*event).c_str());
                        delete event;
                    }
                    else {
                        printf("failed to get event\n");
                    }
                }
                delete outputs; 
            }
            else {
                fprintf(stderr, "failed to connect to server process\n");
            }
        }
    }

    void ProcessCallback(const ProcessState& state)
    {
        switch (state.status)
        {
            case ProcessState::STATUS_RUNNING:
            {
                printf("%s progress: %f%%\n", m_process->getName().c_str(), state.progress);
                break;
            }
            case ProcessState::STATUS_SUSPENDED:
            {
                printf("%s has been suspended\n", m_process->getName().c_str());
                break;
            }
            case ProcessState::STATUS_FINISHED:
            {
                printf("%s finished succesfully\n", m_process->getName().c_str());
                break;
            }
            case ProcessState::STATUS_ERROR:
            {
                fprintf(stderr, "%s finished with ERROR\n", m_process->getName().c_str());
                break;
            }
        }
    }
};

///////////////////////////////////////////////////////////
// CALLBACKY pro update statusu procesu
///////////////////////////////////////////////////////////
// [CHANGE]
// Callback pro spusteny proces
// - state      = stav procesu
// - context    = kontext zadany pri spousteni (typicky napr. this)

void Demo1Callback(const ProcessState& state, void *context)
{
    return ((CDemo1Module *)context)->ProcessCallback(state);
}
void Demo2Callback(const ProcessState& state, void *context)
{
    return ((CDemo2Module *)context)->ProcessCallback(state);
}


///////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    // zvolime nejake parametry procesu
    srand(time(NULL));
    int param1 = rand() % 1000;
    double param2 = 10;
    std::string video = "video3";
    
    // VTApi instance
    VTApi *vtapi = new VTApi(argc, argv);
    
    // priklad retezeni 2 demo modulu, viz popis v hlavicce souboru
    try
    {
        printf("Starting demo1 and demo2 modules example...\n");
        
        CDemo1Module demo1(vtapi);
        demo1.SetParams(param1, param2);
        demo1.Run();

        CDemo2Module demo2(vtapi);
        demo2.SetParams(demo1.getOutputID(), video);
        demo2.Run();
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    // pristup k typum parametru modulu
    try
    {
        CDemo1Module demo1(vtapi);
        TKeys keys = demo1.getMethodKeys();
        
        printf("\nmodule %s parameters:\n", demo1.getName().c_str());
        for (size_t i = 0; i < keys.size(); i++) {
            printf("%s: %s %s \n",
                keys[i].from.c_str(), keys[i].type.c_str(), keys[i].key.c_str());
        }
        
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    // pristup k hodnotam parametru procesu
    try
    {
        CDemo1Module demo1(vtapi);

        // proces identifikovany output ID
        std::string processID;
        GET_PROCESS_ID(processID, demo1, param1, param2);
        
        Process *p = demo1.newProcess(processID);
        p->next();

        printf("\nprocess %s parameter values:\nparam1 = %d\nparam2 = %.2f\n",
            p->getName().c_str(),
            p->getParamInt("param1"),
            p->getParamDouble("param2"));
        
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    // pristup k vysledkum pro urcite parametry nejakeho modulu
    // priklad: chceme najit vysledky demo2 pro video3
    //          spocitane nad mezivysledky z demo1 s danymi hodnotami parametru
    try
    {
        bool found = false;
        CDemo1Module demo1(vtapi);
        
        // najdeme proces pro demo1 s 1. parametrem 11
        // iteracni pristup
        Process *p1 = demo1.newProcess();
        while(p1->next()) {
            if (p1->getParamInt("param1") == param1) {
                found = true;
                break;
            }
        }
        
        // ALTERNATIVA - slozit si process ID rucne
        //std::string processID;
        //GET_PROCESS_ID(processID, demo1, param1, param2);
        //Process *p1_alt = demo1.newProcess(processID);
        //if (p1_alt->next()) found = true;
        
        if (found) {
            found = false;
            CDemo2Module demo2(vtapi);
            
            // najdeme proces, ktery ma jako vstup vystupy p1 a parametr video3.mpg
            Process *p2 = demo2.newProcess();
            // optimalizacni trik, at neiterujeme pres vsechny procesy
            p2->filterByInputs(p1->getName());
            while (p2->next()) {
                if (p2->getParamString("video").compare(video) == 0) {
                    found = true;
                    break;
                }
            }

            if (found) {
                // vypiseme vysledky pro nalezeny proces
                Interval *outputs = p2->getOutputData();
                printf("\nprocess %s previously calculated outputs:\n", p2->getName().c_str());
                while(outputs->next())
                {
                    IntervalEvent *event = outputs->getIntervalEvent("event");
                    if (event) {
                        printf("event: %s\n", toString(*event).c_str());
                        delete event;
                    }
                    else {
                        printf("failed to get event\n");
                    }
                }
                delete outputs;
            }
            delete p2;
        }
        delete p1;
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    // cleanup
    delete vtapi;
    
    return 0;
}
