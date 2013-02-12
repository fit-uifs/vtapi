/**
 * Main classes which provide a basic functionality of VTApi.
 *
 * @copyright Brno University of Technology &copy; 2011 &ndash; 2012
 *
 * VTApi is distributed under BUT OPEN SOURCE LICENCE (Version 1).
 * This licence agreement provides in essentials the same extent of rights as the terms of GNU GPL version 2 and Software fulfils the requirements of the Open Source software.
 *
 * @authors
 * Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 *
 * @todo @b doc[PC]: dodelat create db
 * @todo @b doc[TV]: see -> ref -> mainpage u základních pojmů
 * @todo @b doc: sjednotit malá/velká písmena parametrů a návratových hodnot (zatím to vypadá jak "každý pes, jiná ves")
 */

#ifndef VTAPI_H
#define	VTAPI_H

#include "vtapi_config.h"
#include "vtapi_commons.h"

// virtual definitions of classes, which are contained in this header
// you can use it as an index jump to the appropriate class defitinition
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
 * @brief This represents the Key (of the Key-Value concept)
 *
 * Used in queries (size>0 for vectors)
 *
 * @note You can use size=-1 for NULL :)
 */
class TKey {
// Members
public:
    String type;    /**< Name of the data type */
    String key;     /**< Name of the column */
    int size;       /**< "0" is the value right now */   // you can use -1 for NULL :)
    String from;    /**< The source (table) */

// Methods
public:
    /** 
     * Default constructor
     */
    TKey() : size(-1) {};

    /**
     * Copy constructor
     * @param orig key to coppy
     */
    TKey(const TKey& orig) 
            : type(orig.type), key(orig.key), from(orig.from), size(orig.size) {};

    /**
     * Constructor for full specification of arguments
     * @param type name of a data type
     * @param key name of a column
     * @param size "0" is the value right now
     * @param from distinguish between in/out right now
     * @todo @b doc: asi zobecnit - použito v keyvalues, method, query, video, vtapi a ne ke všemu to, dle mého, sedí
     */
    TKey(const String& type, const String& key, const int size, const String& from = "")
            : type(type), key(key), size(size), from(from) {};

    
    /**
     * Print data
     * @return string of TKey data
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
    String queryString; /**< This is used for (explicit) queries */
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
     * @deprecated
     * Unimplemented
     * @note marked as deprecated, because there is no discouraged mark
     * @param key
     * @return success
     */
    bool keyValue(const TKey& key);

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times as:
     * @param key
     * @param value
     * @param from table optional
     * @return success
     * @todo @b doc: pro každou funkci; není příliš jasné, k čemu ta funkce je (+ from).
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
     * @todo @b doc: Petrovy závorky
     */
    bool setTable(const String& table);
    String table;       /**< This is where the (single) table/selection is stored @todo @b doc: Petrovy závorky */

    // FIXME: use keys instead of this
    /**
     * This is a WHERE statement construction class for "Strings"
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param table table where the key is situated
     * @return
     * @todo @b doc: snad je table míněno takto
     * @todo @b code: vždy vrací true?
     */
    bool whereString(const String& key, const String& value, const String& oper = "=", const String& table = "");

    /**
     * This is a WHERE statement construction class for integers
     * It can be called several times and for arrays as:
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param table table where the key is situated
     * @return
     * @todo @b doc: "and for arrays"? co tím autor myslel? Případně reflektovat u ostatních whereX funkcí
     * @todo @b doc: snad je table míněno takto
     * @todo @b code: vždy vrací true?
     */
    bool whereInt(const String& key, const int value, const String& oper = "=", const String& table = "");

    /**
     * This is a WHERE statement construction class for floats
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param table table where the key is situated
     * @return
     * @todo @b doc: snad je table míněno takto
     * @todo @b code: vždy vrací true?
     */
    bool whereFloat(const String& key, const float value, const String& oper = "=", const String& table = "");
    // FIXME: see above :(
    String where;   /**< This is used either for direct entry of WHERE statement or its build from whereX functions */

protected:
    /**
     * @param key
     * @param table
     * @return
     * @todo @b doc: doplnit
     * @todo @b code: není chyba v escapování při zadání klíče i tabulky? Je tam escapovaný název tabulky, ale nepřidává se k němu nikde klíč..
     */
    String escapeColumn(const String& key, const String& table = "");
};

