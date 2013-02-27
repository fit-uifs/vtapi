
// VTApi
#include "vtapi.h"



class Test : public Method {
public:
    TKeyValues params;

    /**
     * The default constructor, not necessary to edit
     * @return
     */
    Test(const KeyValues& orig, const String& name = "Test") : Method(orig, name) {
        // TODO Vojta: naplnit MethodKeys if not existent (create)
    };

    /**
     * The run function - the first line makes it work, do not edit, please
     * @return
     */
    bool run(const Process& process /** some other params may follow **/, const String& sometext) {
        this->processParams(process, sometext); // Do not edit, please!

        // Your code follows here...
        return true;
    };


    // --- Please, do not edit below this line. ----------------------------------
    // TKeyValue<String> params;
    bool processParams(const Process& process, const String& sometext) { // ...
        // = "test"; // this is the default value for any parameter dataset
        return true;
    };
};






/**
 * The main function - the first line makes it work, do not edit, please.
 */
int main(int argc, char** argv) {
    VTApi vtapi(argc, argv); // Do not edit, please!
    // Process test1 = vtapi.newProcess("Test","test1"); // there are some alternatives if already registered in the DB

    Test test(*vtapi.newDataset("test"));    // init and your method if new (not in the DB)
    Process test1 = *test.newProcess("test1");  // register the process

    // FIXME: koukni na run
    test1.run();  // run the process with inherited and custom parameters
    // test1.run(runTest, "sometext");  // run the process with custom parameters

    return 0;
}
