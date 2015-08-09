/**
 * @file
 * @brief   Declaration of main %VTApi class which provides a basic functionality of %VTApi.
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "data/dataset.h"
#include "data/method.h"
#include "data/sequence.h"
#include "data/task.h"
#include "data/process.h"


/**
 * @brief Own namespace of %VTApi library
 */
namespace vtapi {

/**
 * @brief VTApi class is main entry class for managing %VTApi objects
 *
 */
class VTApi
{
public:

    /**
     * Constructor from program arguments
     * @param argc   argument count (as in usual program)
     * @param argv   argument vector (as in usual program)
     */
    VTApi(int argc, char** argv);

    /**
     * Constructor from config file
     * @param configFile   location of configuration file
     */
    explicit VTApi(const std::string& configFile);

    /**
     * Copy constructor
     * @param orig
     */
    explicit VTApi(const VTApi& orig);
    
    /**
     * Destructor
     */
    ~VTApi();

    /**
     * Creates new dataset + appropriate DB objects and returns its object for iteration
     * @param name new dataset name (simple, unique)
     * @param location new dataset location
     * @param friendly_name user readable dataset name
     * @param description optional description
     * @return dataset object, NULL on error
     */
    Dataset* createDataset(
        const std::string& name,
        const std::string& location,
        const std::string& friendly_name,
        const std::string& description = std::string());
    
    /**
     * Creates new method + appropriate DB objects and returns its object for iteration
     * @param name new method name
     * @param keys_definition definiton of method's keys (DB inputs, outputs)
     * @param params_definition definiton of method's params (user inputs)
     * @param description optional description
     * @return method object, NULL on error
     */
    Method* createMethod(
        const std::string& name,
        const MethodKeys keys_definition,
        const MethodParams params_definition,
        const std::string& description = std::string());
    
    /**
     * Loads dataset(s) for iteration
     * @param name dataset name (empty => all datasets)
     * @return dataset object
     */
    Dataset* loadDatasets(const std::string& name = std::string());

    /**
     * Loads method(s) for iteration
     * @param name method name (empty => all methods)
     * @return method object
     */
    Method* loadMethods(const std::string& name = std::string());

    /**
     * Loads default dataset's sequences for iteration
     * WARNING: Default dataset must be configured
     * @param name   sequence name (no name = all sequences)
     * @return pointer to the new Sequence object
     */
    Sequence* loadSequences(const std::string& name = std::string());

    /**
     * Loads default dataset's videos (= sequences) for iteration
     * WARNING: Default dataset must be configured
     * @param name   video (sequence) name (no name = all sequences)
     * @return pointer to the new Video object
     */
    Video* loadVideos(const std::string& name = std::string());

    /**
     * Loads default dataset's image folders (= sequences) for iteration
     * WARNING: Default dataset must be configured
     * @param name image folder name (no name = all image folders)
     * @return pointer to the new ImageFolder object
     */
    ImageFolder* loadImageFolders(const std::string& name = std::string());

    /**
     * Loads default dataset's processing tasks for iteration
     * WARNING: Default dataset must be configured
     * @param name task name (no name = all tasks)
     * @return pointer to the new Task object
     */
    Task* loadTasks(const std::string& name = std::string());

    /**
     * Loads default dataset's processes for iteration
     * WARNING: Default dataset must be configured
     * @param id   process ID (0 = all processes)
     * @return pointer to the new Process object
     */
    Process* loadProcesses(int id = 0);

private:

    Commons* _pcommons;      /**< Commons are common objects to all vtapi objects */

    VTApi() = delete;             /**< forbidden */
    VTApi& operator=(const VTApi& orig) = delete; /**< forbidden */
};

} // namespace vtapi