/**
 * @brief This is a class where queries are (to be) constructed
 * 
 * Mechanism: TBD
 *
 * @note Errors 21*
 *
 * @todo @b doc: Petrovy závorky
 * @todo @b doc: "Mechanism: TBD" - vysvětlit či zničit
 */
class Select : public Query {
// Members
public:
    // TODO: this->from["intervals"] = "*";
    // FIXME: use keys
    std::multimap<String, String> fromList; /**< This is a tuple <table name, column name> */

    String groupby; /**< String used for the GROUP BY statement */
    String orderby; /**< String used for the ORDER BY statement */

    int limit;  /**< Specify a size (a number of rows) of the resultset */
    int offset; /**< Specify an index of row, where the resultset starts */

// Methods
public:
    /**
     * Construct a SELECT query object
     * @param commons pointer of the existing commons object
     * @param queryString query string
     * @param param parameters for passing to the query
     */
    Select(const Commons& commons, const String& queryString = "", PGparam *param = NULL);

    /**
     * This expands the query, so you can check it before the execution
     * @return string value with the SQL SELECT query
     */
    String getQuery();

    /**
     * This is used to specify the table for FROM statement and the column list for SELECT statement
     * It may be called more times.
     * @param table table to select
     * @param column column for select
     * @return success
     * @todo @b code: vrací vždy true
     */
    bool from(const String& table, const String& column);
#ifdef BLABLA
        int from(const String& table, const String& column);

   #endif

    /**
     * This is used to specify a function in the (column) list
     * It may be called more times.
     * @param funtext
     * @return success
     * @todo @b code: neimplementováno
     */
    bool function(const String& funtext);

    /**
     * This is used to join tables if they can be performed automatically.
     * If not, returns false (no quarantee before version 2).
     * @warning no quarantee before version 2 !!
     * @return success
     * @todo @b code: neimplementováno
     */
    bool join();

    /**
     * This is used to discard current resultset and fetch next one with updated
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
 * @todo @b doc: Petrovy závorky
 * @todo @b doc: "Mechanism: TBD" - vysvětlit či zničit
 */
class Insert : public Query {
public:
    /**
     * Construct a INSERT command object
     * @param commons pointer of the existing commons object
     * @param insertString query string
     * @param param parameters for passing to the query
     */
    Insert(const Commons& commons, const String& insertString = "", PGparam *param = NULL);

    /**
     * This expands the query, so you can check it before the execution
     * @return string value with SQL INSERT command
     */
    String getQuery();
};

/**
 * @brief This is a class where queries are (to be) constructed
 *
 * Mechanism: TBD
 * 
 * @todo <b>nějaký artefakt?:</b> in the future version (1.0), this class will use Select (whereKV)
 *
 * @warning This class used unproperly may destroy the life and the universe.
 * RECOMENDATION: Wait for the version 1.0.
 *
 * @note Error codes 23*
 *
 * @todo @b doc: buď zneškodnit předchozí TODO ("nějaký artefakt?") a nebo tam napsat správnou verzi (todo, recomendation); v závislosti na předchozím výběru taky zkontrolovat, zda tu doc sedí (todo, recomendation, warning).
 * @todo @b doc: Petrovy závorky
 * @todo @b doc: "Mechanism: TBD" - vysvětlit či zničit
 */
class Update : public Query {
public:
    /**
     * Constructor
     * @param commons       parent (this)
     * @param queryString   to be performed
     * @param param         passed to the query
     */
    Update(const Commons& commons, const String& queryString = "", PGparam *param = NULL);

    /**
     * This expands the query, so you can check it before the execution
     * @return string value with SQL UPDATE command
     */
    String getQuery();
};



// ************************************************************************** //
/**
 * @brief KeyValues storage class
 *
 * @see Basic definition of term @ref LOGICAL
 *
 * @note Error codes 30*
 *
 * @todo @b doc: sjednotit "get a ... by a ..." vs. "get ... by the ..."
 */
class KeyValues : public Commons {
// Members
public:
    Select* select; /**< Select is (to be) pre-filled by the constructor @todo @b doc: Petrovy závorky */
    int pos;        /**< N-th tuple of the resultset; initialized to -1 by default */
    Insert* insert; /**< New insert to insert new data */
    Update* update; /**< New update to update new data */
    // some other inherited from @link Commons

// Methods
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
     * @return list of keys
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
     * @return character array
     */
    char *getCharA(const String& key, int& size);
    /**
     * Get character array specified by column index
     * @param col column index
     * @param size size of the array of char values
     * @return character array
     */
    char *getCharA(const int pos, int& size);
    /**
     * Get a character array specified by a column key
     * @param key column key
     * @return character array
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
     * Get long integer value specified by a column key
     * @param key column key
     * @return long integer value
     */
    long getInt8(const String& key);
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
     * @return vector of integer values
     */
    std::vector<int>* getIntV(const int col);
    /**
     * Get a vector of integer values specified by a column key
     * @param key column key
     * @return vector of integer values
     */
    std::vector<int>* getIntV(const String& key);
    /**
     * Get a vector of integer vectors specified by an index of a column
     * @param col index of column
     * @return  vector of vectors of integer values
     */
    std::vector< std::vector<int>* >* getIntVV(const int col);
    /**
     * Get a vector of integer vectors specified by a column key
     * @param key column key
     * @return  vector of vectors of integer values
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
     * @return double value
     */
    double getFloat8(const String& key);
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
     * @return vector of integer values
     */
    std::vector<float>* getFloatV(const int col);
    /**
     * Get a vector of float values specified by the column index
     * @param key column key
     * @return vector of float values
     */
    std::vector<float>* getFloatV(const String& key);

