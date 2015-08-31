/**
 * @file
 * @brief   Declaration of Delete class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "query.h"

namespace vtapi {


class Delete : public Query
{
public:
    Delete(const Commons& commons, const std::string& table)
        : Query(commons, table, true) {}

    /**
     * Gets DELETE query string
     * @return query string
     */
    std::string getQuery() const override
    { return _pquerybuilder->getDeleteQuery(); }

private:
    Delete() = delete;
};


}
