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
#include "../data/taskkeys.h"
#include "../data/taskparams.h"

namespace vtapi {


class QueryPredefined : public Query
{
public:
    QueryPredefined(const Commons& commons)
        : Query(commons, std::string(), false) {}

    QueryPredefined(QueryPredefined &&) = default;
    QueryPredefined & operator=(QueryPredefined &&) = default;

    virtual ~QueryPredefined() {}
};

class QueryBeginTransaction : public QueryPredefined
{
public:
    QueryBeginTransaction(const Commons& commons)
        : QueryPredefined(commons)
    {
        _pquerybuilder->useQueryString(_pquerybuilder->getBeginQuery());
    }
};

class QueryCommitTransaction : public QueryPredefined
{
public:
    QueryCommitTransaction(const Commons& commons)
        : QueryPredefined(commons)
    {
        _pquerybuilder->useQueryString(_pquerybuilder->getCommitQuery());
    }
};

class QueryRollbackTransaction : public QueryPredefined
{
public:
    QueryRollbackTransaction(const Commons& commons)
        : QueryPredefined(commons)
    {
        _pquerybuilder->useQueryString(_pquerybuilder->getRollbackQuery());
    }
};

class QueryDatasetCreate : public QueryPredefined
{
public:
    QueryDatasetCreate(const Commons& commons,
                       const std::string& name,
                       const std::string& location,
                       const std::string& friendly_name,
                       const std::string& description)
        : QueryPredefined(commons)
    {
        std::string sql = _pquerybuilder->getDatasetCreateQuery(name, location,
                                                                friendly_name, description);
        _pquerybuilder->useQueryString(sql);
    }
};

class QueryDatasetReset : public QueryPredefined
{
public:
     QueryDatasetReset(const Commons& commons,
                       const std::string& name)
         : QueryPredefined(commons)
     {
        _pquerybuilder->useQueryString(_pquerybuilder->getDatasetResetQuery(name));
     }
};

class QueryDatasetDelete : public QueryPredefined
{
public:
     QueryDatasetDelete(const Commons& commons,
                        const std::string& name)
         : QueryPredefined(commons)
     {
        _pquerybuilder->useQueryString(_pquerybuilder->getDatasetDeleteQuery(name));
     }
};

class QueryMethodCreate : public QueryPredefined
{
public:
    QueryMethodCreate (const Commons& commons,
                       const std::string& name,
                       const TaskKeyDefinitions keys_definition,
                       const TaskParamDefinitions params_definition,
                       const std::string& description)
        : QueryPredefined(commons)
    {
        std:: string sql = _pquerybuilder->getMethodCreateQuery(name, keys_definition,
                                                               params_definition, description);
        _pquerybuilder->useQueryString(sql);
    }
};

class QueryMethodDelete : public QueryPredefined
{
public:
    QueryMethodDelete(const Commons& commons,
                      const std::string& name)
        : QueryPredefined(commons)
    {
        _pquerybuilder->useQueryString(_pquerybuilder->getMethodDeleteQuery(name));
    }
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
                    const std::string& outputs)
        : QueryPredefined(commons)
    {
        std::string sql = _pquerybuilder->getTaskCreateQuery(name, dsname, mtname,
                                                            params, prereq_task, outputs);
        _pquerybuilder->useQueryString(sql);
    }
};

class QueryTaskDelete : public QueryPredefined
{
public:
    QueryTaskDelete(const Commons& commons,
                    const std::string &dsname,
                    const std::string &taskname)
        : QueryPredefined(commons)
    {
        _pquerybuilder->useQueryString(_pquerybuilder->getTaskDeleteQuery(dsname, taskname));
    }
};

class QueryLastInsertedId : public QueryPredefined
{
public:
    QueryLastInsertedId(const Commons& commons)
        : QueryPredefined(commons), _last_id(0)
    {
        _pquerybuilder->useQueryString(_pquerybuilder->getLastInsertedIdQuery());
    }

    bool execute() override
    {
        int retval = _connection.fetch(_pquerybuilder->getGenericQuery(),
                                       _pquerybuilder->getQueryParam(),
                                       *_presultset);
        if (retval > 0) {
            resultset().setPosition(0);
            _last_id = resultset().getInt(0);
            return true;
        }
        else {
            return false;
        }
    }

    int getLastId() const
    { return _last_id; }

private:
    int _last_id;
};


}
