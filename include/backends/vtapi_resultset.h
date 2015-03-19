/* 
 * File:   vtapi_resultset.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:40 PM
 */

#ifndef VTAPI_RESULTSET_H
#define	VTAPI_RESULTSET_H

#include "vtapi_backendbase.h"
#include "../common/vtapi_types.h"
#include "../common/vtapi_tkeyvalue.h"
#include "../data/vtapi_intervalevent.h"

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
protected:
    DBTYPES_MAP *dbtypes;     /**< map of database types definitions */
    int             pos;            /**< position within resultset */
    void            *res;           /**< result object */

public:

    /**
     * Constructor
     * @param base base object (backend interface, logging...)
     * @param dbtypes preloaded map of database types
     */
    ResultSet(DBTYPES_MAP *dbtypes) {
        this->dbtypes       = dbtypes;
        this->pos           = -1;
        this->res           = NULL;
    };
    /**
     * Virtual destructor
     */
    virtual ~ResultSet() { };

    /**
     * Destroy old result object and replace it with new one
     * @param res new result object
     */
    virtual void newResult(void *res) = 0;

    /**
     * Increments row position within result set
     */
    void step() { this->pos++; };
    /**
     * Gets current row position within result set
     * @return row position
     */
    int getPosition() { return this->pos; };
    /**
     * Sets row position within result set
     * @param pos row position
     */
    void setPosition(const int pos) { this->pos = pos; };

    /**
     * Gets number of rows within result set
     * @return number of rows
     */
    virtual int countRows() = 0;
    /**
     * Gets number of columns within result set
     * @return number of columns
     */
    virtual int countCols() = 0;
    /**
     * Checks whether result set object is valid
     * @return success
     */
    virtual bool isOk() = 0;
    /**
     * Clears result set object
     */
    virtual void clear() = 0;

   /**
     * Get key of a single table column
     * @param col Column index
     * @return Column key
     */
    virtual TKey getKey(int col) = 0;
    /**
     * Get a list of all possible columns as TKeys
     * @return list of keys
     */
    virtual TKeys *getKeys() = 0;

    // =============== GETTERS (Select) ========================================

     /**
     * Generic getter - fetches any value from resultset and returns it as string
     * @param col column index
     * @param arrayLimit array length to print
     * @return string representation of field value
     */
    virtual std::string getValue(const int col, const int arrayLimit = 0) = 0;

    // =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===============
    /**
     * Get single character specified by a column key
     * @param key column key
     * @return character
     */
    char getChar(const std::string& key) {
        return this->getChar(this->getKeyIndex(key));
    }
    /**
     * Get single character specified by column index
     * @param col column index
     * @return character
     */
    virtual char getChar(const int col) = 0;
    /**
     * Get a character array specified by a column key
     * @param key column key
     * @return character array
     */
    std::string getString(const std::string& key) {
        return this->getString(this->getKeyIndex(key));
    };
    /**
     * Get a string value specified by an index of a column
     * @param col column index
     * @return string value
     */
    virtual std::string getString(const int col) = 0;

    // =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==============
    /**
     * Get a boolean value specified by a column key
     * @param key column key
     * @return integer value
     */
    bool getBool(const std::string& key) {
        return this->getBool(this->getKeyIndex(key));
    };
    /**
     * Get a boolean value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    virtual bool getBool(const int col) = 0;
    /**
     * Get an integer value specified by a column key
     * @param key column key
     * @return integer value
     */
    int getInt(const std::string& key) {
        return this->getInt(this->getKeyIndex(key));
    };
    /**
     * Get an integer value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    virtual int getInt(const int col) = 0;
    /**
     * Get long integer value specified by a column key
     * @param key column key
     * @return long integer value
     */
    long long getInt8(const std::string& key) {
        return this->getInt8(this->getKeyIndex(key));
    };
    /**
     * Get long integer value specified by an index of a column
     * @param col index of column
     * @return long integer value
     */
    virtual long long getInt8(const int col) = 0;
    /**
     * Get an array of integer values specified by a column key
     * @param key column key
     * @param size size of the array of integer values
     * @return array of integer values
     */
    int *getIntA(const std::string& key, int& size) {
        return this->getIntA(this->getKeyIndex(key), size);
    };
    /**
     * Get an array of integer values specified by an index of a column
     * @param col index of column
     * @param size size of the array of integer values
     * @return array of integer values
     */
    virtual int *getIntA(const int col, int& size) = 0;
    /**
     * Get a vector of integer values specified by a column key
     * @param key column key
     * @return vector of integer values
     */
    std::vector<int> *getIntV(const std::string& key) {
        return this->getIntV(this->getKeyIndex(key));
    };
    /**
     * Get a vector of integer values specified by an index of a column
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<int> *getIntV(const int col) = 0;
    /**
     * Get an array of long integer values specified by a column key
     * @param key column key
     * @param size size of the array of integer values
     * @return array of integer values
     */
    long long *getInt8A(const std::string& key, int& size) {
        return this->getInt8A(this->getKeyIndex(key), size);
    };
    /**
     * Get an array of long integer values specified by an index of a column
     * @param col index of column
     * @param size size of the array of integer values
     * @return array of integer values
     */
    virtual long long *getInt8A(const int col, int& size) = 0;
    /**
     * Get a vector of long integer values specified by a column key
     * @param key column key
     * @return vector of integer values
     */
    std::vector<long long> *getInt8V(const std::string& key) {
        return this->getInt8V(this->getKeyIndex(key));
    };
    /**
     * Get a vector of long integer values specified by an index of a column
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<long long> *getInt8V(const int col) = 0;

    // =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ==================
    /**
     * Get a float value specified by a column key
     * @param key column key
     * @return float value
     */
    float getFloat(const std::string& key) {
        return this->getFloat(this->getKeyIndex(key));
    };
    /**
     * Get a float value specified by an index of a column
     * @param col index of column
     * @return float value
     */
    virtual float getFloat(const int col) = 0;
    /**
     * Get a double value specified by a column key
     * @param key column key
     * @return double value
     */
    double getFloat8(const std::string& key) {
        return this->getFloat8(this->getKeyIndex(key));
    };
    /**
     * Get a double value specified by an index of a column
     * @param col index of column
     * @return double value
     */
    virtual double getFloat8(const int col) = 0;
    /**
     * Get an array of float values specified by a column key
     * @param key column key
     * @param size size of the array of float values
     * @return array of float values
     */
    float* getFloatA(const std::string& key, int& size) {
        return this->getFloatA(this->getKeyIndex(key), size);
    };
    /**
     * Get array of float values specified by index of column
     * @param col index of column
     * @param size size of the array of float values
     * @return array of float values
     */
    virtual float *getFloatA(const int col, int& size) = 0;
    /**
     * Get a vector of float values specified by the column index
     * @param key column key
     * @return vector of float values
     */
    std::vector<float> *getFloatV(const std::string& key) {
        return this->getFloatV(this->getKeyIndex(key));
    };
    /**
     * Get a vector of float values specified by column key
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<float> *getFloatV(const int col) = 0;

    /**
     * Get an array of double values specified by a column key
     * @param key column key
     * @param size size of the array of float values
     * @return array of float values
     */
    double* getFloat8A(const std::string& key, int& size)
    {
        return this->getFloat8A(this->getKeyIndex(key), size);
    };
    /**
     * Get array of double values specified by index of column
     * @param col index of column
     * @param size size of the array of float values
     * @return array of float values
     */
    virtual double *getFloat8A(const int col, int& size) = 0;

    /**
     * Get a vector of double values specified by the column index
     * @param key column key
     * @return vector of float values
     */
    std::vector<double> *getFloat8V(const std::string& key)
    {
        return this->getFloat8V(this->getKeyIndex(key));
    };
    /**
     * Get a vector of double values specified by column key
     * @param col index of column
     * @return vector of integer values
     */
    virtual std::vector<double> *getFloat8V(const int col) = 0;

    //TODO: is it needed a vector of float vectors as in case of integers?

    // =============== GETTERS - TIMESTAMP =====================================
    /**
     * Get timestamp specified by column key
     * @param key column key
     * @return Timestamp info
     */
    time_t getTimestamp(const std::string& key) {
        return this->getTimestamp(this->getKeyIndex(key));
    };
    /**
     * Get timestamp specified by the column index
     * @param col column index
     * @return Timestamp info
     */
    virtual time_t getTimestamp(const int col) = 0;

    // =============== GETTERS - OpenCV MATRICES ===============================
