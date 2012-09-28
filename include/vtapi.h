/**
 * @mainpage
 *
 * @section ABOUT What is VTApi
 * VTApi is a PostgreSQL database and OpenCV API for the VT project.
 * The project is oriented towards processing of records containing
 * image and video information – categorization, searching and comparison.
 *
 * @section HOMEPAGE VTApi development homepage
 * https://gitorious.org/vtapi
 *
 * @section PREREQUSITIES Prerequisities
 *    - OS Windows, Linux, 32 and 64bit
 *    - GCC 4.4.6, 4.5.2+
 *    - PostgreSQL 9.1 – libpq
 *    - libpqtypes 1.5
 *    - OpenCV 2.3 (mandatory)
 *
 * @note For more information visit https://gitorious.org/vtapi/pages/Install .

 *
 * @section LOGICAL Logical model of VTApi
 * @image html minimal_logical_data_model_small.png "Logical model of VTApi"
 * @image latex minimal_logical_data_model.png "Logical model of VTApi"
 * @image rtf minimal_logical_data_model.png "Logical model of VTApi"
 *
 * @section BASIC_TERMS Basic terms
 * @subsection LOGICAL_DATASET Dataset
 * The dataset is the main storing center. This is used as a folder with video
 * files or as a folder with folders of images. There are stored also the 
 * metadata for each multimedial data.
 *
 * @subsection LOGICAL_SEQUENCE Sequence
 * The sequence is the basic unit of the datasets. This can be represent as
 * a set of images (video) or as a folder of images.
 *
 * @subsection LOGICAL_INTERVAL Interval
 * The interval is a subset of sequence (set of images) with the same metadata,
 * It can be a continuous interval of video (frames) or a sequence of images.
 * The metadata can be generally various, but they are always created by
 * a process.
 *
 * @subsection LOGICAL_METHOD Method
 * The method defines the the structure of data.
 *
 * @subsection LOGICAL_PROCESS Process
 * The process is a specific instance of method. The process inserts data.
 *
 * @subsection LOGICAL_SELECTION Selection
 * The selection is a subset of logically related metadata. Thanks to this, the
 * operations are effective and the processes can be chained. The intervals
 * or the tags are special use of selection.
 *
 * @subsection LOGICAL_KEYVALUES KeyValues
 * The KeyValue is a basic mechanism of organization metadata in VTApi. It is
 * data structure, which allows you to store metadata into pair <key, value>.
 *
 * @section AUTHORS VTApi Team
 * The team consists of following people from Faculty of Information Technology, Brno University of Technology, CZ:
 * @authors Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @authors Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @authors Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @section LICENSE License
 * There will be license information for VTApi.
 * @copyright &copy; FIT BUT, CZ, 2011
 *
 * @example vtapi.conf
 * This file shows an example configuration file for VTApi.
 *
 *
 * @file
 * @authors
 * VTApi Team, FIT BUT, CZ
 * Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 *
 * @section LECENSE License
 *
 * There will be license information for VTApi.
 * &copy; FIT BUT, CZ, 2011
 *
 *
 * @section DESCRIPTION Description
 * 
 * Main classes which provide a basic functionality of VTApi.
 */

#ifndef VTAPI_H
#define	VTAPI_H

#include "vtapi_config.h"
#include "vtapi_commons.h"

// virtual definitions of classes
// list of classes, which are contained in this header
class VTApi;

class TKey;
class KeyValues;

class Dataset;
class Sequence;
class Video;
class Interval;
class Image;
class Method;
class Process;

class Query;
class Select;
class Insert;


/**
 * @brief This is to represent the Key in fields in queries
 * 
 * ... just for the feeling (and vectors, of course)
 * @note You can use size=-1 for NULL :)
 */
class TKey {
// Members
public:
    String type; /**< Name of data type */
    String key;  /**< Name of a column */
    int size;    /**< "0" is the value right now */   // you can use -1 for NULL :)
    String from; /**< Distinguish between in/out right now */

// Methods
public:
    /** 
     * Constructor for NULL
     */
    TKey() : size(-1) {};

    /**
     * Copy constructor
     * @param orig
     */
    TKey(const TKey& orig) 
            : type(orig.type), key(orig.key), from(orig.from), size(orig.size) {};

