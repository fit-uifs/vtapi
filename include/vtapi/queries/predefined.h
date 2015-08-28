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


class QueryPredefined : public Query
{
public:
    QueryPredefined(const Commons& commons);
    virtual ~QueryPredefined();
};

class QueryBeginTransaction : public QueryPredefined
{
public:
    QueryBeginTransaction(const Commons& commons);
};

class QueryCommitTransaction : public QueryPredefined
{
public:
    QueryCommitTransaction(const Commons& commons);
};

class QueryRollbackTransaction : public QueryPredefined
{
public:
    QueryRollbackTransaction(const Commons& commons);
};

class QueryDatasetCreate : public QueryPredefined
{
public:
    QueryDatasetCreate(
        const Commons& commons,
        const std::string& name,
        const std::string& location,
        const std::string& friendly_name,
        const std::string& description);
};

class QueryDatasetReset : public QueryPredefined
{
public:
     QueryDatasetReset(const Commons& commons,
                       const std::string& name);
};

class QueryDatasetDelete : public QueryPredefined
{
public:
     QueryDatasetDelete(const Commons& commons,
                        const std::string& name);
};

class QueryMethodCreate : public QueryPredefined
{
public:
    QueryMethodCreate (const Commons& commons,
                       const std::string& name,
                       const MethodKeys keys_definition,
                       const MethodParams params_definition,
                       const std::string& description);
};

class QueryMethodDelete : public QueryPredefined
{
public:
    QueryMethodDelete(const Commons& commons,
                      const std::string& name);
} ;

class QueryTaskCreate : public QueryPredefined
{
public:
    QueryTaskCreate(const Commons& commons,
                    const std::string& name,
                    const std::string& dsname,
                    const std::string& mtname,
                    const std::string& params,
                    const std::string &prereq_task,
                    const std::string& outputs);
};

class QueryTaskDelete : public QueryPredefined
{
public:
    QueryTaskDelete(const Commons& commons,
                    const std::string &dsname,
                    const std::string &taskname);
};

class QueryLastInsertedId : public QueryPredefined
{
public:
    QueryLastInsertedId(const Commons& commons);
    bool execute() override;
    int getLastId();

private:
    int _last_id;
};


}
