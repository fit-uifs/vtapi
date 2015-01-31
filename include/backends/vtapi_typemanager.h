/* 
 * File:   vtapi_typemanager.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:39 PM
 */

#ifndef VTAPI_TYPEMANAGER_H
#define	VTAPI_TYPEMANAGER_H

#include "vtapi_backendlibs.h"
#include "../common/vtapi_logger.h"

namespace vtapi {

class Connection;

typedef enum {
    TYPE_UNDEFINED = 0,
    TYPE_ARRAY,
    TYPE_BOOLEAN,
    TYPE_COMPOSITE,
    TYPE_DATE,
    TYPE_ENUM,
    TYPE_GEOMETRIC,
    TYPE_NUMERIC,
    TYPE_STRING,
    TYPE_USERDEFINED,
    TYPE_REFTYPE,
    TYPE_UNKNOWN
} type_category_t;

typedef struct {
    type_category_t category;
    short           length;
    type_category_t category_elem;
    short           length_elem;
} type_metadata_t;


typedef std::map<int,std::string>                 oid_map_t;
typedef std::map<std::string, type_metadata_t>    types_map_t;


/**
 * @brief Class handles registering user defined types (mostly PostgreSQL) and
 * retrieving information about data types.
 */
class TypeManager {
public:
    typedef enum {
        TYPE_SEQTYPE,
        TYPE_INOUTTYPE,
        TYPE_CVMAT
    } USER_DEFINED_TYPE;
    
protected:

    fmap_t          *fmap;          /**< function address book */
    Connection      *connection;    /**< connection object */
    Logger          *logger;        /**< logger object for output messaging */
    std::string     thisClass;      /**< class name */

    oid_map_t       oid_map;        /**< types indexed by their OID */
    types_map_t     types_map;      /**< types metadata */

    std::string     schema;         /**< types schema */
public:

    /**
     * Constructor
     * @param fmap function address book
     * @param connection database connection object
     * @param logger logger object
     */
    TypeManager(fmap_t *fmap, Connection *connection, Logger *logger, std::string &schema) {
        this->logger        = logger;
        this->connection    = connection;
        this->fmap          = fmap;
        this->schema        = schema;
    };
    /**
     * Virtual destructor
     */
    virtual ~TypeManager() { };

    /**
     * Convert database OID to type name
     * @param oid type OID
     * @return type name
     */
    std::string toTypname(int oid) {        
        return (oid_map.count(oid) > 0) ? oid_map[oid] : std::string("");
    };

    /**
     * Retrieves metadata(category, length) about given data type
     * @param name type name
     * @return type metadata struct
     */
    type_metadata_t getTypeMetadata(const std::string& name) {
        return (types_map.count(name) > 0) ? types_map[name] : type_metadata_t();
    }

    /**
     * Retrieve user defined type name for schema
     * @param type type enum value
     * @param prefix prefix for return value
     * @return string type string
     */
    std::string getTypeName(USER_DEFINED_TYPE type, const char *prefix = NULL) {
        std::stringstream ss;
        if (prefix) ss << prefix;
        ss << this->schema << ".";
        switch(type) {
            case TYPE_SEQTYPE:  ss << "seqtype"; break;
            case TYPE_INOUTTYPE:ss << "inouttype"; break;
            case TYPE_CVMAT:    ss << "cvmat"; break;
        }
        return ss.str();
    }
protected:

    /**
     * Loads all available database data types
     * @return success
     */
    virtual bool loadTypes() = 0;

};

#if HAVE_POSTGRESQL
class PGTypeManager : public TypeManager {
private:

    std::set<std::string>reftypes;

    

public:

    PGTypeManager(fmap_t *fmap, Connection *connection, Logger* logger, std::string &schema);
    ~PGTypeManager();

    int enum_put (PGtypeArgs *args);
    int enum_get (PGtypeArgs *args);

protected:

    bool loadTypes();
    //void loadRefTypes();
    bool registerTypes();
    void loadRefTypes();
    type_category_t mapCategory(char category_char);
    
};

#endif

#if HAVE_SQLITE
class SLTypeManager : public TypeManager {
private:

public:

    SLTypeManager(fmap_t *fmap, Connection *connection, Logger* logger, std::string &schema);
    ~SLTypeManager();

protected:
    
    bool loadTypes();

};
#endif

} // namespace vtapi

#endif	/* VTAPI_TYPEMANAGER_H */