    /**
     * Constructor for full specification of arguments
     * @param type name of a data type
     * @param key name of a column
     * @param size "0" is the value right now
     * @param from distinguish between in/out right now
     */
    TKey(const String& type, const String& key, const int size, const String& from = "")
            : type(type), key(key), size(size), from(from) {};

    
    /**
     * Print data and return data that was printed
     * @return data of TKey that was printed
     */
    String print();
};


// ************************************************************************** //
/**
 * @brief This is a (virtual) query class
 * 
 * TODO: It will be used for delayed queries (store())
 * @see http://libpqtypes.esilo.com/
 *
 * @note Error codes 20*
 */
class Query : public Commons {
// Members
public:
    std::vector<TKey> keys; /**< This is where those keys are stored */
    String queryString; /**< This is used for (direct) queries */
    PGparam* param; /**< This is used for parameter passing to query */
    PGresult* res; /**< This is where results are (to be) not NULL */
    bool executed; /**< This is a flag wheather the query was executed after any change */

// Methods
public:
    /**
     * Construct a query object
     * @param commons pointer of the existing commons object
     * @param query query string
     * @param param parameters for passing to the query
     */
    Query(const Commons& commons, const String& query = "", PGparam *param = NULL);
    /**
     * Destruct query class and also other commons objects
     */
    virtual ~Query();

    /**
     * This expands the query, so you can check it before the execution
     * @return string value with the query
     */
    virtual String getQuery();

    /**
     * This will commit your query
     * @return success of the query
     */
    bool execute();
    // TODO? virtual bool prepare();

    /**
     * This may be hazardeous for someone...
     * marked as deprecated, because there is no discouraged mark
     * @deprecated
     * @param key
     * @return success
     */
    bool keyValue(const TKey& key);

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times as:
     * @todo
     * @param key
     * @param value
     * @param from
     * @return success
     */
    bool keyString(const String& key, const String& value, const String& from = "");
    bool keyStringA(const String& key, const String* values, const int size, const String& from = "");
    bool keyInt(const String& key, int value, const String& from = "");
    bool keyIntA(const String& key, const int* values, const int size, const String& from = "");
    bool keyFloat(const String& key, float value, const String& from = "");
    bool keyFloatA(const String& key, const float* values, const int size, const String& from = "");
    bool keySeqtype(const String& key, const String& value, const String& from = "");
    bool keyInouttype(const String& key, const String& value, const String& from = "");
    bool keyPermissions(const String& key, const String& value, const String& from = "");

    // FIXME: use keys instead of all the below
    /**
     * This is to specify the (single) table to be inserted in
     * @param table table into which new data will be inserted
     * @return success
     */
    bool setTable(const String& table);
    String table;       /** This is where the (single) table/selection is stored */

    // FIXME: use keys instead of this
    /**
     * This is a WHERE statement construction class for "Strings"
     * It can be called several times as:
     *
     * @param key
     * @param value
     * @param oper
     * @param table
     * @return 
     */
    bool whereString(const String& key, const String& value, const String& oper = "=", const String& table = "");

    /**
     * This is a WHERE statement construction class for integers
     * It can be called several times and for arrays as:
     *
     * @param key
     * @param value
     * @param oper
     * @param table
     * @return
     */
    bool whereInt(const String& key, const int value, const String& oper = "=", const String& table = "");

    bool whereFloat(const String& key, const float value, const String& oper = "=", const String& table = "");
    String where;   // FIXME: see above :(

protected:
    String escapeColumn(const String& key, const String& table = "");
};

/**
 * @brief This is a class where queries are (to be) constructed
 *
 * Mechanism: TBD
 *
 * @note Errors 21*
 *
 */
class Select : public Query {
// Members
public:
    // TODO: this->from["intervals"] = "*";
    // FIXME: use keys
    std::multimap<String, String> fromList; /**< This is a tuple table and column name */

    String groupby; /**< String for a GROUP BY part of the query */
    String orderby; /**< String for a ORDER BY part of the query */

    int limit;  /**< Specify a size (a number of rows) of the resultset */
    int offset; /**< Specify an index of row, where the resultset starts */

// Methods
public:
    /**
     * @todo
     * @param commons
     * @param queryString
     * @param param
     */
    Select(const Commons& commons, const String& queryString = "", PGparam *param = NULL);

