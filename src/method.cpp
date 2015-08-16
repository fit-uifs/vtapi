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
    _select.orderBy(def_col_mt_name);
    
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

Task *Method::createTask(const string& dsname,
                 const TaskParams& params,
                 const string& prereq_task,
                 const string& outputs)
{
    Task *ts = NULL;
    string name = Task::constructName(this->getName(), params);

    // TODO: validace parametru

    QueryTaskCreate q(*this,
                    name,
                    dsname,
                    this->getName(),
                    params.serialize(),
                    prereq_task,
                    outputs);

    if (q.execute()) {
        ts = new Task(*this, name);
        if (!ts->next())
            vt_destruct(ts);
    }

    return ts;
}

Task* Method::loadTasks(const string& name)
{
    return (new Task(*this, name));
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
