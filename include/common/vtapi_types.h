
#pragma once

namespace vtapi {

// type definition:
typedef unsigned int DBTYPE;
//////////////////////////////////////////////////////////////////////
// |       2B         |        1B         |           1B             |
// |  type length[B]  |   DB_TYPE_FLAGs   |     DBTYPE_CATEGORY_T    |
//////////////////////////////////////////////////////////////////////

#define DBTYPE_CATEGORY_MASK    ((DBTYPE)(0xFF))
#define DBTYPE_FLAG_MASK        ((DBTYPE)(0xFF00))
#define DBTYPE_LENGTH_MASK      ((DBTYPE)(0xFFFF0000))

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
    DBTYPE_UD_PSTATUS,      // user defined - process status enum
    DBTYPE_UD_CVMAT,        // user defined - OpenCV matrix
    DBTYPE_UD_EVENT,        // user defined - VT event
    DBTYPE_UD_PSTATE,       // user defined - process state
    DBTYPE_REF_TYPE,        // database type
    DBTYPE_REF_CLASS,       // database class (column ...)
    DBTYPE_MAX = 0xFF
} DBTYPE_CATEGORY;

// additional flags for type categories
typedef enum
{
    DBTYPE_FLAG_NONE        = (0),
    DBTYPE_FLAG_ARRAY       = (1 << 8),     // type is actually array of other type
    DBTYPE_FLAG_NUMERIC     = (1 << 9),     // type is numeric
    DBTYPE_FLAG_GEOMETRIC   = (1 << 10),    // type is geometric
    DBTYPE_FLAG_USERDEFINED = (1 << 11),    // type was created by vtapi
    DBTYPE_FLAG_REFTYPE     = (1 << 12)     // type is database meta-type
} DBTYPE_FLAG;

#define DBTYPE_GETCATEGORY(x)       (DBTYPE_CATEGORY)(x & DBTYPE_CATEGORY_MASK)
#define DBTYPE_SETCATEGORY(x,c)     {x = (x & ~DBTYPE_CATEGORY_MASK) | (c & DBTYPE_CATEGORY_MASK);}
#define DBTYPE_HASFLAG(x,f)         (bool)((x & f) != 0)
#define DBTYPE_GETFLAGS(x)          (DBTYPE_FLAG)(DBTYPE_CATEGORY)(x & DBTYPE_FLAG_MASK)
#define DBTYPE_SETFLAGS(x,f)        {x |= (f & DBTYPE_FLAG_MASK);}
#define DBTYPE_SETCATEGORYFLAGS(x,cf) {DBTYPE_SETCATEGORY(x,cf);DBTYPE_SETFLAGS(x,cf);}
#define DBTYPE_GETLENGTH(x)         (short)((x & DBTYPE_LENGTH_MASK) >> 16)
#define DBTYPE_SETLENGTH(x,l)       {x = (x & ~DBTYPE_LENGTH_MASK) | (((DBTYPE)l << 16) & DBTYPE_LENGTH_MASK);}

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
