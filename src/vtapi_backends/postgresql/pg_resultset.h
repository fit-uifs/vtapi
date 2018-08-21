#pragma once

#include <vtapi/plugins/backend_resultset.h>

namespace vtapi {


class PGResultSet : public ResultSet
{
public:
    // ////////////////////////////////////////////////////////////////////////
    // INTERFACE IMPLEMENTATION
    // ////////////////////////////////////////////////////////////////////////


    /**
     * Destroy old result object and replace it with new one
     * @param res new result object
     */
    void newResult(void *res) override;

    /**
     * Gets number of rows within result set
     * @return number of rows
     */
    int countRows() const override;

    /**
     * Gets number of columns within result set
     * @return number of columns
     */
    int countCols() const override;

    /**
     * Checks whether result set object is valid
     * @return success
     */
    bool isOk() const override;

    /**
     * Clears result set object
     */
    void clear() override;

   /**
     * Get key of a single table column
     * @param col Column index
     * @return Column key
     */
    TKey getKey(int col) const override;

    /**
     * Get a vector of all possible columns as TKeys
     * @return vector of keys
     */
    TKeys getKeys() const override;

    /**
     * Gets type of given column
     * @param col column index
     * @return type name
     */
    std::string getKeyType(int col) const override;

    /**
     * Gets index of given column name
     * @param key column name
     * @return column index
     */
    int getKeyIndex(const std::string& key) const override;

    // =============== GETTERS ===============

     /**
     * Generic getter - fetches any value from resultset and returns it as string
     * @param col column index
     * @return string representation of field value
     */
    std::string getValue(int col) const override;

    /**
     * Get a boolean value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    bool getBool(int col) const override;

    /**
     * Get single character specified by column index
     * @param col column index
     * @return character
     */
    char getChar(int col) const override;

    /**
     * Get a string value specified by an index of a column
     * @param col column index
     * @return string value
     */
    std::string getString(int col) const override;

    /**
     * Get a vector of string values specified by an index of a column
     * @param col index of column
     * @return vector of string values
     */
    std::vector<std::string> getStringVector(int col) const override;

    /**
     * Get an integer value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    int getInt(int col) const override;

    /**
     * Get a vector of integer values specified by an index of a column
     * @param col index of column
     * @return vector of integer values
     */
    std::vector<int> getIntVector(int col) const override;

    /**
     * Get long integer value specified by an index of a column
     * @param col index of column
     * @return long integer value
     */
    long long getInt8(int col) const override;

    /**
     * Get an array of long integer values specified by an index of a column
     * @param col index of column
     * @return array of long integer values
     */
    std::vector<long long> getInt8Vector(int col) const override;

    /**
     * Get a float value specified by an index of a column
     * @param col index of column
     * @return float value
     */
    float getFloat(int col) const override;

    /**
     * Get a vector of float values specified by column key
     * @param col index of column
     * @return vector of float values
     */
    std::vector<float> getFloatVector(int col) const override;

    /**
     * Get a double value specified by an index of a column
     * @param col index of column
     * @return double value
     */
    double getFloat8(int col) const override;

    /**
     * Get array of double values specified by index of column
     * @param col index of column
     * @return vector of double values
     */
    std::vector<double> getFloat8Vector(int col) const override;

    /**
     * Get timestamp specified by the column index
     * @param col column index
     * @return Timestamp info
     */
    std::chrono::system_clock::time_point getTimestamp(int col) const override;

    /**
     * Get OpenCv matrix (cvMat) specified by the column index
     * @param col column index
     * @return CvMat structure
     */
    cv::Mat getCvMat(int col) const override;

    /**
     * Get 2D point specified by the column index
     * @param col column index
     * @return 2D Point
     */
    Point getPoint(int col) const override;

    /**
     * Get vector of 2D points specified by the column key
     * @param col column key
     * @return vector of 2D Points
     */
    std::vector<Point> getPointVector(int col) const override;

    /**
     * Get interval event by column index
     * @param col column index
     * @return interval event class
     */
    IntervalEvent getIntervalEvent(int col) const override;

    /**
     * Get EdfDescriptor by column index
     * @param col column index
     * @return EdfDescriptor class
     */
    EyedeaEdfDescriptor getEdfDescriptor(int col) const override;

    /**
     * Gets process state by an index of a column
     * @param col   index of the column
     * @return process state class
     */
    ProcessState getProcessState(int col) const override;

    /**
     * Gets binary data by a column index
     * @param col   column index
     * @return binary data
     */
    std::vector<char> getBlob(int col) const override;


    // ////////////////////////////////////////////////////////////////////////
    // OWN IMPLEMENTATION
    // ////////////////////////////////////////////////////////////////////////


    explicit PGResultSet(const DatabaseTypes &dbtypes)
        : ResultSet(dbtypes) {}

    ~PGResultSet()
    { clear(); }

private:
    short getKeyTypeLength(int col, short def) const;

    template <typename T>
    T getIntegerSingle(int col) const;

    template <typename T>
    std::vector<T> getIntegerVector(int col) const;

    template <typename T>
    T getFloatSingle(int col) const;

    template <typename T>
    std::vector<T> getFloatVector(int col) const;
};

}
