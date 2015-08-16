/**
 * @file
 * @brief   Declaration of Query class which is general class for SQL queries/commands.
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "query.h"
#include "../data/methodkeys.h"
#include "../data/methodparams.h"

namespace vtapi {


class QueryBeginTransaction : public Query
{
public:
    QueryBeginTransaction (const Commons& commons);
};

class QueryCommitTransaction : public Query
{
public:
    QueryCommitTransaction (const Commons& commons);
};

class QueryRollbackTransaction : public Query
{
public:
    QueryRollbackTransaction (const Commons& commons);
};

class QueryDatasetCreate : public Query
{
public:
    QueryDatasetCreate (
        const Commons& commons,
        const std::string& name,
        const std::string& location,
        const std::string& friendly_name,
        const std::string& description);
};

class QueryDatasetReset : public Query
{
public:
     QueryDatasetReset(const Commons& commons, const std::string& name);
};

class QueryDatasetDelete : public Query
{
public:
     QueryDatasetDelete(const Commons& commons, const std::string& name);
};

class QueryMethodCreate : public Query
{
public:
    QueryMethodCreate (
        const Commons& commons,
        const std::string& name,
        const MethodKeys keys_definition,
        const MethodParams params_definition,
        const std::string& description);
};

class QueryMethodDelete : public Query
{
public:
    QueryMethodDelete(const Commons& commons, const std::string& name);
} ;

class QuerySequenceDelete : public Query
{
public:
    QuerySequenceDelete(const Commons& commons, const std::string& name);
} ;

class QueryLastInsertedId : public Query
{
public:
    QueryLastInsertedId(const Commons& commons);
    bool execute(int &returned_id);

};


}
