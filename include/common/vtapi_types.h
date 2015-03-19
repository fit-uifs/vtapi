
#ifndef VTAPI_TYPES_H
#define VTAPI_TYPES_H

namespace vtapi {

// type definition:
typedef unsigned int DBTYPE;
//////////////////////////////////////////////////////////////////////
// |       2B         |        1B         |           1B             |
// |  type length[B]  |   DB_TYPE_FLAGs   |     DBTYPE_CATEGORY_T    |
//////////////////////////////////////////////////////////////////////

// category indicates how should type value be retrieved
typedef enum
{
    DBTYPE_UNDEFINED = 0,
    DBTYPE_STRING,          // name, text, varchar ...
    DBTYPE_INT,             // 4/8 bytes ints
    DBTYPE_FLOAT,           // 4/8 bytes floats
    DBTYPE_BOOLEAN,         // booleans
    DBTYPE_BLOB,            // binary data
    DBTYPE_TIMESTAMP,       // without time zone
    DBTYPE_GEO_POINT,       // geometric - point
    DBTYPE_GEO_LSEG,        // geometric - line segment
    DBTYPE_GEO_PATH,        // geometric - path
    DBTYPE_GEO_BOX,         // geometric - bounding box
    DBTYPE_GEO_POLYGON,     // geometric - polygon
    DBTYPE_GEO_LINE,        // geometric - line
    DBTYPE_GEO_CIRCLE,      // geometric - circle
    DBTYPE_GEO_GEOMETRY,    // geometric - generic PostGIS geometry type
    DBTYPE_UD_SEQTYPE,      // user defined - sequence type
    DBTYPE_UD_INOUTTYPE,    // user defined - in/out enum typ
    DBTYPE_UD_CVMAT,        // user defined - OpenCV matrix
    DBTYPE_UD_EVENT,        // user defined - VT event
    DBTYPE_REF_TYPE,        // database type
    DBTYPE_REF_CLASS        // database class (column ...)
} DBTYPE_CATEGORY;

// additional flags for type categories
typedef enum
{
    DBTYPE_FLAG_NONE        = (0),
    DBTYPE_FLAG_ARRAY       = (1),      // type is actually array of other type
    DBTYPE_FLAG_NUMERIC     = (1 << 1), // type is numeric
    DBTYPE_FLAG_GEOMETRIC   = (1 << 2), // type is geometric
    DBTYPE_FLAG_USERDEFINED = (1 << 3), // type was created by vtapi
    DBTYPE_FLAG_REFTYPE     = (1 << 4)  // type is database meta-type
} DBTYPE_FLAG;

#define DBTYPE_GETCATEGORY(x)       (DBTYPE_CATEGORY)(x & 0xFF)
#define DBTYPE_SETCATEGORY(x,c)     (x = (x | (c & 0xFF)))
#define DBTYPE_HASFLAG(x,f)         (bool)((((x & 0xFF00) >> 8) & f) != 0)
#define DBTYPE_SETFLAG(x,f)         (x = (x | ((f & 0xFF) << 8)))
#define DBTYPE_GETCATEGORYFLAGS(x)  (short)(x & 0xFFFF)
#define DBTYPE_SETCATEGORYFLAGS(x,f) (x = (x | (f & 0xFFFF)))
#define DBTYPE_GETLENGTH(x)         (short)((x >> 16) & 0xFFFF)
#define DBTYPE_SETLENGTH(x,l)       (x = ((x & (DBTYPE)0xFFFF) | ((l & 0xFFFF) << 16)))

// type definition and map
typedef struct _DBTYPE_DEFINITION_T
{
    DBTYPE type;
    std::string name;
} DBTYPE_DEFINITION_T;

typedef std::map<int,DBTYPE_DEFINITION_T> DBTYPES_MAP;
typedef std::map<int,DBTYPE_DEFINITION_T>::iterator DBTYPES_MAP_IT;
typedef std::pair<int, DBTYPE_DEFINITION_T> DBTYPES_PAIR;


} // namespace

#endif