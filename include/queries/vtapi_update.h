/**
 * @file
 * @brief   Declaration of Update class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_UPDATE_H
#define	VTAPI_UPDATE_H


#include "vtapi_query.h"

namespace vtapi {
    

/**
 * @brief Class for constructing and executing UPDATE queries
 * 
 * @see Basic information on page @ref KEYVALUES
 * 
 * @note Error codes 23*
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Update : public Query {
public:

    /**
     * Constructor of an Update query object
     * @param commons      configuration object of Commons class
     * @param initString   initialization string (query/table or empty)
     */
    Update(const Commons& commons, const std::string& initString = "");
    /**
     * Gets UPDATE query string
     * @return query string
     */
    std::string getQuery();
    /**
     * Executes UPDATE query
     * @return success
     */
    bool execute();
    /**
     * This is a persistent function to set string value to a key
     * @param key       key holding value to be set
     * @param value     string value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setString(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to set string values (string array) to a key
     * @param key       key holding value to be set
     * @param values    string array to be set
     * @param size      size of array
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setStringA(const std::string& key, std::string* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to set integer value to a key
     * @param key       key holding value to be set
     * @param value     integer value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setInt(const std::string& key, int value, const std::string& from = "");
    /**
     * This is a persistent function to set integer values (integer array) to a key
     * @param key       key holding value to be set
     * @param values    integer array to be set
     * @param size      size of array
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setIntA(const std::string& key, int* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to set float value to a key
     * @param key       key holding value to be set
     * @param value     float value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setFloat(const std::string& key, float value, const std::string& from = "");
    /**
     * This is a persistent function to set float values (float array) to a key
     * @param key       key holding value to be set
     * @param values    float array to be set
     * @param size      size of array
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setFloatA(const std::string& key, float* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to set seqtype value to a key
     * @param key       key holding value to be set
     * @param value     seqtype value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setSeqtype(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to set inouttype value to a key
     * @param key       key holding value to be set
     * @param value     inouttype value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setInouttype(const std::string& key, const std::string& value, const std::string& from = "");
    /* * //TODO: delete a space between stars after setPermissions will be actual
     * This is a persistent function to set permissions value to a key
     * @param key       key holding value to be set
     * @param value     permissions value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
//TODO: delete a space between stars in the upper doxygen comment after setPermissions will be actual
//    bool setPermissions(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to set timestamp value to a key
     * @param key       key holding value to be set
     * @param value     timestamp value to be set
     * @param from      selection table (optional)
     * @return success
     * @note It may be called several times.
     */
    bool setTimestamp(const std::string& key, const time_t& value, const std::string& from = "");
    
    /**
     * This is a WHERE statement construction function for strings
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
    bool whereString(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    /**
     * This is a WHERE statement construction function for integers
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
    bool whereInt(const std::string& key, const int value, const std::string& oper = "=", const std::string& from = "");
    /**
     * This is a WHERE statement construction function for floats
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
    bool whereFloat(const std::string& key, const float value, const std::string& oper = "=", const std::string& from = "");
    /**
     * This is a WHERE statement construction function for seqtype
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     * @note It may be called several times.
     */
     bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    /**
     * This is a WHERE statement construction function for inouttype
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
     bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    /* * //TODO: delete a space between stars after wherePermissions will be actual
     * This is a WHERE statement construction function for permissions
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
//TODO: delete a space between stars in the upper doxygen comment after wherePermissions will be actual
//     bool wherePermissions(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    /**
     * This is a WHERE statement construction function for timestamp
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
     bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper = "=", const std::string& from = "");

};

} // namespace vtapi

#endif	/* VTAPI_UPDATE_H */

