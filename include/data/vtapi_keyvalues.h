/**
 * @file
 * @brief   Declaration of Keyvalues class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */


#ifndef VTAPI_KEYVALUES_H
#define	VTAPI_KEYVALUES_H

#include "vtapi_commons.h"
#include "vtapi_intervalevent.h"
#include "vtapi_processstate.h"
#include "../queries/vtapi_select.h"
#include "../queries/vtapi_insert.h"
#include "../queries/vtapi_update.h"

namespace vtapi {

// ************************************************************************** //
/**
 * @brief KeyValues storage class
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 30*
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class KeyValues : public Commons {
public:

    Select* select; /**< Select for select queries (depending on child KeyValue object may be pre-filled by the constructor) */
    Insert* insert; /**< Insert to insert new data */
    Update* update; /**< Update to update new data */

    std::list<Insert*> store;  /**< insert queries stored for transactional execute */

public:
    /**
     * KeyValue contructor from Commons object
     * @param orig   pointer of the existing Commons object
     */
    KeyValues(const Commons& orig);

    /**
     * KeyValue constructor from another KeyValues object
     * @param orig        pointer to the parent KeyValues object
     * @param selection   specific selection name
     */
    KeyValues(const KeyValues& orig, const std::string& selection = "");

    /**
     * This destroys the KeyValues
     * It raises warning when there was something left not-excecuted (some collaborants left)
     */
    ~KeyValues();

    /**
     * The most used function of the VTApi - next (row)
     * @return this or null
     */
    KeyValues* next();

    /**
     * Count total number of represented objects
     * @return object count, -1 for error
     */
    int count();
    /**
     * Gets key of a single table column
     * @param col   column index
     * @return Column key
     */
    TKey getKey(int col);
    /**
     * Gets a list of all possible columns as TKeys
     * @return list of keys
     */
    TKeys* getKeys();

    /**
     * Prints a current tuple of resultset
     * @return success
     */
    bool print();
    /**
     * Prints all tuples of resultset
     * @return success
     */
    bool printAll();


    
    // =============== GETTERS (Select) ========================================

     /**
     * Generic getter - fetches any value from resultset and returns it as string
     * @param col   index of a column
     * @return string representation of field value
     */
    std::string getValue(const int col, const int arrayLimit = 0);

    
    // =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===============
    /**
     * Gets a single character specified by a column key
     * @param key   column key
     * @return character
     */
    char getChar(const std::string& key);
    /**
     * Gets a single character specified by an index of a column
     * @param pos   index of the column
     * @return character
     */
    char getChar(const int pos);   
    /**
     * Gets a character array specified by a column key
     * @param key   column key
     * @return character array
     */
    std::string getString(const std::string& key);
    /**
     * Gets a string value specified by an index of a column
     * @param col   index of the column
     * @return string value
     */
    std::string getString(const int col);

    
    // =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==============
    /**
     * Gets a boolean value specified by a column key
     * @param key   column key
     * @return integer value
     */
    bool getBool(const std::string& key);
    /**
     * Gets a boolean value specified by an indef of a column
     * @param key   column key
     * @return integer value
     */
    bool getBool(const int col);
    /**
     * Gets an integer value specified by a column key
     * @param key   column key
     * @return integer value
     */
    int getInt(const std::string& key);
    /**
     * Gets an integer value specified by an index of a column
     * @param col   index of the column
     * @return integer value
     */
    int getInt(const int col);
    
    /**
     * Gets a long integer value specified by a column key
     * @param key   column key
     * @return long integer value
     */
    long long getInt8(const std::string& key);
    /**
     * Gets a long integer value specified by an index of a column
     * @param col   index of the column
     * @return long integer value
     */
    long long getInt8(const int col);
    
