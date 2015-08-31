/**
 * @file
 * @brief   Declaration of Update class
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
 * @brief Class for constructing and executing UPDATE queries
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
class Update : public Query
{
public:

    /**
     * Constructor of an Update query object
     * @param commons configuration object of Commons class
     * @param table which table to update
     */
    Update(const Commons& commons, const std::string& table)
        : Query(commons, table, true) {}
    
    /**
     * Gets UPDATE query string
     * @return query string
     */
    std::string getQuery() const override
    { return _pquerybuilder->getUpdateQuery(); }
    
private:
    Update() = delete;
};

} // namespace vtapi
