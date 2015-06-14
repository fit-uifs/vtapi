/**
 * This page describes proper installation and usage of custom data types in VTApi.
 *
 * @section 1. Define new data type
 * Execute CREATE TYPE in your PostgreSQL database to install a new data type.
 * For input/output to work, it is first necessary to create type's send and
 * receive functions (CREATE FUNCTION) and then properly assign them to the type
 * (parameters SEND and RECEIVE).
 * 
 * Reference manual for CREATE TYPE is found here (examples at the bottom):
 *      http://www.postgresql.org/docs/9.1/static/sql-createtype.html
 *
 * 
 * @section 2. Register data type
 * Data manipulation on a client side is handled by libpqtypes (1.7) system.
 *      http://libpqtypes.esilo.com/
 *
 * All non-standard data types must be registered before any manipulation with
 * them occurs. This is done by PGregisterTypes() function. Types may require
 * definition of handler functions (get, put) depending on what type category
 * they are - user-defined (necessary), composite (not) or subclass (optional).
 *
 * Reference manual on data types registering (with example):
 *      http://libpqtypes.esilo.com/man3/PQregisterTypes.html
 *
 * Writing data type handlers (with examples):
 *      http://libpqtypes.esilo.com/man3/pqt-handlers.html
 *
 *
 * @section 3. Using a data type
 * Retrieve values from a resultset using PQgetf() function. Put values using
 * parametrized queries and PQputf() function.
 *
 * Reference manual on putting and getting data types (with examples):
 *      http://libpqtypes.esilo.com/man3/pqt-specs.html
 * 
 */

#pragma once

#include <string>
#include <vector>

namespace vtapi {

std::string base64_encode(const void *data, size_t data_size);
std::vector<unsigned char> base64_decode(const std::string& encoded);

}