    /**
     * Gets an array of integer values specified by a column key
     * @param key    column key
     * @param size   size of the array of integer values
     * @return array of integer values
     */
    int* getIntA(const std::string& key, int& size);
    /**
     * Gets an array of integer values specified by an index of a column
     * @param col    index of the column
     * @param size   size of the array of integer values
     * @return array of integer values
     */
    int* getIntA(const int col, int& size);

    /**
     * Gets a vector of integer values specified by a column key
     * @param key   column key
     * @return vector of integer values
     */
    std::vector<int>* getIntV(const std::string& key);
    /**
     * Gets a vector of integer values specified by an index of a column
     * @param col   index of the column
     * @return vector of integer values
     */
    std::vector<int>* getIntV(const int col);
    
    /**
     * Gets an array of long integer values specified by a column key
     * @param key    column key
     * @param size   size of the array of integer values
     * @return array of integer values
     */
    long long* getInt8A(const std::string& key, int& size);
    /**
     * Gets an array of long integer values specified by an index of a column
     * @param col    index of the column
     * @param size   size of the array of integer values
     * @return array of integer values
     */
    long long* getInt8A(const int col, int& size);

    /**
     * Gets a vector of long integer values specified by a column key
     * @param key   column key
     * @return vector of integer values
     */
    std::vector<long long>* getInt8V(const std::string& key);
    /**
     * Gets a vector of long integer values specified by an index of a column
     * @param col   index of the column
     * @return vector of integer values
     */
    std::vector<long long>* getInt8V(const int col);
    
    // =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ==================
    /**
     * Gets a float value specified by a column key
     * @param key   column key
     * @return float value
     */
    float getFloat(const std::string& key);
    /**
     * Gets a float value specified by an index of a column
     * @param col   index of the column
     * @return float value
     */
    float getFloat(const int col);
    
    /**
     * Gets a double value specified by a column key
     * @param key   column key
     * @return double value
     */
    double getFloat8(const std::string& key);
    /**
     * Gets a double value specified by an index of a column
     * @param col   index of the column
     * @return double value
     */
    double getFloat8(const int col);
    
    /**
     * Gets an array of float values specified by a column key
     * @param key    column key
     * @param size   size of the array of float values
     * @return array of float values
     */
    float* getFloatA(const std::string& key, int& size);
    /**
     * Gets an array of float values specified by an index of a column
     * @param col    index of the column
     * @param size   size of the array of float values
     * @return array of float values
     */
    float* getFloatA(const int col, int& size);
    
    /**
     * Gets a vector of float values specified by an index of a column
     * @param key   column key
     * @return vector of float values
     */
    std::vector<float>* getFloatV(const std::string& key);
    /**
     * Gets a vector of float values specified by a column key
     * @param col   index of the column
     * @return vector of integer values
     */
    std::vector<float>* getFloatV(const int col);
    /**
     * Gets an array of double values specified by a column key
     * @param key    column key
     * @param size   size of the array of float values
     * @return array of float values
     */
    double* getFloat8A(const std::string& key, int& size);
    /**
     * Gets an array of double values specified by an index of a column
     * @param col    index of the column
     * @param size   size of the array of float values
     * @return array of float values
     */
    double* getFloat8A(const int col, int& size);

    /**
     * Gets a vector of double values specified by an index of a column
     * @param key   column key
     * @return vector of float values
     */
    std::vector<double>* getFloat8V(const std::string& key);
    /**
     * Gets a vector of double values specified by a column key
     * @param col   index of the column
     * @return vector of integer values
     */
    std::vector<double>* getFloat8V(const int col);