    /**
     * This expands the query, so you can check it before the execution
     * @return string value with SQL select query
     */
    String getQuery();

    /**
     * This is to specify the from clause and the select (column) list
     * It may be called more times.
     * @param table
     * @param column
     * @return success
     */
    bool from(const String& table, const String& column);

    /**
     * This is to join tables if they can be performed automatically.
     * If not, returns false (no quarantee before version 2).
     * @return success
     */
    bool join();

    /**
     * This is to discard current resultset and fetch next one with updated
     * OFFSET value
     * @return success
     */
    bool executeNext();

};

/**
 * @brief This is a class where queries are (to be) constructed
 * 
 * Mechanism: TBD
 *
 * @note Error codes 22*
 *
 */
class Insert : public Query {
public:
    /**
     * @todo
     * @param commons
     * @param insertString
     * @param param
     */
    Insert(const Commons& commons, const String& insertString = "", PGparam *param = NULL);

    /**
     * This expands the query, so you can check it before the execution
     * @return string value with SQL insert command
     */
    String getQuery();
};

/**
 * @brief This is a class where queries are (to be) constructed
 *
 * Mechanism: TBD
 * 
 * @todo: in the future version (1.0), this class will use Select (whereKV)
 *
 * @warning This class used unproperly may destroy the life and the universe.
 * RECOMENDATION: Wait for the version 1.0.
 *
 * @note Error codes 23*
 *
 */
class Update : public Query {
public:
    Update(const Commons& commons, const String& queryString = "", PGparam *param = NULL);

    /**
     * This expands the query, so you can check it before the execution
     * @return
     */
    String getQuery();
};



// ************************************************************************** //
/**
 * @brief KeyValues storage class
 *
 * @note Error codes 30*
 */
class KeyValues : public Commons {
// Members
public:
    Select* select; /**< Select is (to be) pre-filled by the constructor */
    int pos;        /**< Tuple of the resultset; initialized to -1 by default */
    Insert* insert; /**< New insert to insert new data */
    Update* update;
    // some other inherited from @link Commons

// Methods
public:
    KeyValues(const Commons& orig);
    KeyValues(const KeyValues& orig, const String& selection = "");   // FIXME: why is this needed? Stupid C++???

    /**
     * This destroys the KeyValues
     * It raises warning when there was something left not-excecuted (some collaborants left)
     */
    virtual ~KeyValues();

    /**
     * The most used function of the VTApi - next (row)
     * @return this or null
     */
    KeyValues* next();

    /**
     * Get key of a single table column
     * @param pos Column index
     * @return Column key
     */
    TKey getKey(int col);
    /**
     * Get a list of all possible columns as TKeys
     * @return list of key name and TODO
     */
    std::vector<TKey>* getKeys();

    /**
     * Print a current tuple of resultset
     */
    void print();
    /**
     * Print all tuples of resultset
     */
    void printAll();


    // =============== GETTERS (Select) ========================================

     /**
     * Generic getter - fetches any value from resultset and returns it as string
     * @param col column index
     * @return String representation of field value
     */
    String getValue(const int col);

    // =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===============
    /**
     * Get single character specified by a column key
     * @param key column key
     * @return character
     */
    char getChar(const String& key);
    /**
     * Get single character specified by column index
     * @param col column index
     * @return character
     */
    char getChar(const int pos);
    /**
     * Get character array specified by column key
     * @param col column key
     * @param size size of the array of char values
     * @return character
     */
    char *getCharA(const String& key, int& size);
    /**
     * Get character array specified by column index
     * @param col column index
     * @param size size of the array of char values
     * @return character
     */
    char *getCharA(const int pos, int& size);
    /**
     * Get a string value specified by a column key
     * @param key column key
     * @return string value
     */
    String getString(const String& key);
    /**
     * Get a string value specified by an index of a column
     * @param col column index
     * @return string value
     */
    String getString(const int col);