#if HAVE_OPENCV
    /**
     * Get OpenCv matrix (cv::Mat) specified by the column key
     * @param key column key
     * @return CvMat structure
     */
    cv::Mat *getCvMat(const std::string& key) {
        return this->getCvMat(this->getKeyIndex(key));
    }
    /**
     * Get OpenCv matrix (cvMat) specified by the column index
     * @param col column index
     * @return CvMat structure
     */
    virtual cv::Mat *getCvMat(const int col) = 0;
#endif

// =============== GETTERS - GEOMETRIC TYPES ===============================
    
#if HAVE_POSTGRESQL
    /**
     * Get 2D point specified by the column key
     * @param key column key
     * @return 2D Point
     */
    PGpoint getPoint(const std::string& key) {
        return this->getPoint(this->getKeyIndex(key));
    };
    /**
     * Get 2D point specified by the column index
     * @param col column index
     * @return 2D Point
     */
    virtual PGpoint getPoint(const int col) = 0;
    /**
     * Get array of 2D points specified by the column index
     * @param key column index
     * @return vector of 2D Points
     */
    PGpoint *getPointA(const std::string& key, int& size) {
        return this->getPointA(this->getKeyIndex(key), size);
    };
    /**
     * Get array of 2D points specified by the column key
     * @param col column key
     * @return vector of 2D Points
     */
    virtual PGpoint *getPointA(const int col, int& size) = 0;
    /**
     * Get vector of 2D points specified by the column index
     * @param key column index
     * @return vector of 2D Points
     */
    std::vector<PGpoint>*  getPointV(const std::string& key) {
        return this->getPointV(this->getKeyIndex(key));
    };
    /**
     * Get vector of 2D points specified by the column key
     * @param col column key
     * @return vector of 2D Points
     */
    virtual std::vector<PGpoint>*  getPointV(const int col) = 0;
