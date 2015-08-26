/**
 * @file
 * @brief   Declaration of Dataset class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <string>
#include <list>
#include "keyvalues.h"
#include "sequence.h"
#include "task.h"
#include "taskparams.h"
#include "process.h"

namespace vtapi {

class Sequence;
class Video;
class ImageFolder;
class Task;
class TaskParams;
class Process;


/**
 * @brief This class should always be on the path of your program...
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Dataset : protected KeyValues
{
public:

    /**
     * @brief Copy constructor
     * @param copy original object
     */
    Dataset(const Dataset& copy);

    /**
     * Construct dataset object for iterating through VTApi datasets
     * If a specific name is set, object will represent one dataset only
     * @param commons base Commons object
     * @param name dataset name, empty for all datasets
     */
    Dataset(const Commons& commons, const std::string& name = std::string());

    /**
     * Construct dataset object for iterating through VTApi datasets
     * Object will represent set of datasets specified by their names
     * @param commons base Commons object
     * @param names list of dataset names
     */
    Dataset(const Commons& commons, const std::list<std::string>& names);

    using KeyValues::count;
    using KeyValues::print;
    using KeyValues::printAll;
    using KeyValues::printKeys;

    /**
     * Moves to a next dataset and sets dataset name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next() override;

    /**
     * Gets name of the current dataset
     * @return name of the current dataset
     */
    std::string getName();

    /**
     * Gets dataset location relative to base location (in config file)
     * @return location of the current dataset
     */
    std::string getLocation();
    
    /**
     * Gets friendly name of the current dataset
     * @return friendly name of the current dataset
     */
    std::string getFriendlyName();

    /**
     * Gets description of the current dataset
     * @return description of the current dataset
     */
    std::string getDescription();
    
    /**
     * Gets full path to sequence location
     * @return full path
     */
    std::string getDataLocation();

    /**
     * Sets dataset's friendly name
     * @param friendly_name new friendly name
     * @return success
     */
    bool updateFriendlyName(const std::string& friendly_name);
    
    /**
     * Sets dataset's description
     * @param description new description
     * @return success
     */
    bool updateDescription(const std::string& description);
    
    /**
     * Creates new sequence in dataset and returns its object for iteration
     * @param name sequence name (unique)
     * @param location location in dataset
     * @param type 'video', 'imagefolder' or 'data'
     * @param comment optional comment
     * @return sequence object, NULL on error
     */
    Sequence *createSequence(const std::string& name,
                             const std::string& location,
                             const std::string& type,
                             const std::string& comment = std::string());

    /**
     * Creates new video in dataset and returns its object for iteration
     * @param name video name (unique)
     * @param location location in dataset
     * @param realtime UNIX timestamp of start of the video
     * @param speed video speed [0,inf]
     * @param comment optional comment
     * @return video object, NULL on error
     */
    Video *createVideo(const std::string& name,
                       const std::string& location,
                       const time_t& realtime,
                       double speed,
                       const std::string& comment = std::string());

    /**
     * Creates new image folder in dataset and returns its object for iteration
     * @param name image folder name (unique)
     * @param location location in dataset
     * @param comment optional comment
     * @return image folder object, NULL on error
     */
    ImageFolder *createImageFolder(const std::string& name,
                                   const std::string& location,
                                   const std::string& comment = std::string());
    
    /**
     * @brief Creates new task for given method
     * @param mtname method name
     * @param params task parameters
     * @param prereq_task prerequisite task, empty for none
     * @param outputs optional output table
     * @return created task object, NULL on error
     */
    Task *createTask(const std::string& mtname,
                     const TaskParams& params,
                     const std::string& prereq_task,
                     const std::string& outputs = std::string());

    /**
     * Loads dataset's sequences for iteration
     * @param name   sequence name (no name = all sequences)
     * @return pointer to the new Sequence object
     */
    Sequence *loadSequences(const std::string& name = std::string());

    /**
     * Loads dataset's sequences for iteration
     * @param name   sequences names
     * @return pointer to the new Sequence object
     */
    Sequence *loadSequences(const std::list<std::string>& names);

    /**
     * Loads dataset's videos (= sequences) for iteration
     * @param name   video (sequence) name (no name = all sequences)
     * @return pointer to the new Video object
     */
    Video *loadVideos(const std::string& name = std::string());

    /**
     * Loads dataset's videos for iteration
     * @param name   videos names
     * @return pointer to the new Video object
     */
    Video *loadVideos(const std::list<std::string>& names);

    /**
     * Loads dataset's image folders (= sequences) for iteration
     * @param name image folder name (no name = all image folders)
     * @return pointer to the new ImageFolder object
     */
    ImageFolder *loadImageFolders(const std::string& name = std::string());

    /**
     * Loads dataset's image folders for iteration
     * @param names   image folders names
     * @return pointer to the new ImageFolder object
     */
    ImageFolder *loadImageFolders(const std::list<std::string>& names);

    /**
     * Loads dataset's processing tasks for iteration
     * @param name task name (no name = all tasks)
     * @return pointer to the new Task object
     */
    Task *loadTasks(const std::string& name = std::string());
    
    /**
     * Loads dataset's processing tasks for iteration
     * @param name   tasks names
     * @return pointer to the new Task object
     */
    Task *loadTasks(const std::list<std::string>& names);

    /**
     * Loads dataset's processes for iteration
     * @param id   process ID (0 = all processes)
     * @return pointer to the new Process object
     */
    Process *loadProcesses(int id = 0);

    /**
     * Loads dataset's processes for iteration
     * @param ids   processes IDs
     * @return pointer to the new Process object
     */
    Process *loadProcesses(const std::list<int>& ids);

    /**
     * @brief Deletes sequence with given name
     * @param seqname sequence name to delete
     * @return succesful delete
     */
    bool deleteSequence(const std::string &seqname);

    /**
     * @brief Deletes task with given name
     * @param taskname task name to delete
     * @return succesful delete
     */
    bool deleteTask(const std::string &taskname);

protected:
    virtual bool preUpdate();
    
private:
    Dataset() = delete;
    Dataset& operator=(const Dataset&) = delete;
};

} // namespace vtapi