    // =============== GETTERS - TIMESTAMP =====================================
    /**
     * Gets a timestamp specified by a column key
     * @param key   column key
     * @return Timestamp info
     */
    time_t getTimestamp(const std::string& key);
    /**
     * Gets a timestamp specified by an index of a column
     * @param col   index of the column
     * @return Timestamp info
     */
    time_t getTimestamp(const int col);

    
    // =============== GETTERS - OpenCV MATRICES ===============================
#if HAVE_OPENCV
    /**
     * Gets an OpenCV matrix (cv::Mat) specified by a column key
     * @param key   column key
     * @return CvMat structure
     */
    cv::Mat *getCvMat(const std::string& key);
    /**
     * Gets an OpenCV matrix (cv::Mat) specified by an index of a column
     * @param col   index of the column
     * @return CvMat structure
     */
    cv::Mat *getCvMat(const int col);
#endif

    
    // =============== GETTERS - GEOMETRIC TYPES ===============================
    // TODO: geometricke typy
#if HAVE_POSTGRESQL
    /**
     * Gets a 2D point specified by a column key
     * @param key   column key
     * @return 2D Point
     */
    PGpoint getPoint(const std::string& key);
    /**
     * Gets a 2D point specified by an index of a column
     * @param col   index of the column
     * @return 2D Point
     */
    PGpoint getPoint(const int col);

    /**
     * Gets an array of 2D points specified by a column key
     * @param key   column key
     * @return vector of 2D Points
     */
    PGpoint *getPointA(const std::string& key, int& size);
    /**
     * Gets an array of 2D points specified by an index of a column
     * @param col   index of the column
     * @return vector of 2D Points
     */
    PGpoint *getPointA(const int col, int& size);
    /**
     * Gets a vector of 2D points specified by a column key
     * @param key   column key
     * @return vector of 2D Points
     */
    std::vector<PGpoint>* getPointV(const std::string& key);
    /**
     * Gets a vector of 2D points specified by an index of a column
     * @param col   index of the column
     * @return vector of 2D Points
     */
    std::vector<PGpoint>* getPointV(const int col);
#endif

//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a line segment specified by a column key
//     * @param key   column key
//     * @return Line segment
//     */
//    PGlseg getLineSegment(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a line segment specified by an index of a column
//     * @param col   index of the column
//     * @return Line segment
//     */
//    PGlseg getLineSegment(const int col);
//
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a box specified by a column key
//     * @param key   column key
//     * @return Box
//     */
//    PGbox getBox(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a box specified by an index of a column
//     * @param col   index of the column
//     * @return Box
//     */
//    PGbox getBox(const int col);
//
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a circle specified by a column key
//     * @param key   column key
//     * @return Circle
//     */
//    PGcircle getCircle(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a circle specified by an index of a column
//     * @param col   index of the column
//     * @return Circle
//     */
//    PGcircle getCircle(const int col);
//
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a polygon specified by ta column key
//     * @note polygon.pts must be copied out if needed after clearing resultset
//     *          copy_points(polygon.npts, polygon.pts, ...);
//     * @param key   column key
//     * @return Polygon
//     */
//    PGpolygon getPolygon(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a polygon specified by an index of a column
//     * @note polygon.pts must be copied out if needed after clearing resultset
//     *          copy_points(polygon.npts, polygon.pts, ...);
//     * @param col   index of the column
//     * @return Polygon
//     */
//    PGpolygon getPolygon(const int col);
//
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a path specified by a column key
//     * @note path.pts must be copied out if needed after clearing resultset
//     *          copy_points(path.npts, path.pts, ...);
//     * @param key   column key
//     * @return Path
//     */
//    PGpath getPath(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Gets a path specified by an index of a column
//     * @note path.pts must be copied out if needed after clearing resultset
//     *          copy_points(path.npts, path.pts, ...);
//     * @param col   index of the column
//     * @return Path
//     */
//    PGpath getPath(const int col);

#if HAVE_POSTGIS
    /**
     * Gets a GEOS geometry type by a column key
     * @param key   column key
     * @return GEOS geometry
     */
    GEOSGeometry* getGeometry(const std::string& key);
    /**
     * Gets a GEOS geometry type by an index of a column
     * @param col   index of the column
     * @return GEOS geometry
     */
    GEOSGeometry* getGeometry(const int col);
    
