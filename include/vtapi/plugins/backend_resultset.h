#pragma once

#include "../common/dbtypes.h"
#include "../common/tkey.h"
#include "../data/intervalevent.h"
#include "../data/processstate.h"
#include <string>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>

namespace vtapi {


/**
 * @brief Class provides interface to the result set object
 *
 * New result set is inserted via newResult method. Iterating over rows is handled
 * by incrementing pos attribute via step() or setPosition() methods. Field values
 * can be obtained via various getX methods.
 */
class ResultSet
{
public:
    /**
     * Destroy old result object and replace it with new one
     * @param res new result object
     */
    virtual void newResult(void *res) = 0;

    /**
     * Gets number of rows within result set
     * @return number of rows
     */
    virtual int countRows() const = 0;

    /**
     * Gets number of columns within result set
     * @return number of columns
     */
    virtual int countCols() const = 0;

    /**
     * Checks whether result set object is valid
     * @return success
     */
    virtual bool isOk() const = 0;

    /**
     * Clears result set object
     */
    virtual void clear() = 0;

   /**
     * Get key of a single table column
     * @param col Column index
     * @return Column key
     */
    virtual TKey getKey(int col) const = 0;

    /**
     * Get a vector of all possible columns as TKeys
     * @return vector of keys
     */
    virtual TKeys getKeys() const = 0;

    /**
     * Gets type of given column
     * @param col column index
     * @return type name
     */
    virtual std::string getKeyType(int col) const = 0;

    /**
     * Gets index of given column name
     * @param key column name
     * @return column index
     */
    virtual int getKeyIndex(const std::string& key) const = 0;

    // =============== VIRTUAL GETTERS ===============

     /**
     * Generic getter - fetches any value from resultset and returns it as string
     * @param col column index
     * @return string representation of field value
     */
    virtual std::string getValue(int col) const = 0;

    /**
     * Get a boolean value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    virtual bool getBool(int col) const = 0;

    /**
     * Get single character specified by column index
     * @param col column index
     * @return character
     */
    virtual char getChar(int col) const = 0;

    /**
     * Get a string value specified by an index of a column
     * @param col column index
     * @return string value
     */
    virtual std::string getString(int col) const = 0;

    /**
     * Get a vector of string values specified by an index of a column
     * @param col index of column
     * @return vector of string values
     */
    virtual std::vector<std::string> getStringVector(int col) const = 0;

    /**
     * Get an integer value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    virtual int getInt(int col) const = 0;

    /**
     * Get a vector of integer values specified by an index of a column
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<int> getIntVector(int col) const = 0;

    /**
     * Get long integer value specified by an index of a column
     * @param col index of column
     * @return long integer value
     */
    virtual long long getInt8(int col) const = 0;

    /**
     * Get an array of long integer values specified by an index of a column
     * @param col index of column
     * @return array of long integer values
     */
    virtual std::vector<long long> getInt8Vector(int col) const = 0;

    /**
     * Get a float value specified by an index of a column
     * @param col index of column
     * @return float value
     */
    virtual float getFloat(int col) const = 0;

    /**
     * Get a vector of float values specified by column key
     * @param col index of column
     * @return vector of float values
     */
    virtual std::vector<float> getFloatVector(int col) const = 0;

    /**
     * Get a double value specified by an index of a column
     * @param col index of column
     * @return double value
     */
    virtual double getFloat8(int col) const = 0;

    /**
     * Get array of double values specified by index of column
     * @param col index of column
     * @return vector of double values
     */
    virtual std::vector<double> getFloat8Vector(int col) const = 0;

    /**
     * Get timestamp specified by the column index
     * @param col column index
     * @return Timestamp info
     */
    virtual std::chrono::system_clock::time_point getTimestamp(int col) const = 0;

    /**
     * Get OpenCv matrix (cvMat) specified by the column index
     * @param col column index
     * @return CvMat structure
     */
    virtual cv::Mat *getCvMat(int col) const = 0;

    /**
     * Get 2D point specified by the column index
     * @param col column index
     * @return 2D Point
     */
    virtual Point getPoint(int col) const = 0;

    /**
     * Get vector of 2D points specified by the column key
     * @param col column key
     * @return vector of 2D Points
     */
    virtual std::vector<Point> getPointVector(int col) const = 0;

    /**
     * Get interval event by column index
     * @param col column index
     * @return interval event class
     */
    virtual IntervalEvent getIntervalEvent(int col) const = 0;

    /**
     * Gets process state by an index of a column
     * @param col   index of the column
     * @return process state class
     */
    virtual ProcessState getProcessState(int col) const = 0;

    /**
     * Gets binary data by a column index
     * @param col   column index
     * @return binary data
     */
    virtual std::vector<char> getBlob(int col) const = 0;


    // ////////////////////////////////////////////////////////////////////////
    // IMPLEMENTED METHODS
    // ////////////////////////////////////////////////////////////////////////


    /**
     * Constructor
     * @param dbtypes preloaded map of database types
     */
    explicit ResultSet(const DatabaseTypes &dbtypes)
        : _dbtypes(dbtypes), _pos(-1), _res(NULL) {}

    virtual ~ResultSet() {}