#endif

//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get line segment specified by the column key
//     * @param key column key
//     * @return Line segment
//     */
//    PGlseg getLineSegment(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get line segment specified by the column index
//     * @param col column index
//     * @return Line segment
//     */
//    PGlseg getLineSegment(const int col);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get box specified by the column key
//     * @param key column key
//     * @return Box
//     */
//    PGbox getBox(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get box specified by the column index
//     * @param col column index
//     * @return Box
//     */
//    PGbox getBox(const int col);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get circle specified by the column key
//     * @param key column key
//     * @return Circle
//     */
//    PGcircle getCircle(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get circle specified by the column index
//     * @param col column index
//     * @return Circle
//     */
//    PGcircle getCircle(const int col);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get polygon specified by the column key
//     * @note polygon.pts must be copied out if needed after clearing resultset
//     *          copy_points(polygon.npts, polygon.pts, ...);
//     * @param key column key
//     * @return Polygon
//     */
//    PGpolygon getPolygon(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get polygon specified by the column index
//     * @note polygon.pts must be copied out if needed after clearing resultset
//     *          copy_points(polygon.npts, polygon.pts, ...);
//     * @param col column index
//     * @return Polygon
//     */
//    PGpolygon getPolygon(const int col);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get path specified by the column key
//     * @note path.pts must be copied out if needed after clearing resultset
//     *          copy_points(path.npts, path.pts, ...);
//     * @param key column key
//     * @return Path
//     */
//    PGpath getPath(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get path specified by the column index
//     * @note path.pts must be copied out if needed after clearing resultset
//     *          copy_points(path.npts, path.pts, ...);
//     * @param col column index
//     * @return Path
//     */
//    PGpath getPath(const int col);
//
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get cube specified by the column index
//     * @note Cube is defined by 1 (= point) or 2 (= opposite corners of cube) points
//     * @note Points may have 1-100(CUBE_MAX_DIM) dimensions
//     * @param key column key
//     * @return Cube
//     */
//    PGcube getCube(const std::string& key);
//    /* TODO: add "*" to complete and activate doxygen entry ( /<star><star> )
//     * Get cube specified by the column key
//     * @note Cube is defined by 1 (= point) or 2 (= opposite corners of cube) points
//     * @note Points may have 1-100(CUBE_MAX_DIM) dimensions
//     * @param col column index
//     * @return Cube
//     */
//    PGcube getCube(const int col);


