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
#include <Poco/Path.h>
#include <Poco/SharedLibrary.h>
#include <vtapi/common/global.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/insert.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/data/method.h>

using namespace std;

namespace vtapi {


Method::Method(const Method &copy)
    : Method(dynamic_cast<const Commons&>(copy))
{
}

Method::Method(const Commons& commons, const string& name)
    : KeyValues(commons, def_tab_methods)
{
    if (!name.empty())
        _context.method = name;
    
    _select.setOrderBy(def_col_mt_name);
    
    if (!_context.method.empty())
        _select.querybuilder().whereString(def_col_mt_name, _context.method);
}

Method::Method(const Commons& commons, const vector<string>& names)
    : KeyValues(commons, def_tab_methods)
{
    _select.setOrderBy(def_col_mt_name);
    _select.querybuilder().whereStringVector(def_col_mt_name, names);
}

bool Method::next()
{
    if (KeyValues::next()) {
        _context.method = this->getName();
        return true;
    }
    else {
        return false;
    }
}

string Method::getName() const
{
    return this->getString(def_col_mt_name);
}

string Method::getDescription() const
{
    return this->getString(def_col_mt_description);
}

string Method::getPluginPath() const
{
    string path =
            config().modules_dir +
            Poco::Path::separator() +
            "libvtmodule_" + this->getName() +
            Poco::SharedLibrary::suffix();
    return Poco::Path::expand(path);
}

bool Method::updateDescription(const string& description)
{
    return this->updateString(def_col_mt_description, description);
}

Task *Method::createTask(const string& dsname,
                 const TaskParams& params,
                 const string& prereq_task,
                 const string& outputs) const
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

Task* Method::loadTasks(const string& name) const
{
    return (new Task(*this, name));
}

bool Method::deleteTask(const string &dsname, const string &taskname) const
{
    return QueryTaskDelete(*this, dsname, taskname).execute();
}

bool Method::preUpdate()
{
    return update().querybuilder().whereString(def_col_mt_name, _context.method);
}

}