    //TODO: is it needed a vector of float vectors as in case of integers?

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
     * @note polygon.pts must be copied out if needed after clearing resultset
     *          copy_points(polygon.npts, polygon.pts, ...);
     * @param key column key
     * @return Polygon
     */
    PGpolygon getPolygon(const String& key);
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
    PGpath getPath(const String& key);
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
    PGcube getCube(const String& key);
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
     * @todo @b code: neimplementováno
     */
    virtual bool preSet();


    /**
     * Set a new string value of the specified key
     * @param key column key to update
     * @param value new string value of the key
     * @return
     * @todo @b code: bez návratové hodnoty (doplnit pak do doc)
     */
    bool setString(const String& key, const String& value);

    /**
     * Set a new integer value of the specified key
     * @note New integer value is casted from string value
     * @param key column key to update
     * @param value new integer value (in string representation) of the key
     * @return
     * @todo @b code: bez návratové hodnoty (doplnit pak do doc)
     */
    bool setInt(const String& key, const String& value);

    /**
     * Set a new integer value of the specified key
     * @param key column key to update
     * @param value new integer value of the key
     * @return
     * @todo @b code: bez návratové hodnoty (doplnit pak do doc)
     */
    bool setInt(const String& key, int value);

    /**
     * Set a new integer array of the specified key
     * @param key column key to update
     * @param values new integer array of the key
     * @param size size of the array of integer values
     * @return success
     */
    bool setIntA(const String& key, const int* values, int size);

    /**
     * Set a new integer vector of the specified key
     * @param key column key to update
     * @param values new integer vector of the key
     * @return success
     * @unimplemented
     */
    bool setIntV(const String& key, const std::vector<int> values);

    /**
     * Set a new float value of the specified key
     * @note New flaot value is casted from string value
     * @param key column key to update
     * @param value new float value (in string representation) of the key
     * @return success
     */
    bool setFloat(const String& key, const String& value);

    /**
     * Set a new float value of the specified key
     * @param key column key to update
     * @param value new float value of the key
     * @return success
     * @todo @b code: bez návratové hodnoty (doplnit pak do doc)
     */
    bool setFloat(const String& key, float value);

    /**
     * Set a new float array of the specified key
     * @param key column key to update
     * @param values new float array of the key
     * @param size size of the array of integer values
     * @return
     * @todo @b code: bez návratové hodnoty (doplnit pak do doc)
     */
    bool setFloatA(const String& key, const float* values, int size);

    /**
     * Set a new float vector of the specified key
     * @param key column key to update
     * @param values new float vector of the key
     * @return
     * @todo @b code: neimplementováno (doplnit pak do doc návratovou hodnotu)
     */
    bool setFloatV(const String& key, const std::vector<float> values);

    /**
     * Execute SQL UPDATE command
     * @return success
     */
    bool setExecute();
    
    // =============== ADDERS (Insert) ========================================
    // TODO: implement?
    /**
     *
     * @param key
     * @param value
     * @return
     * @todo @b code: addX neimplementováno
     */
    bool addString(const String& key, const String& value);
    bool addInt(const String& key, const String& value);
    bool addInt(const String& key, int value);
    bool addIntA(const String& key, int* value, int size);
    bool addFloat(const String& key, const String& value);
    bool addFloat(const String& key, float value);
    bool addFloatA(const String& key, float* value, int size);

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

protected:
    String caption; /**< table caption */
    String tableOpt; /**< custom table options (border, padding, whatever..) */

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
     * This goes through resultset and retrieves metadata necessary for print.
     * @note It needs to be done before every print.
     * @param row if not set to -1, this indicates single row print
     * @param indicator whether column widths will be required
     * @return metadata for print, pair consisting of two vectors:
     *  a) Tkeys - column types etc., b) ints - column widths
     */
    std::pair< std::vector<TKey>*,std::vector<int>* > getFieldsInfo(const int row = -1, int get_widths = 1);
};