#if HAVE_POSTGIS
    /**
     * Get GEOS geometry type by the column key
     * @param key column key
     * @return GEOS geometry
     */
    GEOSGeometry* getGeometry(const std::string& key) {
        return this->getGeometry(this->getKeyIndex(key));
    }
    /**
     * Get GEOS geometry type by the column index
     * @param col column index
     * @return GEOS geometry
     */
    virtual GEOSGeometry* getGeometry(const int col) = 0;
    /**
     * Get GEOS geometry (linestring) type by the column key
     * @param key column key
     * @return GEOS geometry
     */
    GEOSGeometry* getLineString(const std::string& key) {
        return this->getLineString(this->getKeyIndex(key));
    }
    /**
     * Get GEOS geometry (linestring) type by the column index
     * @param col column index
     * @return GEOS geometry
     */
    virtual GEOSGeometry* getLineString(const int col) = 0;
#endif

    // =============== GETTERS - INTERVAL EVENT ================================
    /**
     * Get interval event by column key
     * @param key column key
     * @return interval event class
     */
    IntervalEvent *getIntervalEvent(const std::string& key) {
        return this->getIntervalEvent(this->getKeyIndex(key));
    }
    /**
     * Get interval event by column index
     * @param col column index
     * @return interval event class
     */
    virtual IntervalEvent *getIntervalEvent(const int col) = 0;
    
    // =============== GETTERS - OTHER =========================================

    /**
     * Gets binary data by a column key
     * @param key   column key
     * @param size size of output data
     * @return allocated data
     */
    void *getBlob(const std::string& key, int &size) {
        return this->getBlob(this->getKeyIndex(key), size);
    }
    /**
     * Gets binary data by a column index
     * @param col   column index
     * @param size size of output data
     * @return allocated data
     */
    virtual void *getBlob(const int col, int &size) = 0;

    ////////////// Print support methods
    /**
     * This goes through resultset and retrieves metadata necessary for print.
     * @note It needs to be done before every print.
     * @param row if not set to -1, this indicates single row print
     * @param get_widths whether column widths will be required
     * @param arrayLimit limits length of printed array
     * @return metadata for print, pair consisting of two vectors:
     *  -# Tkeys - column types etc.
     *  -# ints - column widths
     * @todo @b Vojta [param arrayLimit] který popisek je správný (tento nebo "desired column widths" z .cpp)?
     */
    virtual std::pair< TKeys*,std::vector<int>* > getKeysWidths(const int row, bool get_widths, const int arrayLimit) = 0;

protected:

    /**
     * Gets type of given column
     * @param col column index
     * @return type name
     */
    virtual std::string getKeyType(const int col) = 0;
    /**
     * Gets index of given column name
     * @param key column name
     * @return column index
     */
    virtual int getKeyIndex(const std::string& key) = 0;

};


#if HAVE_POSTGRESQL
class PGResultSet : public ResultSet, public PGBackendBase
{
public:

    PGResultSet(const PGBackendBase &base, DBTYPES_MAP *dbtypes);
    ~PGResultSet();

    void newResult(void *res);

