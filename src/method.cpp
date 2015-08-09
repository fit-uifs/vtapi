/**
 * @file
 * @brief   Methods of Method class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <functional>
#include <vtapi/common/global.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/insert.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/data/method.h>

using namespace std;

namespace vtapi {


Method::Method(const Commons& commons, const string& name)
    : KeyValues(commons)
{
    if (!name.empty())
        context().method = name;
    
    _select.from(def_tab_methods, def_col_all);
    
    if (!context().method.empty())
        _select.whereString(def_col_mt_name, context().method);
}

Method::Method(const Commons& commons, const list<string>& names)
    : KeyValues(commons)
{
    _select.from(def_tab_methods, def_col_all);

    _select.whereStringInList(def_col_mt_name, names);
}

Method::~Method()
{}


bool Method::next()
{
    if (KeyValues::next()) {
        context().method = this->getName();
        return true;
    }
    else {
        return false;
    }
}

string Method::getName()
{
    return this->getString(def_col_mt_name);
}

string Method::getDescription()
{
    return this->getString(def_col_mt_description);
}

bool Method::updateDescription(const string& description)
{
    return this->updateString(def_col_mt_description, description);
}

Task* Method::createTask(
    const TaskParams& params,
    const list<string>& tasknames_prereq,
    const string& outputTable)
{
    Task *ts = NULL;
    bool retval = true;

    //TODO: validace parametru

    string name = constructTaskName(params);

    QueryBeginTransaction(*this).execute();
    
    {
        Insert insert(*this, def_tab_tasks);
        retval &= insert.keyString(def_col_task_name, name);
        retval &= insert.keyString(def_col_task_mtname, context().method);
        retval &= insert.keyString(def_col_task_params, params.serialize());
        if (!outputTable.empty())
            retval &= insert.keyString(def_col_task_outputs, outputTable);

        if (retval && (retval = insert.execute())) {
            for (auto & item : tasknames_prereq) {
                Insert insert2(*this, def_tab_tasks_prereq);
                retval &= insert.keyString(def_col_tprq_taskname, name);
                retval &= insert.keyString(def_col_tprq_taskprereq, item);
                if (retval && (retval = insert2.execute())) {
                }
                else {
                    break;
                }
            }
        }
    }
    
    if (retval) {
        QueryCommitTransaction(*this).execute();

        ts = loadTasks(name);
        if (!ts->next()) vt_destruct(ts);
    }
    else {
        QueryRollbackTransaction(*this).execute();
    }

    return ts;
}

Task* Method::loadTasks(const string& name)
{
    return (new Task(*this, name));
}

string Method::constructTaskName(const TaskParams & params)
{
    string input;

    input += context().method;
    input += 'p';

    string par = params.serializeAsName();
    if (!par.empty()) {
        input += '_';
        input += par;
    }

    hash<string> hash_fn;
    stringstream ss;
    ss << hex << hash_fn(input);

    return ss.str();
}

bool Method::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_methods);
    if (ret) {
        ret &= _update->whereString(def_col_mt_name, context().method);
    }

    return ret;
}

}