    // =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==============
    /**
     * Get an integer value specified by a column key
     * @param key column key
     * @return integer value
     */
    int getInt(const String& key);
    /**
     * Get an integer value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    int getInt(const int col);
    /**
     * Get long value specified by a column key
     * @param key column key
     * @return integer value
     */
    long getInt8(const String& key);
    /**
     * Get long value specified by an index of a column
     * @param col index of column
     * @return integer value
     */
    long getInt8(const int col);
    /**
     * Get an array of integer values specified by a column key
     * @param key column key
     * @param size size of the array of integer values
     * @return array of integer values
     */
    int* getIntA(const String& key, int& size);
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
     * @return  array of integer values
     */
    std::vector<int>* getIntV(const int col);
    /**
     * Get a vector of integer values specified by a column key
     * @param key column key
     * @return  array of integer values
     */
    std::vector<int>* getIntV(const String& key);
    /**
     * Get a vector of integer vectors specified by an index of a column
     * @param col index of column
     * @return  array of arrays of integer values
     */
    std::vector< std::vector<int>* >* getIntVV(const int col);
    /**
     * Get a vector of integer vectors specified by a column key
     * @param key column key
     * @return  array of arrays of integer values
     */
    std::vector< std::vector<int>* >* getIntVV(const String& key);

    // =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ==================
    /**
     * Get a float value specified by a column key
     * @param key column key
     * @return float value
     */
    float getFloat(const String& key);
    /**
     * Get a float value specified by an index of a column
     * @param col index of column
     * @return float value
     */
    float getFloat(const int col);
    /**
     * Get a double value specified by a column key
     * @param key column key
     * @return float value
     */
    double getFloat8(const String& key);
    /**
     * Get a double value specified by an index of a column
     * @param col index of column
     * @return float value
     */
    double getFloat8(const int col);
    /**
     * Get an array of float values specified by a column key
     * @param key column key
     * @param size size of the array of float values
     * @return array of float values
     */
    float* getFloatA(const String& key, int& size);
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
     * @return  array of integer values
     */
    std::vector<float>* getFloatV(const int col);
    /**
     * Get a vector of float values specified by the column index
     * @param key column key
     * @return  array of float values
     */
    std::vector<float>* getFloatV(const String& key);

    // =============== GETTERS - TIMESTAMP =====================================
    /**
     * Get timestamp specified by column key
     * @param key column key
     * @return Timestamp info
     */
    struct tm getTimestamp(const String& key);
    /**
     * Get timestamp specified by the column index
     * @param col column index
     * @return Timestamp info
     */
    struct tm getTimestamp(const int col);

    // =============== GETTERS - OpenCV MATRICES ===============================
#ifdef __OPENCV_CORE_C_H__
    /**
     * Get OpenCv matrix (cvMat) specified by the column key
     * @param key column key
     * @return CvMat structure
     */
    CvMat *getCvMat(const String& key);
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
    CvMatND *getCvMatND(const String& key);
    /**
     * Get OpenCv matrix (cvMatND) specified by the column index
     * @param col column index
     * @return CvMatND structure
     */
    CvMatND *getCvMatND(const int col);
#endif
    
    // =============== GETTERS - GEOMETRIC TYPES ===============================
#ifdef POSTGIS
    /**
     * Get 2D point specified by the column key
     * @param key column key
     * @return 2D Point
     */
    PGpoint getPoint(const String& key);
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
    PGlseg getLineSegment(const String& key);
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
    PGbox getBox(const String& key);
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
    PGcircle getCircle(const String& key);
    /**
     * Get circle specified by the column index
     * @param col column index
     * @return Circle
     */ 
    PGcircle getCircle(const int col);
    /**
     * Get polygon specified by the column key
     * note: polygon.pts must be copied out if needed after clearing resultset
     *          copy_points(polygon.npts, polygon.pts, ...);
     * @param key column key
     * @return Polygon
     */
    PGpolygon getPolygon(const String& key);
    /**
     * Get polygon specified by the column index
     * note: polygon.pts must be copied out if needed after clearing resultset
     *          copy_points(polygon.npts, polygon.pts, ...);
     * @param col column index
     * @return Polygon
     */ 
    PGpolygon getPolygon(const int col);
    /**
     * Get path specified by the column key
     * note: path.pts must be copied out if needed after clearing resultset
     *          copy_points(path.npts, path.pts, ...);
     * @param key column key
     * @return 2D Point
     */
    PGpath getPath(const String& key);
    /**
     * Get path specified by the column index
     * note: path.pts must be copied out if needed after clearing resultset
     *          copy_points(path.npts, path.pts, ...);
     * @param col column index
     * @return 2D Point
     */ 
    PGpath getPath(const int col);

