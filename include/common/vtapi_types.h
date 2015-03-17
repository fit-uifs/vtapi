
#ifndef VTAPI_TYPES_H
#define VTAPI_TYPES_H

namespace vtapi {

// type definition:
typedef unsigned int VTAPI_DBTYPE;
//////////////////////////////////////////////////////////////////////
// |       2B         |        1B         |           1B             |
// |  type length[B]  |VTAPI_DB_TYPE_FLAGs| VTAPI_DB_TYPE_CATEGORY_T |
//////////////////////////////////////////////////////////////////////

typedef enum
{
    TYPE_UNDEFINED      = 0x00,
    TYPE_BOOLEAN        = 0x01,
    TYPE_COMPOSITE      = 0x02,
    TYPE_DATE           = 0x03,
    TYPE_ENUM           = 0x04,
    TYPE_GEOMETRIC      = 0x05,
    TYPE_NUMERIC        = 0x06,
    TYPE_STRING         = 0x07,
    TYPE_REFTYPE        = 0x08,
    TYPE_UD_SEQTYPE     = 0xF0,
    TYPE_UD_INOUTTYPE   = 0xF1,
    TYPE_UD_CVMAT       = 0xF2,
    TYPE_UD_EVENT       = 0xF3
} VTAPI_DBTYPE_CATEGORY_T;

typedef enum
{
    TYPE_FLAG_USERDEFINED    = 0x01,
    TYPE_FLAG_ARRAY          = 0x02
} VTAPI_DBTYPE_FLAG_T;

#define VTAPI_DBTYPE_GETCATEGORY(x)     (x & 0xFF)
#define VTAPI_DBTYPE_SETCATEGORY(x,c)   (x = x | (c & 0xFF))
#define VTAPI_DBTYPE_HASFLAG(x,f)       (((x & 0xFF00) >> 8) & (f) != 0)
#define VTAPI_DBTYPE_SETFLAG(x,f)       (x = x | ((f & 0xFF) << 8))
#define VTAPI_DBTYPE_GETCATEGORYFLAGS(x)(x & 0xFFFF)
#define VTAPI_DBTYPE_SETCATEGORYFLAGS(x,f) (x = x | (f & 0xFFFF))
#define VTAPI_DBTYPE_GETLENGTH(x)       ((x >> 16) & 0xFFFF)
#define VTAPI_DBTYPE_SETLENGTH(x,l)     (x = ((x & (VTAPI_DBTYPE)0xFFFF) | ((l & 0xFFFF) << 16)))

// type definition and map
typedef struct _VTAPI_DBTYPE_DEFINITION_T
{
    VTAPI_DBTYPE type;
    std::string name;
} VTAPI_DBTYPE_DEFINITION_T;

typedef std::map<int,VTAPI_DBTYPE_DEFINITION_T> VTAPI_DBTYPES_MAP;
typedef std::pair<int, VTAPI_DBTYPE_DEFINITION_T> VTAPI_DBTYPES_PAIR;


} // namespace

#endif