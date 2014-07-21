/* 
 * File:   vtapi_keyvalues.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:51 PM
 */

#ifndef VTAPI_KEYVALUES_H
#define	VTAPI_KEYVALUES_H

#include "vtapi_commons.h"
#include "../queries/vtapi_queries.h"

namespace vtapi {

// ************************************************************************** //
/**
 * @brief KeyValues storage class
 *
 * @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 30*
 *
 * @todo @b doc: sjednotit "get a ... by a ..." vs. "get ... by the ..."
 */
class KeyValues : public Commons {
public:

    Select* select; /**< Select is (to be) pre-filled by the constructor @todo @b doc: Petrovy závorky */
    Insert* insert; /**< New insert to insert new data */
    Update* update; /**< New update to update new data */

public:
    /**
     * KeyValue contructor from commons object
     * @param orig pointer of the existing commons object
     */
    KeyValues(const Commons& orig);

    /**
     * KeyValue constructor from another KeyValues object
     * @param orig pointer to the parent KeyValues object
     * @param selection specific selection name
     */
    KeyValues(const KeyValues& orig, const string& selection = "");

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
     * Get key of a single table column
     * @param col Column index
     * @return Column key
     */
    TKey getKey(int col);
    /**
     * Get a list of all possible columns as TKeys
     * @return list of keys
     */
    TKeys* getKeys();

    /**
     * Print a current tuple of resultset
     * @return success
     */
    bool print();
    /**
     * Print all tuples of resultset
     * @return success
     */
    bool printAll();


    // =============== GETTERS (Select) ========================================

     /**
     * Generic getter - fetches any value from resultset and returns it as string
     * @param col column index
     * @return String representation of field value
     */
    string getValue(const int col, const int arrayLimit = 0);

    // =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===============
    /**
     * Get single character specified by a column key
     * @param key column key
     * @return character
     */
    char getChar(const string& key);
    /**
     * Get single character specified by column index
     * @param pos column index
     * @return character
     */
    char getChar(const int pos);
    /**
     * Get character array specified by column key
     * @param key column key
     * @param size size of the array of char values
     * @return character array
     */
    char *getCharA(const string& key, int& size);
    /**
     * Get character array specified by column index
     * @param pos column index
     * @param size size of the array of char values
     * @return character array
     */
    char *getCharA(const int pos, int& size);
    /**
     * Get a character array specified by a column key
     * @param key column key
     * @return character array
     */
    string getString(const string& key);
    /**
     * Get a string value specified by an index of a column
     * @param col column index
     * @return string value
     */
    string getString(const int col);

    // =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==============
    /**
     * Get an integer value specified by a column key
     * @param key column key
     * @return integer value
     */
    int getInt(const string& key);
    /**
     * Get an integer value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    int getInt(const int col);
    /**
     * Get long integer value specified by a column key
     * @param key column key
     * @return long integer value
     */
    long getInt8(const string& key);
    /**
     * Get long integer value specified by an index of a column
     * @param col index of column
     * @return long integer value
     */
    long getInt8(const int col);
    /**
     * Get an array of integer values specified by a column key
     * @param key column key
     * @param size size of the array of integer values
     * @return array of integer values
     */
    int* getIntA(const string& key, int& size);
    /**
     * Get an array of integer values specified by an index of a column
     * @param col index of column
     * @param size size of the array of integer values
     * @return array of integer values
     */
    int* getIntA(const int col, int& size);

    /**
     * Get a vector of integer values specified by an index of a column
     * @param col index of column
     * @return vector of integer values
     */
    vector<int>* getIntV(const int col);
    /**
     * Get a vector of integer values specified by a column key
     * @param key column key
     * @return vector of integer values
     */
    vector<int>* getIntV(const string& key);
    /**
     * Get a vector of integer vectors specified by an index of a column
     * @param col index of column
     * @return  vector of vectors of integer values
     */
    vector< vector<int>* >* getIntVV(const int col);
    /**
     * Get a vector of integer vectors specified by a column key
     * @param key column key
     * @return  vector of vectors of integer values
     */
    vector< vector<int>* >* getIntVV(const string& key);

    // =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ==================
    /**
     * Get a float value specified by a column key
     * @param key column key
     * @return float value
     */
    float getFloat(const string& key);
    /**
     * Get a float value specified by an index of a column
     * @param col index of column
     * @return float value
     */
    float getFloat(const int col);
    /**
     * Get a double value specified by a column key
     * @param key column key
     * @return double value
     */
    double getFloat8(const string& key);
    /**
     * Get a double value specified by an index of a column
     * @param col index of column
     * @return double value
     */
    double getFloat8(const int col);
    /**
     * Get an array of float values specified by a column key
     * @param key column key
     * @param size size of the array of float values
     * @return array of float values
     */
    float* getFloatA(const string& key, int& size);
    /**
     * Get array of float values specified by index of column
     * @param col index of column
     * @param size size of the array of float values
     * @return array of float values
     */
    float* getFloatA(const int col, int& size);
    /**
     * Get a vector of integer values specified by column key
     * @param col index of column
     * @return vector of integer values
     */
    vector<float>* getFloatV(const int col);
    /**
     * Get a vector of float values specified by the column index
     * @param key column key
     * @return vector of float values
     */
    vector<float>* getFloatV(const string& key);

