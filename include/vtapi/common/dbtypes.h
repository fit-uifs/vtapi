#pragma once

#include <vtapi/common/exception.h>
#include <vtapi/common/serialize.h>
#include <string>
#include <map>


namespace vtapi {


class DatabaseTypes
{
public:
    enum TypeCategory
    {
        CATEGORY_NONE = 0,
        CATEGORY_STRING,          // name, text, varchar ...
        CATEGORY_INT,             // 4/8 bytes ints
        CATEGORY_FLOAT,           // 4/8 bytes floats
        CATEGORY_BOOLEAN,         // booleans
        CATEGORY_BLOB,            // binary data
        CATEGORY_TIMESTAMP,       // without time zone
        CATEGORY_GEO_POINT,       // geometric - point
        CATEGORY_GEO_LSEG,        // geometric - line segment
        CATEGORY_GEO_PATH,        // geometric - path
        CATEGORY_GEO_BOX,         // geometric - bounding box
        CATEGORY_GEO_POLYGON,     // geometric - polygon
        CATEGORY_GEO_LINE,        // geometric - line
        CATEGORY_GEO_CIRCLE,      // geometric - circle
        CATEGORY_GEO_GEOMETRY,    // geometric - generic PostGIS geometry type
        CATEGORY_UD_SEQTYPE,      // user defined - sequence type
        CATEGORY_UD_INOUTTYPE,    // user defined - in/out enum typ
        CATEGORY_UD_PSTATUS,      // user defined - process status enum
        CATEGORY_UD_CVMAT,        // user defined - OpenCV matrix
        CATEGORY_UD_EVENT,        // user defined - VT event
        CATEGORY_UD_PSTATE,       // user defined - process state
        CATEGORY_UD_EDFDESCRIPTOR,// user defined - EdfDescriptor
        CATEGORY_REF_TYPE,        // database type
        CATEGORY_REF_CLASS        // database class (column ...)
    };

    enum TypeFlag
    {
        FLAG_NONE        = (0),
        FLAG_ARRAY       = (1),          // type is actually array of other type
        FLAG_NUMERIC     = (1 << 1),     // type is numeric
        FLAG_GEOMETRIC   = (1 << 2),     // type is geometric
        FLAG_USERDEFINED = (1 << 3),     // type was created by vtapi
        FLAG_REFTYPE     = (1 << 4)      // type is database meta-type
    };

    /**
     * @brief The TypeDefinition class
     */
    class TypeDefinition
    {
    public:
        short int _length;      // type length in bytes
        short int _category;    // TypeCategory value
        char _flags;            // TypeFlag combination
        std::string _name;      // string name

        TypeDefinition() : _length(0), _category(0), _flags(0) {}
    };


    /**
     * @brief Get type definition
     * @param oid type OID
     * @return reference to definition
     */
    inline const TypeDefinition & type(int oid) const
    {
        const auto it = _data.find(oid);
        if (it == _data.end())
            throw(RuntimeException("Failed to find database type: " + vtapi::toString(oid)));
        else
            return it->second;
    }

    /**
     * @brief Get type definition for modification
     * @param oid type OID
     * @return reference to definition
     */
    inline TypeDefinition & type(int oid)
    {
        auto it = _data.find(oid);
        if (it == _data.end())
            throw(RuntimeException("Failed to find database type: " + vtapi::toString(oid)));
        else
            return it->second;
    }

    /**
     * @brief Insert type to map
     * @param oid type OID
     * @param definition definition
     */
    inline void insert(int oid, const TypeDefinition & definition)
    { _data.insert(std::make_pair(oid, definition)); }

    /**
     * @brief Insert type to map
     * @param oid type OID
     * @param definition definition
     */
    inline void insert(int oid, TypeDefinition && definition)
    { _data.insert(std::make_pair(oid, std::move(definition))); }

private:
    std::map<int,TypeDefinition> _data; /**< database OID to type definition */

};


} // namespace