    /**
     * Get cube specified by the column index
     * Cube is defined by 1 (= point) or 2 (= opposite corners of cube) points
     * Points may have 1-100(CUBE_MAX_DIM) dimensions
     * @param key column key
     * @return Cube
     */
    PGcube getCube(const String& key);
    /**
     * Get cube specified by the column key
     * Cube is defined by 1 (= point) or 2 (= opposite corners of cube) points
     * Points may have 1-100(CUBE_MAX_DIM) dimensions
     * @param col column index
     * @return Cube
     */
    PGcube getCube(const int col);

    /**
     * Get GEOS geometry type by the column key
     * @param key column key
     * @return GEOS geometry
     */
    GEOSGeometry* getGeometry(const String& key);
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
    GEOSGeometry* getLineString(const String& key);
    /**
     * Get GEOS geometry (linestring) type by the column index
     * @param col column index
     * @return GEOS geometry
     */
    GEOSGeometry* getLineString(const int col);
    /**
     * Get array of 2D points specified by the column index
     * @param col column index
     * @return vector of 2D Points
     */
    std::vector<PGpoint>*  getPointV(const String& key);
    /**
     * Get array of 2D points specified by the column key
     * @param col column key
     * @return vector of 2D Points
     */
    std::vector<PGpoint>*  getPointV(const int col);
#endif

    // =============== GETTERS - OTHER =========================================
    /**
     * Get an integer with an OID value specified by a column key
     * @param key column key
     * @return integer with the OID value
     */
    int getIntOid(const String& key);

    // =============== SETTERS (Update) ========================================
    /**
     * This is to support updates in derived classes
     * (unimplemented error 3010 in this class)
     * @return success (in derived classes)
     */
    virtual bool preSet();


    // TODO: overit jestli a jak funguje... jako UPDATE?
    bool setString(const String& key, const String& value);
    bool setInt(const String& key, const String& value);
    bool setInt(const String& key, int value);
    bool setIntA(const String& key, const int* values, int size);
    bool setIntV(const String& key, const std::vector<int> values);
    bool setFloat(const String& key, const String& value);
    bool setFloat(const String& key, float value);
    bool setFloatA(const String& key, const float* values, int size);
    bool setFloatV(const String& key, const std::vector<float> values);
    bool setExecute();
    
    // =============== ADDERS (Insert) ========================================
    // TODO: implement?
    bool addString(const String& key, const String& value);
    bool addInt(const String& key, const String& value);
    bool addInt(const String& key, int value);
    bool addIntA(const String& key, int* value, int size);
    bool addFloat(const String& key, const String& value);
    bool addFloat(const String& key, float value);
    bool addFloatA(const String& key, float* value, int size);
    bool addExecute();

    /**
     * This is to check whether the underlying dataset, sequence (directory / video) or interval (image) exists
     * @return found
     */
    bool checkStorage();

protected:
    // table caption
    String caption;
    // custom table options (border, padding, whatever..)
    String tableOpt;

    ////////////// Print support methods

    /**
     * Prints table header.
     * @param fInfo Metadata for print, pair consisting of two vectors:
     *  a) Tkeys - column types etc., b) ints - column widths
     */
    void printHeader(const std::pair< std::vector<TKey>*,std::vector<int>* > fInfo);
    /**
     * Prints one row of resultset.
     * @param row Which row of resultset to print
     * @param widths Metadata - vector of column widths
     */
    void printRowOnly(const int row, const std::vector<int>* widths);
    /**
     * Prints table footer and info about printed rows
     * @param count How many rows were printed (0 = single row was printed)
     */

    void printFooter(const int count = 0);
    /**
     * This goes through result set and retrieves metadate necessary for print.
     * It needs to be done before every print.
     * @param row If not set to -1, this indicates single row print.
     * @param indicator whether column widths will be required
     * @return Metadata for print, pair consisting of two vectors:
     *  a) Tkeys - column types etc., b) ints - column widths
     */
    std::pair< std::vector<TKey>*,std::vector<int>* > getFieldsInfo(const int row = -1, int get_widths = 1);
};



/**
 * @brief This class should always be on the path of your programm...
 *
 * @note Error codes 31*
 */