    //TODO: is getFloatVV needed?

    // =============== GETTERS - TIMESTAMP =====================================
    /**
     * Get timestamp specified by column key
     * @param key column key
     * @return Timestamp info
     */
    time_t getTimestamp(const string& key);
    /**
     * Get timestamp specified by the column index
     * @param col column index
     * @return Timestamp info
     */
    time_t getTimestamp(const int col);

    // =============== GETTERS - OpenCV MATRICES ===============================
#ifdef __OPENCV_CORE_C_H__XXX
    /**
     * Get OpenCv matrix (cvMat) specified by the column key
     * @param key column key
     * @return CvMat structure
     */
    CvMat *getCvMat(const string& key);
    /**
     * Get OpenCv matrix (cvMat) specified by the column index
     * @param col column index
     * @return CvMat structure
     */
    CvMat *getCvMat(const int col);
    /**
     * Get OpenCv matrix (cvMatND) specified by the column key
     * @param key column key
     * @return CvMatND structure
     */
    CvMatND *getCvMatND(const string& key);
    /**
     * Get OpenCv matrix (cvMatND) specified by the column index
     * @param col column index
     * @return CvMatND structure
     */
    CvMatND *getCvMatND(const int col);
#endif

    // =============== GETTERS - GEOMETRIC TYPES ===============================
    // TODO: geometricke typy
#ifdef POSTGIS
    /**
     * Get 2D point specified by the column key
     * @param key column key
     * @return 2D Point
     */
    PGpoint getPoint(const string& key);
    /**
     * Get 2D point specified by the column index
     * @param col column index
     * @return 2D Point
     */
    PGpoint getPoint(const int col);
    /**
     * Get line segment specified by the column key
     * @param key column key
     * @return Line segment
     */
    PGlseg getLineSegment(const string& key);
    /**
     * Get line segment specified by the column index
     * @param col column index
     * @return Line segment
     */
    PGlseg getLineSegment(const int col);
    /**
     * Get box specified by the column key
     * @param key column key
     * @return Box
     */
    PGbox getBox(const string& key);
    /**
     * Get box specified by the column index
     * @param col column index
     * @return Box
     */
    PGbox getBox(const int col);
    /**
     * Get circle specified by the column key
     * @param key column key
     * @return Circle
     */
    PGcircle getCircle(const string& key);
    /**
     * Get circle specified by the column index
     * @param col column index
     * @return Circle
     */
    PGcircle getCircle(const int col);
    /**
     * Get polygon specified by the column key
     * @note polygon.pts must be copied out if needed after clearing resultset
     *          copy_points(polygon.npts, polygon.pts, ...);
     * @param key column key
     * @return Polygon
     */
    PGpolygon getPolygon(const string& key);
    /**
     * Get polygon specified by the column index
     * @note polygon.pts must be copied out if needed after clearing resultset
     *          copy_points(polygon.npts, polygon.pts, ...);
     * @param col column index
     * @return Polygon
     */
    PGpolygon getPolygon(const int col);
    /**
     * Get path specified by the column key
     * @note path.pts must be copied out if needed after clearing resultset
     *          copy_points(path.npts, path.pts, ...);
     * @param key column key
     * @return Path
     */
    PGpath getPath(const string& key);
    /**
     * Get path specified by the column index
     * @note path.pts must be copied out if needed after clearing resultset
     *          copy_points(path.npts, path.pts, ...);
     * @param col column index
     * @return Path
     */
    PGpath getPath(const int col);

    /**
     * Get cube specified by the column index
     * @note Cube is defined by 1 (= point) or 2 (= opposite corners of cube) points
     * @note Points may have 1-100(CUBE_MAX_DIM) dimensions
     * @param key column key
     * @return Cube
     */
    PGcube getCube(const string& key);
    /**
     * Get cube specified by the column key
     * @note Cube is defined by 1 (= point) or 2 (= opposite corners of cube) points
     * @note Points may have 1-100(CUBE_MAX_DIM) dimensions
     * @param col column index
     * @return Cube
     */
    PGcube getCube(const int col);

    /**
     * Get GEOS geometry type by the column key
     * @param key column key
     * @return GEOS geometry
     */
    GEOSGeometry* getGeometry(const string& key);
    /**
     * Get GEOS geometry type by the column index
     * @param col column index
     * @return GEOS geometry
     */
    GEOSGeometry* getGeometry(const int col);
    /**
     * Get GEOS geometry (linestring) type by the column key
     * @param key column key
     * @return GEOS geometry
     */
    GEOSGeometry* getLineString(const string& key);
    /**
     * Get GEOS geometry (linestring) type by the column index
     * @param col column index
     * @return GEOS geometry
     */
    GEOSGeometry* getLineString(const int col);
    /**
     * Get array of 2D points specified by the column index
     * @param key column index
     * @return vector of 2D Points
     */
    vector<PGpoint>*  getPointV(const string& key);
    /**
     * Get array of 2D points specified by the column key
     * @param col column key
     * @return vector of 2D Points
     */
    vector<PGpoint>*  getPointV(const int col);
#endif

