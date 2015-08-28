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

#include <ctime>
#include "../data/processstate.h"
#include "../data/intervalevent.h"
#include "../data/commons.h"
#include "query.h"

namespace vtapi {

    
/**
 * @brief Class for constructing and executing INSERT queries
 * 
 * @see Basic information on page @ref KEYVALUES
 * 
 * @note Error codes 22*
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
    Insert(const Commons& commons, const std::string& table);
    
    /**
     * Gets INSERT query string
     * @return query string
     */
    std::string getQuery() override;
    
    /**
     * This is a persistent function to add string value to a key
     * @param key      key holding value to be inserted
     * @param value    string to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyString(const std::string& key, const std::string& value);
    
    /**
     * This is a persistent function to add string values (string array) to a key
     * @param key      key holding values to be inserted
     * @param values   string array to be inserted
     * @param size     size of array
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyStringA(const std::string& key, std::string* values, const int size);
    
    /**
     * @brief Add bool value
     * @param key key holding values to be inserted
     * @param value bool value
     * @param from selection table (optional)
     * @return success
     */
    bool keyBool(const std::string& key, bool value);

    /**
     * This is a persistent function to add integer value to a key
     * @param key      key holding value to be inserted
     * @param value    integer to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyInt(const std::string& key, int value);
    
    /**
     * This is a persistent function to add integer values (integer array) to a key
     * @param key      key holding value to be inserted
     * @param values   integer array to be inserted
     * @param size     size of array
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyIntA(const std::string& key, int* values, const int size);
    
    /**
     * This is a persistent function to add float value to a key
     * @param key      key holding value to be inserted
     * @param value    float value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyFloat(const std::string& key, float value);

    /**
     * This is a persistent function to add float values (float array) to a key
     * @param key      key holding value to be inserted
     * @param values   float array to be inserted
     * @param size     size of array
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyFloatA(const std::string& key, float* values, const int size);

    /**
     * This is a persistent function to add float value to a key
     * @param key      key holding value to be inserted
     * @param value    float value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyFloat8(const std::string& key, double value);

    /**
     * This is a persistent function to add float values (float array) to a key
     * @param key      key holding value to be inserted
     * @param values   float array to be inserted
     * @param size     size of array
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyFloat8A(const std::string& key, double* values, const int size);
    
    /**
     * This is a persistent function to add seqtype value to a key
     * @param key      key holding value to be inserted
     * @param value    seqtype value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keySeqtype(const std::string& key, const std::string& value);
    
    /**
     * This is a persistent function to add inouttype value to a key
     * @param key      key holding value to be inserted
     * @param value    inouttype value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyInouttype(const std::string& key, const std::string& value);
    
    /**
     * This is a persistent function to add timestamp value to a key
     * @param key      key holding value to be inserted
     * @param value    timestamp value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyTimestamp(const std::string& key, const time_t& value);
    
    /**
     * This is a persistent function to add cv::Mat value to a key
     * @param key      key holding value to be inserted
     * @param value    cv::Mat value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyCvMat(const std::string& key, const cv::Mat& value);
    
    /**
     * This is a persistent function to add IntervalEvent value to a key
     * @param key     key holding value to be inserted
     * @param value   IntervalEvent value to be inserted
     * @param from    selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyIntervalEvent(const std::string& key, const IntervalEvent& value);
    
    /**
     * This is a persistent function to add ProcessState value to a key
     * @param key     key holding value to be inserted
     * @param value   ProcessState value to be inserted
     * @param from    selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyProcessState(const std::string& key, const ProcessState& value);

private:
    Insert() = delete;
    Insert(const Insert&) = delete;
    Insert& operator=(const Insert&) = delete;
};

} // namespace vtapi
