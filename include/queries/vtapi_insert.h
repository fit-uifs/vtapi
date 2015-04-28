/**
 * @file
 * @brief   Declaration of Insert class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_INSERT_H
#define	VTAPI_INSERT_H

#include "vtapi_query.h"

namespace vtapi {

    
/**
 * @brief Class for constructing and executing INSERT queries
 * 
 * @see Basic information on page @ref KEYVALUES
 * 
 * @note Error codes 22*
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Insert : public Query {
public:

    /**
     * Constructor of an INSERT query object
     * @param commons      configuration object of Commons class
     * @param initString   default table into which to insert / full SQL query
     */
    Insert(const Commons& commons, const std::string& initString = "");
    /**
     * Gets INSERT query string
     * @return query string
     */
    std::string getQuery();
    /**
     * Executes INSERT query
     * @return success
     */
    bool execute();
    /**
     * This is a persistent function to add string value to a key
     * @param key      key holding value to be inserted
     * @param value    string to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyString(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to add string values (string array) to a key
     * @param key      key holding values to be inserted
     * @param values   string array to be inserted
     * @param size     size of array
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to add integer value to a key
     * @param key      key holding value to be inserted
     * @param value    integer to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyInt(const std::string& key, int value, const std::string& from = "");
    /**
     * This is a persistent function to add integer values (integer array) to a key
     * @param key      key holding value to be inserted
     * @param values   integer array to be inserted
     * @param size     size of array
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyIntA(const std::string& key, int* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to add float value to a key
     * @param key      key holding value to be inserted
     * @param value    float value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyFloat(const std::string& key, float value, const std::string& from = "");
    /**
     * This is a persistent function to add float values (float array) to a key
     * @param key      key holding value to be inserted
     * @param values   float array to be inserted
     * @param size     size of array
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyFloatA(const std::string& key, float* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to add seqtype value to a key
     * @param key      key holding value to be inserted
     * @param value    seqtype value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keySeqtype(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to add inouttype value to a key
     * @param key      key holding value to be inserted
     * @param value    inouttype value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyInouttype(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to add timestamp value to a key
     * @param key      key holding value to be inserted
     * @param value    timestamp value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyTimestamp(const std::string& key, const time_t& value, const std::string& from = "");
    
#if HAVE_OPENCV
    /**
     * This is a persistent function to add cv::Mat value to a key
     * @param key      key holding value to be inserted
     * @param value    cv::Mat value to be inserted
     * @param from     selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from = "");
#endif
    
    /**
     * This is a persistent function to add IntervalEvent value to a key
     * @param key     key holding value to be inserted
     * @param value   IntervalEvent value to be inserted
     * @param from    selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyIntervalEvent(const std::string& key, const IntervalEvent& value, const std::string& from = "");
    /**
     * This is a persistent function to add ProcessState value to a key
     * @param key     key holding value to be inserted
     * @param value   ProcessState value to be inserted
     * @param from    selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool keyProcessState(const std::string& key, const ProcessState& value, const std::string& from = "");
};

} // namespace vtapi

#endif	/* VTAPI_INSERT_H */