    /**
     * Gets a GEOS geometry (linestring) type by a column key
     * @param key   column key
     * @return GEOS geometry
     */
    GEOSGeometry* getLineString(const std::string& key);
    /**
     * Gets a GEOS geometry (linestring) type by an index of a column
     * @param col   index of the column
     * @return GEOS geometry
     */
    GEOSGeometry* getLineString(const int col);
#endif

    // =============== GETTERS - CUSTOM COMPOSITE TYPES ========================
    /**
     * Gets interval event by a column key
     * @param key   column key
     * @return interval event class
     */
    IntervalEvent *getIntervalEvent(const std::string& key);
    /**
     * Gets interval event by an index of a column
     * @param col   index of the column
     * @return interval event class
     */
    IntervalEvent *getIntervalEvent(const int col);
    /**
     * Gets process state by a column key
     * @param key   column key
     * @return process state class
     */
    ProcessState *getProcessState(const std::string& key);
    /**
     * Gets process state by an index of a column
     * @param col   index of the column
     * @return process state class
     */
    ProcessState *getProcessState(const int col);
    
    // =============== GETTERS - OTHER =========================================
    /**
     * Gets binary data by a column key
     * @param key   column key
     * @param size size of output data
     * @return allocated data
     */
    void *getBlob(const std::string& key, int &size);
    /**
     * Gets binary data by a column index
     * @param col   column index
     * @param size size of output data
     * @return allocated data
     */
    void *getBlob(const int col, int &size);

    // =============== SETTERS (Update) ========================================
    /**
     * This is to support updates in derived classes
     * (unimplemented error 3010 in this class)
     * @return success (in derived classes)
     */
    virtual bool preSet();


    /**
     * Sets a new string value of a specified key
     * @param key     column key to update
     * @param value   new string value of the key
     * @return success
     */
    bool setString(const std::string& key, const std::string& value);

    /**
     * Sets a new integer value of a specified key
     * @note New integer value is casted from string value
     * @param key     column key to update
     * @param value   new integer value (in string representation) of the key
     * @return success
     * @unimplemented
     */
    bool setInt(const std::string& key, const std::string& value);
    /**
     * Sets a new integer value of a specified key
     * @param key     column key to update
     * @param value   new integer value of the key
     * @return success
     */
    bool setInt(const std::string& key, int value);

    /**
     * Sets a new integer array of a specified key
     * @param key      column key to update
     * @param values   new integer array of the key
     * @param size     size of the array of integer values
     * @return success
     */
    bool setIntA(const std::string& key, int* values, int size);
    /**
     * Sets a new integer vector of a specified key
     * @param key      column key to update
     * @param values   new integer vector of the key
     * @return success
     * @unimplemented
     */
    bool setIntV(const std::string& key, const std::vector<int> values);

    /**
     * Sets a new float value of a specified key
     * @note New flaot value is casted from string value
     * @param key     column key to update
     * @param value   new float value (in string representation) of the key
     * @return success
     * @unimplemented
     */
    bool setFloat(const std::string& key, const std::string& value);
    /**
     * Sets a new float value of a specified key
     * @param key     column key to update
     * @param value   new float value of the key
     * @return success
     */
    bool setFloat(const std::string& key, float value);

    /**
     * Sets a new float array of a specified key
     * @param key      column key to update
     * @param values   new float array of the key
     * @param size     size of the array of integer values
     * @return success
     */
    bool setFloatA(const std::string& key, float* values, int size);
    /**
     * Sets a new float vector of a specified key
     * @param key      column key to update
     * @param values   new float vector of the key
     * @return success
     * @unimplemented
     */
    bool setFloatV(const std::string& key, const std::vector<float> values);
    /**
     * Sets a new timestamp of a specified key
     * @param key   column key to update
     * @param value timestamp value
     * @return  success
     */
    bool setTimestamp(const std::string& key, const time_t& value);
    /**
     * Sets a new process status of a specified key
     * @param key column key to update
     * @param value status value
     * @return success
     */
    bool setPStatus(const std::string& key, ProcessState::STATUS_T value);
    /**
     * Executes SQL UPDATE command
     * @return success
     */
    bool setExecute();



