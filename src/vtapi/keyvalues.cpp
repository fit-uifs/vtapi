/**
 * @file
 * @brief   Methods of KeyValues class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <iostream>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/data/keyvalues.h>
#include <utility>

using namespace std;

namespace vtapi {


KeyValues::KeyValues(const Commons& commons, const string& selection)
    : Commons(commons, false), _select(commons, selection)
{
    if (selection.empty())
        throw RuntimeException("no table specified for KeyValues");

    _context.selection = selection;

    _select.setLimit(def_select_limit);
}

KeyValues::~KeyValues()
{
    if (_pupdate) _pupdate->execute();
}

bool KeyValues::next()
{
    // UPDATE data is handled automatically here
    if (_pupdate && !updateExecute())
        return false;
    
    // SELECT data if result set is not valid
    if (_select.resultset().getPosition() == -1) {
        if (!_select.execute() || _select.resultset().countRows() == 0) {
            _select.resultset().setPosition(-1);
            return false;
        }
        else {
            _select.resultset().setPosition(0);
            return true;
        }
    }
    // use previous resultset
    else {
        // check SELECT limit, should new resultset be fetched?
        if (def_select_limit > 0 && _select.resultset().getPosition() + 1 >= def_select_limit) {
            if (!_select.executeNext() || _select.resultset().countRows() == 0) {
                _select.resultset().setPosition(-1);
                return false;
            }
            else {
                _select.resultset().setPosition(0);
                return true;
            }
        }
        // step through resultset
        else {
            if (_select.resultset().countRows() > _select.resultset().getPosition() + 1) {
                _select.resultset().incPosition();
                return true;
            }
            else {
                _select.resultset().setPosition(-1);
                return false;
            }
        }
    }
}

int KeyValues::count()
{
    int cnt = -1;

    std::string query   = _select.querybuilder().getCountQuery();
    void *param         = _select.querybuilder().getQueryParam();
    void *paramDup      = _select.querybuilder().duplicateQueryParam(param);
    ResultSet *res      = backend().createResultSet(connection().getDBTypes());

    if (connection().fetch(query, paramDup, *res) > 0) {
        res->setPosition(0);
        cnt = res->getInt8(0);
    }

    if (paramDup) _select.querybuilder().destroyQueryParam(paramDup);
    vt_destruct(res);

    return cnt;
}

Update &KeyValues::update()
{
    if (!_pupdate) {
        _pupdate = std::make_shared<Update>(*this, _context.selection);
        if (!preUpdate())
            throw RuntimeException("preUpdate() failed");
    }

    return *_pupdate;
}

bool KeyValues::preUpdate()
{
    VTLOG_WARNING("Update performed using generic KeyValues");
    return true;
}

bool KeyValues::updateExecute()
{
    if (_pupdate) {
        bool ret = _pupdate->execute();
        _pupdate.reset();
        return ret;
    }
    else {
        return false;
    }
}


}
