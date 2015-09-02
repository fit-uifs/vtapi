/**
 * @file
 * @brief   Declaration of Keyvalues class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "commons.h"
#include "intervalevent.h"
#include "processstate.h"
#include "../common/tkey.h"
#include "../queries/select.h"
#include "../queries/update.h"
#include <chrono>
#include <vector>
#include <memory>

namespace vtapi {


/**
 * @brief KeyValues storage class
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class KeyValues : public Commons
{
public:
    /**
     * Constructor of base KeyValues object representing generic DB tuples
     * @param orig base Commons object
     * @param selection specific DB table
     */
    KeyValues(const Commons& orig, const std::string& selection);

    /**
     * This destroys the KeyValues
     * It raises warning when there was something left not-excecuted (some collaborants left)
     */
    virtual ~KeyValues();

    /**
     * The most used function of the VTApi - next (row)
     * @return cursor succesfully set to next existing row
     */
    virtual bool next();

    /**
     * Count total number of represented objects
     * @return object count, -1 for error
     */
    int count();

    /**
     * Executes SQL UPDATE command
     * @return success
     */
    bool updateExecute();

    // =============== GETTERS (Select) ===============

    /**
     * Gets key of a single table column
     * @param col   column index
     * @return Column key
     */
    TKey getKey(int col) const
    { return _select._presultset->getKey(col); }

    /**
     * Gets a vector of all possible columns as TKeys
     * @return vector of keys
     */
    TKeys getKeys() const
    { return _select._presultset->getKeys(); }

    /**
     * @brief Generic getter - fetches any value from resultset and returns it as string
     * @param key column key
     * @return value
     */
    inline std::string getValue(const std::string& key) const
    { return _select._presultset->getValue(key); }

     /**
     * @brief Generic getter - fetches any value from resultset and returns it as string
     * @param col   index of a column
     * @return string representation of field value
     */
    inline std::string getValue(const int col) const
    { return _select._presultset->getValue(col); }

    /**
     * Gets a boolean value specified by a column key
     * @param key   column key
     * @return integer value
     */
    inline bool getBool(const std::string& key) const
    { return _select._presultset->getBool(key); }

    /**
     * Gets a boolean value specified by an indef of a column
     * @param key   column key
     * @return integer value
     */
    inline bool getBool(int col) const
    { return _select._presultset->getBool(col); }

    /**
     * Gets a single character specified by a column key
     * @param key   column key
     * @return character
     */
    inline char getChar(const std::string& key) const
    { return _select._presultset->getChar(key); }

    /**
     * Gets a single character specified by an index of a column
     * @param col   index of the column
     * @return character
     */
    inline char getChar(int col) const
    { return _select._presultset->getChar(col); }

    /**
     * Gets a character array specified by a column key
     * @param key   column key
     * @return character array
     */
    inline std::string getString(const std::string& key) const
    { return _select._presultset->getString(key); }

    /**
     * Gets a string value specified by an index of a column
     * @param col   index of the column
     * @return string value
     */
    inline std::string getString(int col) const
    { return _select._presultset->getString(col); }

    /**
     * Gets a vector of string values specified by a column key
     * @param key   column key
     * @return vector of string values
     */
    inline std::vector<std::string> getStringVector(const std::string& key) const
    { return _select._presultset->getStringVector(key); }

    /**
     * Gets a vector of string values specified by a column key
     * @param col   index of the column
     * @return vector of string values
     */
    inline std::vector<std::string> getStringVector(int col) const
    { return _select._presultset->getStringVector(col); }

    /**
     * Gets an integer value specified by a column key
     * @param key   column key
     * @return integer value
     */
    inline int getInt(const std::string& key) const
    { return _select._presultset->getInt(key); }

    /**
     * Gets an integer value specified by an index of a column
     * @param col   index of the column
     * @return integer value
     */
    inline int getInt(int col) const
    { return _select._presultset->getInt(col); }
    
    /**
     * Gets a vector of integer values specified by a column key
     * @param key   column key
     * @return vector of integer values
     */
    inline std::vector<int> getIntVector(const std::string& key) const
    { return _select._presultset->getIntVector(key); }

    /**
     * Gets a vector of integer values specified by an index of a column
     * @param col   index of the column
     * @return vector of integer values
     */
    inline std::vector<int> getIntVector(int col) const
    { return _select._presultset->getIntVector(col); }

    /**
     * Gets a long integer value specified by a column key
     * @param key   column key
     * @return long integer value
     */
    inline long long getInt8(const std::string& key) const
    { return _select._presultset->getInt8(key); }

    /**
     * Gets a long integer value specified by an index of a column
     * @param col   index of the column
     * @return long integer value
     */
    inline long long getInt8(int col) const
    { return _select._presultset->getInt8(col); }

    /**
     * Gets a vector of long integer values specified by a column key
     * @param key   column key
     * @return vector of integer values
     */
    inline std::vector<long long> getInt8Vector(const std::string& key) const
    { return _select._presultset->getInt8Vector(key); }

    /**
     * Gets a vector of long integer values specified by an index of a column
     * @param col   index of the column
     * @return vector of integer values
     */
    inline std::vector<long long> getInt8Vector(int col) const
    { return _select._presultset->getInt8Vector(col); }

    /**
     * Gets a float value specified by a column key
     * @param key   column key
     * @return float value
     */
    inline float getFloat(const std::string& key) const
    { return _select._presultset->getFloat(key); }

    /**
     * Gets a float value specified by an index of a column
     * @param col   index of the column
     * @return float value
     */
    inline float getFloat(int col) const
    { return _select._presultset->getFloat(col); }
    
    /**
     * Gets a vector of float values specified by an index of a column
     * @param key   column key
     * @return vector of float values
     */
    inline std::vector<float> getFloatVector(const std::string& key) const
    { return _select._presultset->getFloatVector(key); }

    /**
     * Gets a vector of float values specified by a column key
     * @param col   index of the column
     * @return vector of integer values
     */
    inline std::vector<float> getFloatVector(int col) const
    { return _select._presultset->getFloatVector(col); }

    /**
     * Gets a double value specified by a column key
     * @param key   column key
     * @return double value
     */
    inline double getFloat8(const std::string& key) const
    { return _select._presultset->getFloat8(key); }

    /**
     * Gets a double value specified by an index of a column
     * @param col   index of the column
     * @return double value
     */
    inline double getFloat8(int col) const
    { return _select._presultset->getFloat8(col); }

    /**
     * Gets a vector of double values specified by an index of a column
     * @param key   column key
     * @return vector of float values
     */
    inline std::vector<double> getFloat8Vector(const std::string& key) const
    { return _select._presultset->getFloat8Vector(key); }

    /**
     * Gets a vector of double values specified by a column key
     * @param col   index of the column
     * @return vector of integer values
     */
    inline std::vector<double> getFloat8Vector(int col) const
    { return _select._presultset->getFloat8Vector(col); }

    /**
     * Gets a timestamp specified by a column key
     * @param key   column key
     * @return Timestamp info
     */
    inline std::chrono::system_clock::time_point getTimestamp(const std::string& key) const
    { return _select._presultset->getTimestamp(key); }

    /**
     * Gets a timestamp specified by an index of a column
     * @param col   index of the column
     * @return Timestamp info
     */
    inline std::chrono::system_clock::time_point getTimestamp(int col) const
    { return _select._presultset->getTimestamp(col); }

    /**
     * Gets an OpenCV matrix (cv::Mat) specified by a column key
     * @param key   column key
     * @return CvMat structure
     */
    inline cv::Mat getCvMat(const std::string& key) const
    { return _select._presultset->getCvMat(key); }

    /**
     * Gets an OpenCV matrix (cv::Mat) specified by an index of a column
     * @param col   index of the column
     * @return CvMat structure
     */
    inline cv::Mat getCvMat(int col) const
    { return _select._presultset->getCvMat(col); }

    /**
     * Gets a 2D point specified by a column key
     * @param key   column key
     * @return 2D Point
     */
    inline Point getPoint(const std::string& key) const
    { return _select._presultset->getPoint(key); }

    /**
     * Gets a 2D point specified by an index of a column
     * @param col   index of the column
     * @return 2D Point
     */
    inline Point getPoint(int col) const
    { return _select._presultset->getPoint(col); }

    /**
     * Gets a vector of 2D points specified by a column key
     * @param key   column key
     * @return vector of 2D Points
     */
    inline std::vector<Point> getPointVector(const std::string& key) const
    { return _select._presultset->getPointVector(key); }

    /**
     * Gets a vector of 2D points specified by an index of a column
     * @param col   index of the column
     * @return vector of 2D Points
     */
    inline std::vector<Point> getPointVector(int col) const
    { return _select._presultset->getPointVector(col); }

    /**
     * Gets interval event by a column key
     * @param key   column key
     * @return interval event class
     */
    inline IntervalEvent getIntervalEvent(const std::string& key) const
    { return _select._presultset->getIntervalEvent(key); }

    /**
     * Gets interval event by an index of a column
     * @param col   index of the column
     * @return interval event class
     */
    inline IntervalEvent getIntervalEvent(int col) const
    { return _select._presultset->getIntervalEvent(col); }

    /**
     * Gets process state by a column key
     * @param key   column key
     * @return process state class
     */
    inline ProcessState getProcessState(const std::string& key) const
    { return _select._presultset->getProcessState(key); }

    /**
     * Gets process state by an index of a column
     * @param col   index of the column
     * @return process state class
     */
    inline ProcessState getProcessState(int col) const
    { return _select._presultset->getProcessState(col); }

    /**
     * Gets binary data by a column key
     * @param key   column key
     * @param size size of output data
     * @return allocated data
     */
    inline std::vector<char> getBlob(const std::string& key) const
    { return _select._presultset->getBlob(key); }

    /**
     * Gets binary data by a column index
     * @param col   column index
     * @param size size of output data
     * @return allocated data
     */
    inline std::vector<char> getBlob(int col) const
    { return _select._presultset->getBlob(col); }

    // =============== SETTERS (Update) ===============

    /**
     * Sets a new boolean value of a specified key
     * @param key     column key to update
     * @param value   new boolean value of the key
     * @return success
     */
    inline bool updateBool(const std::string& key, bool value)
    { return update()._pquerybuilder->keyBool(key, value); }

    /**
     * Sets a new char value of a specified key
     * @param key     column key to update
     * @param value   new char value of the key
     * @return success
     */
    inline bool updateChar(const std::string& key, char value)
    { return update()._pquerybuilder->keyChar(key, value); }

    /**
     * Sets a new string value of a specified key
     * @param key     column key to update
     * @param value   new string value of the key
     * @return success
     */
    inline bool updateString(const std::string& key, const std::string& value)
    { return update()._pquerybuilder->keyString(key, value); }

    /**
     * Sets a new string vector value of a specified key
     * @param key     column key to update
     * @param values   new string vector value of the key
     * @return success
     */
    inline bool updateStringVector(const std::string& key, const std::vector<std::string>& values)
    { return update()._pquerybuilder->keyStringVector(key, values); }

    /**
     * Sets a new integer value of a specified key
     * @param key     column key to update
     * @param value   new integer value of the key
     * @return success
     */
    inline bool updateInt(const std::string& key, int value)
    { return update()._pquerybuilder->keyInt(key, value); }

    /**
     * Sets a new integer vector of a specified key
     * @param key      column key to update
     * @param values   new integer vector of the key
     * @return success
     */
    inline bool updateIntVector(const std::string& key, const std::vector<int> &values)
    { return update()._pquerybuilder->keyIntVector(key, values); }

    /**
     * Sets a new long integer value of a specified key
     * @param key     column key to update
     * @param value   new long integer value of the key
     * @return success
     */
    inline bool updateInt8(const std::string& key, long long value)
    { return update()._pquerybuilder->keyInt8(key, value); }

    /**
     * Sets a new long integer vector of a specified key
     * @param key      column key to update
     * @param values   new long integer vector of the key
     * @return success
     */
    inline bool updateInt8Vector(const std::string& key, const std::vector<long long> &values)
    { return update()._pquerybuilder->keyInt8Vector(key, values); }

    /**
     * Sets a new float value of a specified key
     * @param key     column key to update
     * @param value   new float value of the key
     * @return success
     */
    inline bool updateFloat(const std::string& key, float value)
    { return update()._pquerybuilder->keyFloat(key, value); }

    /**
     * Sets a new float vector of a specified key
     * @param key      column key to update
     * @param values   new float vector of the key
     * @return success
     * @unimplemented
     */
    inline bool updateFloatVector(const std::string& key, const std::vector<float> &values)
    { return update()._pquerybuilder->keyFloatVector(key, values); }

    /**
     * Sets a new double value of a specified key
     * @param key     column key to update
     * @param value   new double value of the key
     * @return success
     */
    inline bool updateFloat8(const std::string& key, double value)
    { return update()._pquerybuilder->keyFloat8(key, value); }

    /**
     * Sets a new double vector of a specified key
     * @param key      column key to update
     * @param values   new double vector of the key
     * @return success
     * @unimplemented
     */
    inline bool updateFloat8Vector(const std::string& key, const std::vector<double> &values)
    { return update()._pquerybuilder->keyFloat8Vector(key, values); }

    /**
     * Sets a new timestamp of a specified key
     * @param key   column key to update
     * @param value timestamp value
     * @return  success
     */
    inline bool updateTimestamp(const std::string& key, const std::chrono::system_clock::time_point &value)
    { return update()._pquerybuilder->keyTimestamp(key, value); }

    /**
     * Sets a new cv::mat of a specified key
     * @param key column key to update
     * @param value cv::mat value
     * @return success
     */
    inline bool updateCvMat(const std::string& key, const cv::Mat &value)
    { return update()._pquerybuilder->keyCvMat(key, value); }

    /**
     * Sets a new point of a specified key
     * @param key column key to update
     * @param value point value
     * @return success
     */
    inline bool updatePoint(const std::string& key, Point value)
    { return update()._pquerybuilder->keyPoint(key, value); }

    /**
     * Sets a new point vector of a specified key
     * @param key      column key to update
     * @param values   new point vector of the key
     * @return success
     * @unimplemented
     */
    inline bool updatePointVector(const std::string& key, const std::vector<Point> &values)
    { return update()._pquerybuilder->keyPointVector(key, values); }

    /**
     * Sets a new interval event of a specified key
     * @param key column key to update
     * @param value interval event value
     * @return success
     */
    inline bool updateIntervalEvent(const std::string& key, const IntervalEvent &value)
    { return update()._pquerybuilder->keyIntervalEvent(key, value); }

    /**
     * Sets a new process status of a specified key
     * @param key column key to update
     * @param value status value
     * @return success
     */
    inline bool updateProcessStatus(const std::string& key, ProcessState::Status value)
    { return update()._pquerybuilder->keyProcessStatus(key, value); }

    /**
     * Sets a new binary data of a specified key
     * @param key column key to update
     * @param value binary data value
     * @return success
     */
    inline bool updateBlob(const std::string &key, const std::vector<char> &value)
    { return update()._pquerybuilder->keyBlob(key, value); }

protected:
    Select _select; /**< Select object for select queries (usually pre-filled by the constructor) */

    Update & update();
    virtual bool preUpdate();

private:
    std::shared_ptr<Update> _pupdate; /**< Update object to update new data */

    KeyValues() = delete;
    KeyValues(const KeyValues& copy) = delete;
    KeyValues& operator=(const KeyValues&) = delete;

    //TODO: remove these friends
    friend class Process;
    friend class Task;
};

} // namespace vtapi