    // =============== ADDERS (Insert) ========================================

    /**
     * Adds a new string to a specified key
     * @param key     column key to insert
     * @param value   new string value of the key
     * @return success
     */
    bool addString(const std::string& key, const std::string& value);
    
    /**
     * Adds a new integer value to a specified key
     * @param key     column key to insert
     * @param value   new integer value of the key
     * @return success
     */
    bool addInt(const std::string& key, int value);
    /**
     * Adds a new integer array to a specified key
     * @param key     column key to insert
     * @param value   new integer array of the key
     * @param size    size of the array of integer values
     * @return success
     */
    bool addIntA(const std::string& key, int* value, int size);
    
    /**
     * Adds a new float value to a specified key
     * @param key     column key to insert
     * @param value   new float value of the key
     * @return success
     */
    bool addFloat(const std::string& key, float value);
    /**
     * Adds a new float array to a specified key
     * @param key     column key to insert
     * @param value   new float array of the key
     * @param size    size of the array of float values
     * @return success
     */
    bool addFloatA(const std::string& key, float* value, int size);
    /**
     * Adds a new timestamp to a specified key
     * @param key       column key to insert
     * @param value     new timestamp
     * @return success
     */
    bool addTimestamp(const std::string& key, const time_t& value);
    
#if HAVE_OPENCV
    /**
     * Adds a new OpenCV matrix (cv::Mat) to a specified key
     * @param key     column key to insert
     * @param value   new OpenCV matrix (cv::Mat) of the key
     * @return success
     */
    bool addCvMat(const std::string& key, cv::Mat& value);
#endif
    
    /**
     * Adds a new IntervalEvent value to a specied key
     * @param key     column key to insert
     * @param value   new IntervalEvent value of the key
     * @return success
     */
    bool addIntervalEvent(const std::string& key, IntervalEvent& value);
    
    /**
     * Executes SQL INSERT command
     * @return success
     */
    bool addExecute();

    /**
     * This is used to check whether the underlying dataset, sequence (directory / video) or interval (image) exists
     * @return success of found
     */
    bool checkStorage();

    /**
     * This goes through resultset and retrieves metadata necessary for print
     * @note It needs to be done before every print.
     * @param row if       not set to -1, this indicates single row print
     * @param get_widths   whether column widths will be required
     * @return metadata for print, pair consisting of two vectors:
     *  -#) Tkeys - column types etc.
     *  -#) ints - column widths
     * 
     * @unimplemented Is this method needed? It seems that was implemented and afterwards was deleted (or renamed - prinRowOnly()? )...
     * @todo Is this method needed? It seems that was implemented and afterwards was deleted (or renamed - printRowOnly()? )...
     */
    std::pair< TKeys*,std::vector<int>* > getFieldsInfo(const int row, int get_widths);

protected:
    std::string caption; /**< table caption */
    std::string tableOpt; /**< custom table options (border, padding, whatever..) */

    ////////////// Print support methods

    /**
     * Prints table header
     * @param fInfo   Metadata for print, pair consisting of two vectors:
     *  -# Tkeys - column types etc.
     *  -# ints - column widths
     * @return success
     */
    bool printHeader(const std::pair< TKeys*,std::vector<int>* > &fInfo);

    /**
     * Prints one row of resultset
     * @param row      Which row of resultset to print
     * @param widths   Metadata - vector of column widths
     * @return success
     */
    bool printRowOnly(const int row, const std::vector<int>* widths);

    /**
     * Prints table footer and info about printed rows
     * @param count   How many rows were printed (0 = single row was printed)
     * @return success
     */
    bool printFooter(const int count);

};

} // namespace vtapi

#endif	/* VTAPI_KEYVALUES_H */
