/**
 * @file
 * @brief   Declaration of TaskProgress class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */


#pragma once

#include "keyvalues.h"
#include "task.h"
#include "process.h"
#include "sequence.h"

namespace vtapi {

class Task;
class Process;
class Sequence;

/**
 * @brief Class represents task's processing progress on specified sequences
 * Iterate with next() over progress information for multiple sequences
 * Query progress information with get*()
 * Update progress information for sequence with updateIsDone()
 */
class TaskProgress : protected KeyValues
{
public:
    /**
     * @brief Constructor for one or all sequences
     * @param commons shared commons object
     * @param taskname task for which progress is queried
     * @param seqname sequence name for which progress is queried, empty for all
     * @param acquired if seqname is set with acquire==true, this object then expects
     * succesful call to updateIsDone() or it deletes progress info on destruction
     */
    TaskProgress(const Commons &commons,
                 const std::string& taskname,
                 const std::string& seqname = std::string(),
                 bool acquired = false);

    /**
     * @brief Constructor for set of sequences
     * @param commons shared commons object
     * @param taskname task for which progress is queried
     * @param seqnames sequence names for which progress is queried
     */
    TaskProgress(const Commons &commons,
                 const std::string& taskname,
                 const std::vector<std::string>& seqnames);

    ~TaskProgress();

    using KeyValues::count;

    /**
     * @brief Step to progress information for next sequence
     * @return progress information exists
     */
    bool next() override;

    /**
     * @brief Gets current progress's task name
     * @return task name
     */
    std::string getTaskName() const;

    /**
     * @brief Gets current progress's task object
     * @return  task object
     */
    Task *getTask() const;

    /**
     * @brief Gets current progress's process ID
     * @return process ID
     */
    int getProcessId() const;

    /**
     * @brief Gets current progress's process object
     * @return process object
     */
    Process *getProcess() const;

    /**
     * @brief Gets current progress's sequence name
     * @return sequence name
     */
    std::string getSequenceName() const;

    /**
     * @brief Gets current progress's sequence object
     * @return sequence object
     */
    Sequence *getSequence() const;

    /**
     * @brief Is process finished with sequence
     * @return boolean
     */
    bool getIsDone() const;

    /**
     * @brief Gets time when sequence was acquired by process
     * @return time
     */
    std::chrono::system_clock::time_point getStartedTime() const;

    /**
     * @brief Gets time when sequence was finished by process
     * @return time
     */
    std::chrono::system_clock::time_point getFinishedTime() const;

    /**
     * @brief Mark sequence as finished or in progress
     * Sequence must be acquired to call this method (acquire argument in constructor)
     * @param finished true if sequence is finished, false on in progress
     * @return success
     */
    bool updateIsDone(bool finished);

protected:
    bool preUpdate() override;

private:
    bool _acquired;
    bool _update_set;
};


}
