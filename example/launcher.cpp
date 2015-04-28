/*
 * Demo VTApi pro launcher modulu
 * by: Vojtech Froml (ifroml[at]fit.vutbr.cz)
 * 
 * Ukazky:
 *
 * 1. Spusteni procesu urciteho modulu (asynchronne)
 *    - proces pro demo1 modul; vstupy = dataset videi, 2 nahodne ciselne parametry.
 *    - asynchronne, otevre komunikaci s instanci procesu
 *
 * 2. Spusteni procesu urciteho modulu (synchronne, retezeny)
 *    - proces pro demo2 modul; vstupy = vysledky z predchoziho procesu, nazev videa
 *    - synchronne, neotevira komunikaci, pouze ceka na jeho skonceni
 *    - retezeny, pouziva vysledky procesu z ukazky 1 pro video "video3.mpg"
 *
 * 3. Vypis vysledku procesu
 *
 * 4. Ziskani parametry modulu a jejich typu
 *
 * 5. Nalezeni jiz existujiciho procesu dle jeho parametru
 * 
 * 6. Ziska hodnoty parametru pro jeden proces
 * 
 * 
 * [CHANGE] oznacuje zmeny z commitu 29.4.2015
*/


///////////////////////////////////////////////////////////
// Implementace
///////////////////////////////////////////////////////////

#include <cstdio>
#include <unistd.h> // sleep
#include <vtapi.h>

using namespace vtapi;
using namespace std;

class CDemo1Module;
class CDemo2Module;

