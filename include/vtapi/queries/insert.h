/**
 * @file
 * @brief   Declaration of Insert class
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

    
/**
 * @brief Class for constructing and executing INSERT queries
 * 
 * @see Basic information on page @ref KEYVALUES
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Insert : public Query
{
public:
    /**
     * Constructor of an INSERT query object
     * @param commons configuration object of Commons class
     * @param table table into which to insert
     */
    Insert(const Commons& commons, const std::string& table)
        : Query(commons, table, true) {}
    
    /**
     * Gets INSERT query string
     * @return query string
     */
    std::string getQuery() const override
    { return _pquerybuilder->getInsertQuery(); }

private:
    Insert() = delete;
    Insert(const Insert&) = delete;
    Insert& operator=(const Insert&) = delete;
};

} // namespace vtapi