class Dataset : public KeyValues {
public:
    /**
     * This is the recommended constructor.
     *
     * @warning you can ommit the \a name only in these cases:
     *    -# Don't know the name -> use next
     *    -# The dataset is in your vtapi.conf
     *
     * @param orig
     * @param name
     * @todo params orig&name in documentation
     */
    Dataset(const KeyValues& orig, const String& name = "");

    /**
     * Move to a next dataset and set dataset name and location varibles
     * @return success
     * @note Over-loading next() from KeyValues
     */
    bool next();

    /**
     * Get a dataset name
     * @return string value with the name of the dataset
     */
    String getName();
    /**
     * Get a dataset location
     * @return string value with the location of the dataset
     */
    String getLocation();


    /**
     * Get new sequence(s) of the current dataset
     * @param name of the sequence (none for all)
     * @return sequence
     */
    Sequence* newSequence(const String& name = "");
    /**
     * Get new video(s) of current dataset
     * @param name (none for all)
     * @return video
     */
    Video* newVideo(const String& name = "");

    
    /**
     * Get new method(s) of the current dataset
     * @param name of the method (none for all)
     * @return method
     */
    Method* newMethod(const String& name = "");

    /**
     * Get new process of the current dataset
     * @param name of new process
     * @return process
     */
    Process* newProcess(const String& name = "");

protected:

};


/**
 * @brief A Sequence class manages videos and images
 *
 * @note Error codes 32*
 */
class Sequence : public KeyValues {
/* Memebers
protected:
    String file_name_video; // < File name of a video
    String file_name_image; // < File name of an image */
//Methods
public:
    /**
     * Constructor for sequences
     * @param orig pointer to the parrent
     * @param name name of sequence, which we can construct
     */
    Sequence(const KeyValues& orig, const String& name = "");

    /**
     * Move to a next sequence and set sequence name and location varibles
     * @return success
     * @note Over-loading next() from KeyValues
     */
    bool next();

    /**
     * Get a sequence name
     * @return string value with the name of the sequence
     */
    String getName();
    /**
     * Get a sequence location
     * @return string value with the location of the sequence
     */
    String getLocation();

    /**
     * Add new sequence to a table
     * @param name name of the sequence
     * @param location location of the sequence
     * @param type type of the sequence
     * @return success
     */
    bool add(String name, String location, String type);

    /**
     * Create a new interval specified by a start time and an end time
     * @param t1 start time
     * @param t2 end time
     * @return pointer to the new interval
     */
    Interval* newInterval(const int t1 = -1, const int t2 = -1);

    /**
     * Create a new image specified by a name
     * @param name name of the image
     * @return pointer to the new image
     */
    Image* newImage(const String& name = "");

    /**
     * @todo Not implemented
     * @param name
     * @return
     */
    Process* newProcess(const String& name = "");

#ifdef __OPENCV_CORE_HPP__
    cv::Mat getNextImage();
#endif

};


/**
 * @brief Video class manages videos
 *
 * @note Error codes 321*
 */
class Video : public Sequence {
public:
    /**
     * Constructor for Video
     * @param orig -in class (parent)
     * @param name of the sequence
     */    
    Video(const KeyValues& orig, const String& name);

    /**
     * Create a new frame specified by the frame number
     * @param name name of the image
     * @return pointer to the new image
     */
    Image* newFrame(const int frame = 1);

    /**
     * This is most probably what you always wanted...
     * @return string value with the location of the video data
     */
    String getDataLocation();


    /**
     * Add new video to the dataset
     * @param name of the video
     * @param location of the video
     * @return success
     */
    bool add(String name, String location);


#ifdef __OPENCV_CORE_HPP__
    /**
     * @todo Test
     * @param name
     * @return
     */
    bool openVideo();

    cv::Mat getNextImage();
#endif

};



#ifdef __OPENCV_HIGHGUI_HPP__
/**
 * @brief This is the ever-simplest video player...
 *
 * VideoPlayer makes copies of each object, so it doesn't affect nexts() performed elsewhere,
 * however, it may fail in case of next, where are hundreds of thousands of tuples (@see Keyvalues)
 * // TODO: This behavior might be chenged later
 *
 *  @note Error codes 16*
 */
class VideoPlayer : public Commons {
protected:
    String videoOutput;
    //