/**
 * @brief This class should always be on the path of your programm...
 *
 * @see Basic definition of term @ref LOGICAL
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
     * @param orig pointer to the parent KeyValues object
     * @param name specific dataset name
     * @todo @b doc: nejsem si jist, zda i zde je parametr "const KeyValues& orig" nadřazený prvek
     */
    Dataset(const KeyValues& orig, const String& name = "");

    /**
     * Move to a next dataset and set dataset name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
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
 * @see Basic definition of term @ref LOGICAL
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
     * @param orig pointer to the parrent KeyValues object
     * @param name specific sequence name
     */
    Sequence(const KeyValues& orig, const String& name = "");

    /**
     * Move to a next sequence and set sequence name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
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
     * Add new sequence to a database table
     * @param name name of the sequence
     * @param location location of the sequence
     * @param type type of the sequence
     * @return success
     * @todo @code: bez návratové hodnoty
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
     *
     * @param name
     * @return
     * @todo @b code: neimplementováno (potom doplnit i doc)
     */
    Process* newProcess(const String& name = "");

#ifdef __OPENCV_CORE_HPP__
    /**
     *
     * @return
     * @todo @b code: neimplementováno (potom doplnit i doc)
     */
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
     * @param orig pointer to the parrent KeyValues object
     * @param name specific sequence name
     */    
    Video(const KeyValues& orig, const String& name);

    /**
     * Create a new frame specified by the frame number
     * @param name name of the image
     * @return pointer to the new image
     * @todo @b code: neimplementováno
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
     * @todo @b code: bez návratové hodnoty
     */
    bool add(String name, String location);


#ifdef __OPENCV_CORE_HPP__
    /**
     * @todo Test
     * @param name
     * @return
     * @todo @b code: neimplementováno (potom doplnit i doc)
     */
    bool openVideo();

    /**
     *
     * @return
     * @todo @b code: neimplementováno (potom doplnit i doc)
     */
    cv::Mat getNextImage();
#endif

};



#ifdef __OPENCV_HIGHGUI_HPP__
/**
 * @brief This is the ever-simplest video player...
 *
 * VideoPlayer makes copies of each object, so it doesn't affect nexts() performed elsewhere,
 * however, it may fail in case of next, where are hundreds of thousands of tuples (@see Keyvalues)
 * // TODO: This behavior might be changed later
 *
 *  @note Error codes 16*
 *
 *  @todo @b doc: třída prakticky bez dokumentace
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
     * A void constructor - plays nothing at all at the moment
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
     * ... or a default capture in case of none (can be used to store the capture)
     * @return
     * @todo @b code: bez návratové hodnoty (potom doplnit i do doc)
     */
    bool play();
};

#endif




/**
 * @brief Interval is equivalent to an interval of images
 *
 * @see Basic definition of term @ref LOGICAL
 *
 * @note Error codes 33*
 */
class Interval : public KeyValues {
public:
    // TODO: destroy in destructor if doom
    Sequence* parentSequence; /**< This is because of image and video load && getSequence() documented. @todo @b doc: co tím chtěl autor říci? */
    bool parentSequenceDoom; /**< this is whether to destroy the above */

public:
    /**
     * Constructor for intervals
     * @param orig pointer to the parrent KeyValues object
     * @param selection specific name of a selection table
     */
    Interval(const KeyValues& orig, const String& selection = "intervals");

    // bool next(); not necessary

    /**
     * This is to (fast)
     * @return sequence string
     * @todo @b doc: Co tím popiskem "This is to (fast)" chtěl auto říci?
     */
    String getSequenceName();
    
    /**
     * @warning This function has changed the return signature in version pre2!
     *          You may use getSequenceName()...
     *
     * This is used to query (if needed) the Sequences (may be slow for continuous use)
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
     * Add new interval to the table
     * @param sequence interval name
     * @param t1 start time
     * @param t2 end time
     * @param location location of the image
     * @return success
     * @todo @b code: vrací vždy true
     */
    bool add(const String& sequence, const int t1, const int t2 = -1, const String& location = "");

