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

#include <ctime>
#include <list>
#include "../data/processstate.h"
#include "../data/intervalevent.h"
#include "../data/commons.h"
#include "query.h"

namespace vtapi {
    

/**
 * @brief Class for constructing and executing UPDATE queries
 * 
 * @see Basic information on page @ref KEYVALUES
 * 
 * @note Error codes 23*
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Update : public QueryWhere
{
public:

    /**
     * Constructor of an Update query object
     * @param commons configuration object of Commons class
     * @param table which table to update
     */
    Update(const Commons& commons, const std::string& table);
    
    /**
     * Gets UPDATE query string
     * @return query string
     */
    std::string getQuery() override;
    
    /**
     * This is a persistent function to set string value to a key
     * @param key       key holding value to be set
     * @param value     string value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setString(const std::string& key, const std::string& value);
    
    /**
     * This is a persistent function to set string values (string array) to a key
     * @param key       key holding value to be set
     * @param values    string array to be set
     * @param size      size of array
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setStringA(const std::string& key, std::string* values, const int size);
    
    /**
     * This is a persistent function to set bool value to a key
     * @param key       key holding value to be set
     * @param value     integer value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setBool(const std::string& key, bool value);

    /**
     * This is a persistent function to set integer value to a key
     * @param key       key holding value to be set
     * @param value     integer value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setInt(const std::string& key, int value);
    
    /**
     * This is a persistent function to set integer values (integer array) to a key
     * @param key       key holding value to be set
     * @param values    integer array to be set
     * @param size      size of array
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setIntA(const std::string& key, int* values, const int size);
    
    /**
     * This is a persistent function to set float value to a key
     * @param key       key holding value to be set
     * @param value     float value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setFloat(const std::string& key, float value);
    
    /**
     * This is a persistent function to set float values (float array) to a key
     * @param key       key holding value to be set
     * @param values    float array to be set
     * @param size      size of array
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setFloatA(const std::string& key, float* values, const int size);
    
    /**
     * This is a persistent function to set seqtype value to a key
     * @param key       key holding value to be set
     * @param value     seqtype value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setSeqtype(const std::string& key, const std::string& value);
    
    /**
     * This is a persistent function to set inouttype value to a key
     * @param key       key holding value to be set
     * @param value     inouttype value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setInouttype(const std::string& key, const std::string& value);
    
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setProcessStatus(const std::string& key, ProcessState::STATUS_T value);
    
    /**
     * This is a persistent function to set timestamp value to a key
     * @param key       key holding value to be set
     * @param value     timestamp value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setTimestamp(const std::string& key, const time_t& value);
    
private:
    Update() = delete;
    Update(const Update&) = delete;
    Update& operator=(const Update&) = delete;
};

} // namespace vtapi