    int countRows();
    int countCols();
    bool isOk();
    void clear();

    TKey getKey(int col);
    TKeys* getKeys();

    std::string getValue(const int col, const int arrayLimit = 0);

    char getChar(const int pos);
    std::string getString(const int col);
    bool getBool(const int col);
    int getInt(const int col);
    long long getInt8(const int col);
    int* getIntA(const int col, int& size);
    std::vector<int>* getIntV(const int col);
    long long* getInt8A(const int col, int& size);
    std::vector<long long>* getInt8V(const int col);
    float getFloat(const int col);
    double getFloat8(const int col);
    float* getFloatA(const int col, int& size);
    std::vector<float>* getFloatV(const int col);
    double* getFloat8A(const int col, int& size);
    std::vector<double>* getFloat8V(const int col);
    time_t getTimestamp(const int col);
    IntervalEvent *getIntervalEvent(const int col);
#if HAVE_POSTGRESQL
    PGpoint getPoint(const int col);
    PGpoint* getPointA(const int col, int& size);
    std::vector<PGpoint>*  getPointV(const int col);
#endif
#if HAVE_POSTGIS
    GEOSGeometry* getGeometry(const int col);
    GEOSGeometry* getLineString(const int col);
#endif
#if HAVE_OPENCV
    cv::Mat *getCvMat(const int col);
#endif
    void *getBlob(const int col, int &size);

    std::pair< TKeys*,std::vector<int>* > getKeysWidths(const int row = -1, bool get_widths = 1, const int arrayLimit = 0);

protected:
    std::string getKeyType(const int col);
    short getKeyTypeLength(const int col, const short def);
    int getKeyIndex(const std::string& key);

    template<typename TDB, typename TOUT>
    TOUT getSingleValue(const int col, const char *def);
    template<typename TDB, typename TOUT>
    TOUT *getArray(const int col, int &size, const char *def);
    template<typename TDB, typename TOUT>
    std::vector<TOUT> *getVector(const int col, const char *def);
};
#endif

#if HAVE_SQLITE
class SLResultSet : public ResultSet, public SLBackendBase
{
public:

    SLResultSet(const SLBackendBase &base);
    ~SLResultSet();

    void newResult(void *res);

    int countRows();
    int countCols();
    bool isOk();
    void clear();

    TKey getKey(int col);
    TKeys* getKeys();

    std::string getValue(const int col, const int arrayLimit = 0);

    char getChar(const int pos);
    std::string getString(const int col);
    bool getBool(const int col);
    int getInt(const int col);
    long long getInt8(const int col);
    int* getIntA(const int col, int& size);
    std::vector<int>* getIntV(const int col);
    long long* getInt8A(const int col, int& size);
    std::vector<long long>* getInt8V(const int col);
    std::vector< std::vector<int>* >* get(const int col);
    float getFloat(const int col);
    double getFloat8(const int col);
    float* getFloatA(const int col, int& size);
    std::vector<float>* getFloatV(const int col);
    double* getFloat8A(const int col, int& size);
    std::vector<double>* getFloat8V(const int col);
    time_t getTimestamp(const int col);
    IntervalEvent *getIntervalEvent(const int col);
#if HAVE_POSTGRESQL
    PGpoint getPoint(const int col);
    PGpoint* getPointA(const int col, int& size);
    std::vector<PGpoint>*  getPointV(const int col);
#endif
#if HAVE_POSTGIS
    GEOSGeometry* getGeometry(const int col);
    GEOSGeometry* getLineString(const int col);
#endif
#if HAVE_OPENCV
    cv::Mat *getCvMat(const int col);
#endif
    void *getBlob(const int col, int &size);
    
    std::pair< TKeys*,std::vector<int>* > getKeysWidths(const int row = -1, bool get_widths = 1, const int arrayLimit = 0);

protected:

    std::string getKeyType(const int col);
    int getKeyIndex(const std::string& key);

};
#endif

} // namespace vtapi

#endif	/* VTAPI_RESULTSET_H */