    /**
     * Gets current row position within result set
     * @return row position
     */
    inline int getPosition() const
    { return _pos; }

    /**
     * Sets row position within result set
     * @param pos row position
     */
    inline void setPosition(int pos)
    { _pos = pos; }

    /**
     * Increments row position within result set
     */
    inline void incPosition()
    { _pos++; }

    // =============== GETTERS by key ===============

    /**
     * @brief Gets value and return it as string
     * @param col column key
     * @return value
     */
    std::string getValue(const std::string& key) const
    { return this->getValue(this->getKeyIndex(key)); }

    /**
     * Get a boolean value specified by a column key
     * @param key column key
     * @return integer value
     */
    inline bool getBool(const std::string& key) const
    { return this->getBool(this->getKeyIndex(key)); }

    /**
     * Get single character specified by a column key
     * @param key column key
     * @return character
     */
    inline char getChar(const std::string& key) const
    { return this->getChar(this->getKeyIndex(key)); }

    /**
     * Get a character array specified by a column key
     * @param key column key
     * @return string
     */
    inline std::string getString(const std::string& key) const
    { return this->getString(this->getKeyIndex(key)); }

    /**
     * Get a vector of string values specified by an key of a column
     * @param key key of column
     * @return vector of string values
     */
    inline std::vector<std::string> getStringVector(const std::string &key) const
    { return this->getStringVector(this->getKeyIndex(key)); }

    /**
     * Get an integer value specified by an key of a column
     * @param key key of column
     * @return integer value
     */
    inline int getInt(const std::string &key) const
    { return this->getInt(this->getKeyIndex(key)); }

    /**
     * Get a vector of integer values specified by an key of a column
     * @param key key of column
     * @return vector of integer values
     */
    inline std::vector<int> getIntVector(const std::string &key) const
    { return this->getIntVector(this->getKeyIndex(key)); }

    /**
     * Get long integer value specified by an key of a column
     * @param key key of column
     * @return long integer value
     */
    inline long long getInt8(const std::string &key) const
    { return this->getInt8(this->getKeyIndex(key)); }

    /**
     * Get an array of long integer values specified by an key of a column
     * @param key key of column
     * @return array of long integer values
     */
    inline std::vector<long long> getInt8Vector(const std::string &key)
    { return this->getInt8Vector(this->getKeyIndex(key)); }

    /**
     * Get a float value specified by an key of a column
     * @param key key of column
     * @return float value
     */
    inline float getFloat(const std::string &key) const
    { return this->getFloat(this->getKeyIndex(key)); }

    /**
     * Get a vector of float values specified by column key
     * @param key key of column
     * @return vector of float values
     */
    inline std::vector<float> getFloatVector(const std::string &key) const
    { return this->getFloatVector(this->getKeyIndex(key)); }

    /**
     * Get a double value specified by an key of a column
     * @param key key of column
     * @return double value
     */
    inline double getFloat8(const std::string &key) const
    { return this->getFloat8(this->getKeyIndex(key)); }

    /**
     * Get array of double values specified by key of column
     * @param key key of column
     * @return vector of double values
     */
    inline std::vector<double> getFloat8Vector(const std::string &key)
    { return this->getFloat8Vector(this->getKeyIndex(key)); }

    /**
     * Get timestamp specified by the column key
     * @param key column key
     * @return Timestamp info
     */
    inline std::chrono::system_clock::time_point getTimestamp(const std::string &key) const
    { return this->getTimestamp(this->getKeyIndex(key)); }

    /**
     * Get OpenCv matrix (cvMat) specified by the column key
     * @param key column key
     * @return CvMat structure
     */
    inline cv::Mat *getCvMat(const std::string &key) const
    { return this->getCvMat(this->getKeyIndex(key)); }

    /**
     * Get 2D point specified by the column key
     * @param key column key
     * @return 2D Point
     */
    inline Point getPoint(const std::string &key) const
    { return this->getPoint(this->getKeyIndex(key)); }

    /**
     * Get vector of 2D points specified by the column key
     * @param key column key
     * @return vector of 2D Points
     */
    inline std::vector<Point> getPointVector(const std::string &key) const
    { return this->getPointVector(this->getKeyIndex(key)); }

    /**
     * Get interval event by column key
     * @param key column key
     * @return interval event class
     */
    inline IntervalEvent getIntervalEvent(const std::string &key) const
    { return this->getIntervalEvent(this->getKeyIndex(key)); }

    /**
     * Gets process state by an key of a column
     * @param key   key of the column
     * @return process state class
     */
    inline ProcessState getProcessState(const std::string &key) const
    { return this->getProcessState(this->getKeyIndex(key)); }

    /**
     * Gets binary data by a column key
     * @param key   column key
     * @return binary data
     */
    inline std::vector<char> getBlob(const std::string &key)
    { return this->getBlob(this->getKeyIndex(key)); }

protected:
    const DatabaseTypes &_dbtypes;  /**< map of database types definitions */
    int _pos;                       /**< position within resultset */
    void *_res;                     /**< result object */

private:
    ResultSet() = delete;
    ResultSet(const ResultSet&) = delete;
    ResultSet & operator=(const ResultSet&) = delete;
};


} // namespace vtapi