    /**
     * This is used to support updates
     * @return success (in derived classes)
     * @todo @b code: vrací vždy true
     */
    bool preSet();
};

/**
 * @brief This represents images
 *
 * @note Error codes 339*
 */
class Image : public Interval {
// Methods
public:
    /**
     * Constructor for Images
     * @param orig pointer to the parrent KeyValues object
     * @param selection specific name of a selection table
     */
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
     * @todo @b code: Nemělo by se to sjednotit se sekvencí, tedy neměla by se
     *                stringová funkce jmenovat getLocationName?
     */
    String getLocation();

    /**
     * Simply adds an image (interval) to the sequence table (no checking)
     * @param sequence interval name
     * @param t time (start time is the same as the end time)
     * @param location location of the image
     * @return success
     * @todo @b code: bez návratové hodnoty
     */
    bool add(const String& sequence, const int t, const String& location);

protected:
    
};


/**
 * @brief A class which represents methods and gets also their keys
 *
 * @see Basic definition of term @ref LOGICAL
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
     * @param orig pointer to the parrent KeyValues object
     * @param name specific name of method, which we can construct
     */
    Method(const KeyValues& orig, const String& name = "");

    /**
     * Move to a next method and set a method name and its methodkeys variables
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();

    /**
     * Get a name of the current method
     * @return string value with the name of the method
     */
    String getName();
    /**
     * This is used to refresh the methodKeys vector
     * @return vector<TKey>
     */
    std::vector<TKey> getMethodKeys();

    /**
     * Create new process for current dataset
     * @return pointer to new process
     */
    Process* newProcess(const String& name = "");

private:
    /**
     *
     * @param inout
     * @todo code: neimplementováno (pak doplnit i doc)
     */
    void printData(const String& inout);
};

/**
 * @brief A class which represents processes and gets information about them
 *
 * @see Basic definition of term @ref LOGICAL
 *
 * @note Error codes 36*
 */
class Process : public KeyValues {
// Methods
public:
    /**
     * Constructor for processes
     * @param orig pointer to the parrent KeyValues object
     * @param name specific name of process, which we can construct
     */
    Process(const KeyValues& orig, const String& name = "");

    /**
     * Individual next() for processes, which stores current process
     * and selection to commons
     * @note Overloading next() from KeyValues
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
     * @todo @b doc: má to cenu komentovat? :)
     */
    bool add(const String& method, const String& name, const String& selection="intervals");

    /**
     * Create new interval for process
     * // TODO: unused t1, t2
     * @param t1 currently unused
     * @param t2 currently unused
     * @return new interval
     * @todo @b code: Nepoužívané parametry t1, t2
     */
    Interval* newInterval(const int t1 = -1, const int t2 = -1);
    /**
     * Create new sequence for process
     * // TODO: not implemented method
     * @param name specific sequence name
     * @return new sequence
     * @todo @b doc: neimplementováno
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
     * Constructor recomended by any program
     * @param argc
     * @param argv
     * @todo @b doc: "(in the future) je stále aktuální?
     * @todo @b doc: další konstruktory
     */
    VTApi(int argc, char** argv);

    /**
     * Constructor
     * @deprecated
     * @param configFile location
     */
    VTApi(const String& configFile);

    /**
     * Constructor
     * @deprecated
     * @param connStr
     * @param location
     * @param user
     * @param password
     */
    VTApi(const String& connStr, const String& location, const String& user, const String& password);

    /**
     * Copy constructor
     * @param orig
     */
    VTApi(const Commons& orig);

    /**
     * Copy constructor
     * @param orig
     */
    VTApi(const VTApi& orig);

    /**
     * Destructor
     */
    virtual ~VTApi();


    /**
     * This might be a HOW-TO function for learning and testing purposes
     * @see doxygen -> vtapi.conf, samples.cpp
     * @code
     */
    void test();


    /**
     * This is how to continue after creating the API class...
     * @param name specific dataset name
     * @return new dataset
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
 * @todo @b doc: parametry konstruktorů
 */
template <class T>
class TKeyValue : public TKey {
// Members
public:    
    String typein; /**< This attribute is there for validation */
    T* values; /**< @todo @b doc: doplnit */

// Methods
public:
    /**
     * Default constructor
     */
    TKeyValue() : TKey(), values(NULL) {};

    /**
     *
     * @param type
     * @param key
     * @param value
     * @param from table (optional)
     */
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