    std::vector<Image> images;
    std::vector<Video> videos;
    std::vector<Interval> intervals;

public:
    /**
     * A void construcotr - plays nothing at all at the moment
     * @param orig
     */
    VideoPlayer(Commons& orig);
    VideoPlayer(Image& image);
    VideoPlayer(Video& video);
    VideoPlayer(Interval& interval);

    bool playerAdd(Image& image);
    bool playerAdd(Video& video);
    bool playerAdd(Interval& interval);

    bool setPlayerOutput(String filename);
    String getPlayerOutput();

    /**
     * This function simply plays what added before
     * ... or a default capture in cese of none (can be used to store the capture)
     * @return
     */
    bool play();
};

#endif




/**
 * @brief Interval is equivalent to an interval of images
 *
 * @note Error codes 33*
 */
class Interval : public KeyValues {
public:
    /** This is because of image and video load && getSequence() documented. */
    Sequence* parentSequence;   // TODO: destroy in destructor if doom
    bool parentSequenceDoom;    // this is whwether do destroy the above

public:
    /**
     * Constructor for intervals
     * @param orig pointer to the parrent
     * @param selection name of a selection table
     */
    Interval(const KeyValues& orig, const String& selection = "intervals");

    // bool next(); not necessary

    /**
     * This is to (fast)
     * @return sequence string
     */
    String getSequenceName();
    
    /**
     * WARNING: This function has changed the return signature in version pre2!
     *          You may use getSequenceName()...
     *
     * This is to query (if needed) the Sequences (may be slow for continuous use) to
     * @return sequence object
     */
    Sequence* getSequence();

    /**
     * Get a start time of the current interval
     * @return start time
     */
    int getStartTime();
    /**
     * Get an end time of the current interval
     * @return end time
     */
    int getEndTime();

    /**
     * Add new interval to a table
     * @todo return always true
     * @param sequence interval name
     * @param t1 start time
     * @param t2 end time
     * @param location of the image
     * @return success
     */
    bool add(const String& sequence, const int t1, const int t2 = -1, const String& location = "");

    /**
     * This is to support updates
     * @return success (in derived classes)
     */
    bool preSet();
};

/**
 * @brief This represents images
 * @todo Not implemented [TV]
 * @note Error codes 339*
 */
class Image : public Interval {
// Methods
public:
    Image(const KeyValues& orig, const String& selection = "intervals");

    /**
     * Get a sequence (order) number of the current image (interval)
     * @return number or 0
     */
    int getTime();

    /**
     * This is most probably what you always wanted...
     * @return string value with the location of the data
     */
    String getDataLocation();
    /**
     * This is here just for image name
     * @return string value with the location of the image
     */
    String getLocation();

    /**
     * Simply adds an image (interval) to the sequence (no checking)
     * @param sequence
     * @param t
     * @param location
     * @return success
     */
    bool add(const String& sequence, const int t, const String& location);

protected:
    
};


/**
 * @brief A class which represents methods and gets also their keys
 *
 * @note Error codes 35*
 */
class Method : public KeyValues {
// Members
public:
    std::vector<TKey> methodKeys; /**< A vector of key-value pairs*/
    
// Methods
public:
    /**
     * Constructor for methods
     * @param orig pointer to the parrent
     * @param name name of method, which we can construct
     */
    Method(const KeyValues& orig, const String& name = "");

    /**
     * Move to a next method and set a method name and its methodkeys variables
     * @return success
     * @note Over-loading next() from KeyValues
     */
    bool next();
    /**
     * Get a name of the current method
     * @return string value with the name of the method
     */
    String getName();
    /**
     * This is to refresh the methodKeys vector
     * @return vector<TKey>
     */
    std::vector<TKey> getMethodKeys();

    /**
     * Create new process for current dataset
     * @return pointer to new sequence
     */
    Process* newProcess(const String& name = "");

private:
    /**
     * @todo: NOT IMPLEMENTED? [TV]
     * @param inout
     */
    void printData(const String& inout);
};

/**
 * @brief A class which represents processes and gets information about them
 *
 * @note Error codes 36*
 */
class Process : public KeyValues {
// Methods
public:
    Process(const KeyValues& orig, const String& name = "");

    /**
     * Individual next() for processes, which stores current process
     * and selection to commons
     * @return success
     */
    bool next();


