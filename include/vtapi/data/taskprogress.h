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

#include <list>
#include "keyvalues.h"
#include "task.h"
#include "process.h"
#include "sequence.h"


namespace vtapi {

class Task;
class Process;
class Sequence;


class TaskProgress : protected KeyValues
{
public:
    TaskProgress(const Commons &commons,
                 const std::string& taskname,
                 const std::string& seqname = std::string());
    TaskProgress(const Commons &commons,
                 const std::string& taskname,
                 const std::list<std::string>& seqnames);

    using KeyValues::count;
    using KeyValues::print;
    using KeyValues::printAll;
    using KeyValues::printKeys;

    bool next() override;

    std::string getTaskName();
    Task *getTask();

    int getProcessId();
    Process *getProcess();

    std::string getSequenceName();
    Sequence *getSequence();

    bool getIsDone();

    time_t getStartedTime();
    time_t getFinishedTime();

    bool updateIsDone(bool value);

protected:
    virtual bool preUpdate();
};


}