    // =============== GETTERS - OTHER =========================================
    /**
     * Get an integer with an OID value specified by a column key
     * @param key column key
     * @return integer with the OID value
     */
    int getIntOid(const string& key);
    /**
     * Get an integer with an OID value specified by a column index
     * @param col column index
     * @return integer with the OID value
     */
    int getIntOid(const int col);


    // =============== SETTERS (Update) ========================================
    /**
     * This is to support updates in derived classes
     * (unimplemented error 3010 in this class)
     * @return success (in derived classes)
     */
    virtual bool preSet();


    /**
     * Set a new string value of the specified key
     * @param key column key to update
     * @param value new string value of the key
     * @return success
     */
    bool setString(const string& key, const string& value);

    /**
     * Set a new integer value of the specified key
     * @note New integer value is casted from string value
     * @param key column key to update
     * @param value new integer value (in string representation) of the key
     * @return success
     */
    bool setInt(const string& key, const string& value);

    /**
     * Set a new integer value of the specified key
     * @param key column key to update
     * @param value new integer value of the key
     * @return success
     */
    bool setInt(const string& key, int value);

    /**
     * Set a new integer array of the specified key
     * @param key column key to update
     * @param values new integer array of the key
     * @param size size of the array of integer values
     * @return success
     */
    bool setIntA(const string& key, int* values, int size);

    /**
     * Set a new integer vector of the specified key
     * @param key column key to update
     * @param values new integer vector of the key
     * @return success
     * @unimplemented
     */
    bool setIntV(const string& key, const vector<int> values);

    /**
     * Set a new float value of the specified key
     * @note New flaot value is casted from string value
     * @param key column key to update
     * @param value new float value (in string representation) of the key
     * @return success
     */
    bool setFloat(const string& key, const string& value);

    /**
     * Set a new float value of the specified key
     * @param key column key to update
     * @param value new float value of the key
     * @return success
     */
    bool setFloat(const string& key, float value);

    /**
     * Set a new float array of the specified key
     * @param key column key to update
     * @param values new float array of the key
     * @param size size of the array of integer values
     * @return success
     */
    bool setFloatA(const string& key, float* values, int size);

    /**
     * Set a new float vector of the specified key
     * @param key column key to update
     * @param values new float vector of the key
     * @return success
     * @unimplemented neimplementováno
     */
    bool setFloatV(const string& key, const vector<float> values);

    /**
     * Execute SQL UPDATE command
     * @return success
     */
    bool setExecute();

    // =============== ADDERS (Insert) ========================================
    /**
     *
     * @param key
     * @param value
     * @return
     * @todo @b code: všechny addX neimplementovány
     */
    bool addString(const string& key, const string& value);
    bool addInt(const string& key, const string& value);
    bool addInt(const string& key, int value);
    bool addIntA(const string& key, int* value, int size);
    bool addFloat(const string& key, const string& value);
    bool addFloat(const string& key, float value);
    bool addFloatA(const string& key, float* value, int size);

    /**
     * Execute SQL INSERT command
     * @return success
     */
    bool addExecute();

    /**
     * This is used to check whether the underlying dataset, sequence (directory / video) or interval (image) exists
     * @return found
     */
    bool checkStorage();

    /**
     * This goes through resultset and retrieves metadata necessary for print.
     * @note It needs to be done before every print.
     * @param row if not set to -1, this indicates single row print
     * @param get_widths whether column widths will be required @todo @b doc[VF]: který popisek je správný (tento nebo "desired column widths" z .cpp)?
     * @return metadata for print, pair consisting of two vectors:
     *  a) Tkeys - column types etc., b) ints - column widths
     */
    pair< TKeys*,vector<int>* > getFieldsInfo(const int row, int get_widths);

protected:
    string caption; /**< table caption */
    string tableOpt; /**< custom table options (border, padding, whatever..) */

    ////////////// Print support methods

    /**
     * Prints table header.
     * @param fInfo Metadata for print, pair consisting of two vectors:
     *  a) Tkeys - column types etc., b) ints - column widths
     * @return success
     */
    bool printHeader(const pair< TKeys*,vector<int>* > fInfo);

    /**
     * Prints one row of resultset.
     * @param row Which row of resultset to print
     * @param widths Metadata - vector of column widths
     * @return success
     */
    bool printRowOnly(const int row, const vector<int>* widths);

    /**
     * Prints table footer and info about printed rows
     * @param count How many rows were printed (0 = single row was printed)
     * @return success
     */
    bool printFooter(const int count);

};

} // namespace vtapi

#endif	/* VTAPI_KEYVALUES_H */