    /**
     * Get a process name
     * @return string value with a process name
     */
    String getName();
    /**
     * Get a name of a table where are stored an input data
     * @return string value with an input data table name
     */
    String getInputs();
    /**
     * Get a name of a table where are stored an output data
     * @return string value with an output data table name
     */
    String getOutputs();

    // TODO: o tohle bych se ani nepokousel
    /**
     * A dangerous and rather discouraged function...
     * @deprecated by the human power
     * @param method
     * @param name
     * @param selection
     * @return
     */
    bool add(const String& method, const String& name, const String& selection="intervals");

    /**
     * Create new interval for process
     * // TODO: unused t1, t2
     * @param t1 currently unused
     * @param t2 currently unused
     * @return new interval
     */
    Interval* newInterval(const int t1 = -1, const int t2 = -1);
    /**
     * Create new sequence for process
     * // TODO: not implemented method
     * @param name sequence name (TODO: is it correct?)
     * @return new sequence
     */
    Sequence* newSequence(const String& name = "");

// TODO:    void print();
};


/**
 * @brief VTApi class manages Commons and processes args[]
 * This is how to begin
 *
 * TODO: include http://www.gnu.org/s/gengetopt/gengetopt.html
 *       special interest to the configuration files is needed
 *
 * @note Error codes 60*
 */ // ********************************************************************** //
class VTApi {
// Members
public:
    Commons* commons; /**< Commons are common objects to the project. */

// Methods
public:
    /**
     * Constructor recomended (in the future)
     * @param argc
     * @param argv
     */
    VTApi(int argc, char** argv);
    VTApi(const String& configFile);
    VTApi(const String& connStr, const String& location, const String& user, const String& password);
    VTApi(const Commons& orig);
    VTApi(const VTApi& orig);
    virtual ~VTApi();


    /**
     * For testing and learning purposes
     * This might be a HOW-TO function
     */
    void test();


    /**
     * This is how to continue after creating the API class...
     * @return
     */
    Dataset* newDataset(const String& name = "");

protected:

};




// this is just a development branch...
/**
 * @brief A generic class for storing a single keyvalue type
 *
 * It uses std::copy (memcpy) to maintain the object data (except pointer targets)
 * 
 * @warning
 *     - use PDOs only ... http://en.wikipedia.org/wiki/Plain_old_data_structure <br>
 *     - if you use pointers, you shouldn't free them
 * @note You can use size=-1 for NULL :)
 *
 * @see http://en.wikipedia.org/wiki/Plain_old_data_structure
 * @see http://www.cplusplus.com/doc/tutorial/templates/
 * @see http://stackoverflow.com/questions/2627223/c-template-class-constructor-with-variable-arguments
 * @see http://www.cplusplus.com/reference/std/typeinfo/type_info/
 */
template <class T>
class TKeyValue : public TKey {
// Members
public:    
    String typein; /**< This attribute is there for validation */
    T* values;

// Methods
public:
    TKeyValue() : TKey(), values(NULL) {};
    TKeyValue(const String& type, const String& key, const T& value, const String& from = "")
            : TKey(type, key, 1, from) {
        values = new T[1];
        values[0] = value;
        typein = typeid(this->values).name();
    }
    TKeyValue (const String& type, const String& key, const T* values, const int size, const String& from = "")
            : TKey(type, key, size, from) {
        this->values = new T[this->size];
        std::copy(values, values+size, this->values);
        // memcpy(this->values, values, size*sizeof(values));
        typein = typeid(values).name();
    }

    ~TKeyValue () {
        destructall(values);
    }
    
    /**
     * Print values from TKey members
     * @return string which contains a dump of TKey members
     */
    String print();
};


// FIXME: tohle kdyz dam jinam, tak je to v haji - proc?
template <class T>
String TKeyValue<T>::print() {
    String ret = "TKeyValue<" + String(typeid(values).name()) + "> type=" + type +
            ", key=" + key + ", from=" + from + ", size=" + toString(size) + ", values=\n";
    if (values && size > 0) {
        for(int i=0; i < size; ++i) {
            ret += toString(values[i]) + ", ";
        }
    }
    else ret += "NULL  ";
    ret = ret.erase(ret.length()-2) + "\n";

    std::cout << ret;
    return (ret);
}

#endif	/* VTAPI_H */

