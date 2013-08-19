/* 
 * File:   vtapi_typemanager.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:39 PM
 */

#ifndef VTAPI_TYPEMANAGER_H
#define	VTAPI_TYPEMANAGER_H

namespace vtapi {
    class TypeManager;
    class PGTypeManager;
    class SLTypeManager;
}

#include "vtapi_libloader.h"
#include "vtapi_connection.h"
#include "../common/vtapi_logger.h"
#include "../common/vtapi_global.h"

namespace vtapi {

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


typedef map<int,string>                 oid_map_t;
typedef map<string, type_metadata_t>    types_map_t;


//TODO: comment
class TypeManager {
protected:

    fmap_t          *fmap;      /**< function address book */
    Connection      *connection;    /**< connection object */
    Logger          *logger;        /**< logger object for output messaging */
    string          thisClass;      /**< class name */

    oid_map_t       oid_map;        /**< types indexed by their OID */
    types_map_t     types_map;      /**< types metadata */

public:

    TypeManager(fmap_t *fmap, Connection *connection, Logger *logger) {
        this->logger        = logger;
        this->connection    = connection;
        this->fmap          = fmap;
    };
    virtual ~TypeManager() { };

    string toTypname(int oid) {        
        return (oid_map.count(oid) > 0) ? oid_map[oid] : string("");
    };

    type_metadata_t getTypeMetadata(const string& name) {
        return (types_map.count(name) > 0) ? types_map[name] : type_metadata_t();
    }

protected:

    virtual bool loadTypes() = 0;


};


class PGTypeManager : public TypeManager {
private:

    set<string> reftypes;

    

public:

    PGTypeManager(fmap_t *fmap, Connection *connection, Logger* logger);
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


class SLTypeManager : public TypeManager {
private:

public:

    SLTypeManager(fmap_t *fmap, Connection *connection, Logger* logger);
    ~SLTypeManager();

protected:
    
    bool loadTypes();

};

} // namespace vtapi

#endif	/* VTAPI_TYPEMANAGER_H */

