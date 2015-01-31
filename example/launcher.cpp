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
*/

#define XSTR(s) STR(s)
#define STR(s) #s

// nahodne parametry spoustenych procesu
#define RAND_PARAM1      (rand() % 1000)
#define RAND_PARAM2      (rand() % 1000)
#define DEF_VIDEO       "video3"

// format process ID
#define GET_PROCESS_ID(id,m,p1,p2) \
    { std::stringstream ss; ss << m.getName() << "p_" << p1 << "_" << p2; id == ss.str(); }


///////////////////////////////////////////////////////////
// Implementace
///////////////////////////////////////////////////////////

#include <cstdio>
#include <vtapi.h>

using namespace vtapi;

class CDemo1Module;
class CDemo2Module;

int main();
static void Demo1Callback(Process::STATE_T state, Process *process, void *context);
static void Demo2Callback(Process::STATE_T state, Process *process, void *context);


///////////////////////////////////////////////////////////
// Trida demo1 modulu
// - dedi ze tridy Method VTApi
///////////////////////////////////////////////////////////
class CDemo1Module : public Method
{
public:
    Process *m_process;     // instance beziciho procesu
    
public:
    // v konstruktoru treba upravit nazev modulu ("demo1")
    CDemo1Module(VTApi *vtapi) : Method(*vtapi->commons, "demo1")
    {
        m_process = NULL;
        next();
    };
    virtual ~CDemo1Module()
    {
        if (m_process) delete m_process;
    };
    
    // Nejakou funkci SetParams() je treba implementovat vzdy, kdyz chceme modul
    // spoustet s nedefaultnimi parametry
    void SetParams(int param1, int param2)
    {
        // timto zpusobem se vytvari novy proces
        if (!m_process) m_process = this->addProcess(Demo1Callback, (void *)this);

        // k dispozici zatim setParamInt, setParamDouble, setParamString, setParamInputs
        m_process->setParamInt("param1", param1);
        m_process->setParamInt("param2", param2);
    }

    // Kombinace SetParams() a Run(), pouze kosmeticky ucel
    void Run(int param1, int param2)
    {
        SetParams(param1, param2);
        Run();
    }
    
    // Funkce pro synchronni beh modulu
    void Run()
    {
        // timto zpusobem se vytvari novy proces
        // callback + context mohou byt NULL
        if (!m_process) m_process = this->addProcess(Demo1Callback, (void *)this);

        printf("\nrunning process...\n");
        m_process->run();
        
        // zde dalsi mozne operace nad skoncenym procesem
    }
        
    // Pomocna funkce pro ziskani ID vystupnich dat, pouze kosmeticky ucel
    std::string getOutputID()
    {
        return m_process->getName();
    }
    
    // Callback volany z procesu behem funkce run()
    void ProcessCallback(Process::STATE_T state)
    {
        switch (state)
        {
            case Process::STATE_STARTED:
            {
                printf("%s has been started\n", m_process->getName().c_str());
                break;
            }
            case Process::STATE_RUNNING:
            {
                // proces muze mit castecne vysledky
                printf("%s is running\n", m_process->getName().c_str());
                break;
            }
            case Process::STATE_DONE:
            {
                printf("%s finished\n", m_process->getName().c_str());
                break;
            }
            case Process::STATE_ERROR:
            {
                fprintf(stderr, "Error occurred in %s\n", m_process->getName().c_str());
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
    
public:
    CDemo2Module(VTApi *vtapi) : Method(*vtapi->commons, "demo2") {
        m_process = NULL;
        next();
    };
    virtual ~CDemo2Module()
    {
        if (m_process) delete m_process;
    }
    
    void SetParams(const std::string& inputID, const std::string& videoName)
    {
        if (!m_process) m_process = this->addProcess(Demo2Callback, this);

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
        printf("\nrunning process...\n");
        m_process->run();
        
        Interval *outputs = m_process->getOutputData();
        printf("\nprocess %s new outputs:\n", m_process->getName().c_str());
        while(outputs->next())
        {
            printf("event %s: %d\n",
                outputs->getString("event_name").c_str(),
                outputs->getInt("event_arg"));
        }
        delete outputs;
    }
    
    void ProcessCallback(Process::STATE_T state)
    {
        switch (state)
        {
            case Process::STATE_STARTED:
            {
                printf("%s has been started\n", m_process->getName().c_str());
                break;
            }
            case Process::STATE_RUNNING:
            {
                // proces muze mit castecne vysledky
                printf("%s is running\n", m_process->getName().c_str());
                break;
            }
            case Process::STATE_DONE:
            {
                printf("%s finished\n", m_process->getName().c_str());
                break;
            }
            case Process::STATE_ERROR:
            {
                fprintf(stderr, "Error occurred in %s\n", m_process->getName().c_str());
                break;
            }
        }
    }
};

///////////////////////////////////////////////////////////
// CALLBACKY pro update statusu procesu
///////////////////////////////////////////////////////////
// Callback pro spusteny proces
// - state      = stav procesu
// - process    = objekt pro dotazovani vysledku apod. (netreba pro 1-proces modul)
// - context    = kontext zadany pri spousteni (typicky napr. this)

void Demo1Callback(Process::STATE_T state, Process *process, void *context)
{
    (process);  // unused
    return ((CDemo1Module *)context)->ProcessCallback(state);
}
void Demo2Callback(Process::STATE_T state, Process *process, void *context)
{
    (process);  // unused
    return ((CDemo2Module *)context)->ProcessCallback(state);
}


///////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////
int main()
{
    // load random process parameters
    srand(time(NULL));
    int param1 = RAND_PARAM1;
    int param2 = RAND_PARAM2;
    std::string video = DEF_VIDEO;
    
    // VTApi instance
    // - tento konstruktor pouzije konfiguraci z ./vtapi.conf
    VTApi *vtapi = new VTApi();
    
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
            printf("%s %s\n", keys[i].type.c_str(), keys[i].key.c_str());
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

        printf("\nprocess %s parameter values:\nparam1 = %d\nparam2 = %d\n",
            p->getName().c_str(),
            p->getParamInt("param1"),
            p->getParamInt("param2"));
        
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
        
        // najdeme proces pro demo1 s parametry 11 a 50
        // iteracni pristup
        Process *p1 = demo1.newProcess();
        while(p1->next()) {
            if (p1->getParamInt("param1") == param1 &&
                p1->getParamInt("param2") == param2) {
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
                    printf("event %s: %d\n",
                        outputs->getString("event_name").c_str(),
                        outputs->getInt("event_arg"));
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