int main(int argc, char *argv[]);
static void Demo1Callback(const ProcessState& state, void *context);


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
        cout << "starting process of mod_demo1 (asynchronous, suspended)" << endl;

        // timto zpusobem se vytvari novy proces
        // [CHANGE] odebrany argumenty
        if (!m_process) m_process = this->addProcess();

        // [CHANGE] asynchronni volani, vytvoreni pozastaveneho procesu
        if (m_process->run(true, true)) {
            cout << m_process->getName() << " started" << endl;
            
            // [CHANGE] inicializujeme objekt pro komunikaci s procesem
            if (!m_pctrl) m_pctrl = m_process->getProcessControl();
            if (m_pctrl->client(2500, Demo1Callback, this)) {
                
                // odpauzujeme proces
                if (m_process->controlResume(m_pctrl)) {
                    cout << m_process->getName() << " unpaused" << endl;
                }
                else {
                    cerr << "failed to unpause process" << endl;
                }
            }
            else {
                cerr << "failed to connect to server process" << endl;
            }
        }
        
        // zde dalsi mozne operace nad skoncenym procesem
        
        // cekani na koncovou notifikaci finished/error
        // ...
        sleep(2);
        
    }
    
    // Callback volany z procesu behem funkce run(), pozor - jine vlakno
    void ProcessCallback(const ProcessState& state)
    {
        switch (state.status)
        {
            // proces reportuje progres
            case ProcessState::STATUS_RUNNING:
            {
                cout << m_process->getName() << " progress: " << state.progress << "%" << endl;
                break;
            }
            // proces byl pozastaven
            case ProcessState::STATUS_SUSPENDED:
            {
                cout << m_process->getName() << " has been suspended" << endl;
                break;
            }
            // proces skoncil uspesne
            case ProcessState::STATUS_FINISHED:
            {
                cout << m_process->getName() << " finished succesfully" << endl;
                break;
            }
            // proces skoncil s chybou
            case ProcessState::STATUS_ERROR:
            {
                cerr << m_process->getName() << " finished with ERROR: " << state.lastError << endl;
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
        next();
    };
    virtual ~CDemo2Module()
    {
        if (m_process) delete m_process;
    }
    
    void SetParams(const string& inputID, const string& videoName)
    {
        if (!m_process) m_process = this->addProcess();

        m_process->setInputs(inputID);
        m_process->setParamString("video", videoName);
    }
    
    void Run(const string& inputID, const string& videoName)
    {
        SetParams(inputID, videoName);
        Run();
    }
    
    void Run()
    {
        cout << "starting process of mod_demo2 (synchronous)" << endl;

        if (!m_process) m_process = this->addProcess();

        if (m_process->run()) {
            cout << m_process->getName() << " started" << endl;

            // UKAZKA 3
            // vypis vystupu procesu
            
            cout << "\nEXAMPLE 3: Printing process outputs" << endl;
            cout << "-------------------------------------------------" << endl;

            Interval *outputs = m_process->getOutputData();
            while (outputs->next()) {
                IntervalEvent *event = outputs->getIntervalEvent("event");
                if (event) {
                    cout << "event: " << toString(*event) << endl;
                    delete event;
                }
                else {
                    cerr << "failed to get event" << endl;
                }
            }
            delete outputs; 
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


///////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    // zvolime nejake parametry procesu
    srand(time(NULL));
    int param1 = rand() % 1000;
    double param2 = (rand() % 1000) / 1000.0;
    string video = "video3";
    
    // VTApi instance
    VTApi *vtapi = new VTApi(argc, argv);
    
    // UKAZKA 1 + 2
    // priklad retezeni procesu 2 demo modulu, viz popis v hlavicce souboru
    try
    {
        cout << endl << "EXAMPLE 1: Starting demo1 process" << endl;
        cout << "-------------------------------------------------" << endl;
        
        CDemo1Module demo1(vtapi);
        demo1.SetParams(param1, param2);
        demo1.Run();

        cout << endl << "EXAMPLE 2: Starting demo2 process" << endl;
        cout << "-------------------------------------------------" << endl;

        CDemo2Module demo2(vtapi);
        demo2.SetParams(demo1.m_process->getName(), video);
        demo2.Run();
    }
    catch(exception e)
    {
        cerr << e.what() << endl;
    }

    // UKAZKA 4
    // pristup k typum parametru modulu
    try
{
        cout << endl << "EXAMPLE 4: Accessing module parameters" << endl;
        cout << "-------------------------------------------------" << endl;

        CDemo1Module demo1(vtapi);
        TKeys keys = demo1.getMethodKeys();
        
        cout << demo1.getName() << " parameters:" << endl;
        for (size_t i = 0; i < keys.size(); i++) {
            cout << keys[i].from << ": " << keys[i].type << ' ' << keys[i].key << endl;
        }
        
    }
    catch(exception e)
    {
        cerr << e.what() << endl;
    }
    
    // UKAZKA 5 + 6
    // a) nalezneme proces s urcitymi parametry
    // b) zjistime, jestli existuje
    // c) vypiseme zpet jeho parametry
    // slozity priklad:
    //      chceme najit vysledky procesu z demo2 pro video3
    //      spocitane nad mezivysledky z demo1 s danymi hodnotami parametru
    try
{
        cout << endl << "EXAMPLE 5: Finding process by its parameters" << endl;
        cout << "-------------------------------------------------" << endl;

        // a) k sestaveni ID hledaneho procesu potrebujeme:

        // ID vstupniho procesu
        CDemo1Module demo1(vtapi);
        Process *p1 = demo1.newProcess();
        p1->setParamInt("param1", param1);      // nastavime parametry vstupniho procesu
        p1->setParamDouble("param2", param2);
        string inputID = p1->constructName();   // ID vstupniho procesu
        delete p1;

        // ID vystupniho procesu
        CDemo2Module demo2(vtapi);
        Process *p2 = demo2.newProcess();
        p2->setParamString("video", video);     // hledame vysledky pro video3
        p2->setInputs(inputID);                 // nad mezivysledky z procesu demo1
        string outputID = p2->constructName();  // ID vstupniho procesu
        delete p2;

        // b) nalezneme proces
        
        Process *p = demo2.newProcess(outputID);
        if (p->next()) {
            cout << "found process: " << outputID << endl;
        }
        else {
            cerr << "failed to find process: " << outputID << endl;
        }
        delete p;
        
        // c) vypsani parametru
        
        cout << endl << "EXAMPLE 6: Getting process parameters:" << endl;
        cout << "-------------------------------------------------" << endl;

        p1 = demo1.newProcess(inputID);
        if (p1->next()) {
            cout << "process " << inputID << " parameters:" << endl
                << "param1: " << p->getParamInt("param1") << endl
                << "param2: " << p->getParamDouble("param2") << endl;
        }
        delete p1;
    }
    catch(exception e)
    {
        cerr << e.what() << endl;
    }

    // cleanup
    delete vtapi;
    
    return 0;
}
