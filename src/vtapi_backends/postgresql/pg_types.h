#pragma once

#include <vtapi/common/exception.h>
#include <vtapi/data/intervalevent.h>
#include <vtapi/data/eyedea_edfdescriptor.h>
#include <vtapi/data/processstate.h>
#include <libpq-fe.h>
#include <libpqtypes.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <chrono>
#include <ctime>

namespace vtapi {


// /////////////////////////////////////////////////
// TYPE CONVERTERS

template<typename TDB, typename TOUT>
class TypeConverterDatabase
{
public:
    static TOUT convert(TDB &val)
    {
        return static_cast<TOUT>(val);
    }
};

template <>
class TypeConverterDatabase<PGnumeric,int>
{
public:
    static int convert(PGnumeric &val)
    {
        return std::atoi(val);
    }
};

template <>
class TypeConverterDatabase<PGnumeric,long long>
{
public:
    static long long convert(PGnumeric &val)
    {
        return std::atoll(val);
    }
};

template <>
class TypeConverterDatabase<PGnumeric,float>
{
public:
    static float convert(PGnumeric &val)
    {
        return static_cast<float>(std::atof(val));
    }
};

template <>
class TypeConverterDatabase<PGnumeric,double>
{
public:
    static double convert(PGnumeric &val)
    {
        return std::atof(val);
    }
};

template <>
class TypeConverterDatabase<PGtimestamp,std::chrono::system_clock::time_point>
{
public:
    static std::chrono::system_clock::time_point convert(PGtimestamp &val)
    {
        time_t t = static_cast<time_t>(val.epoch);
        return std::chrono::system_clock::from_time_t(t) + std::chrono::microseconds(val.time.usec);
    }
};

template <>
class TypeConverterDatabase<PGresult*,cv::Mat>
{
public:
    static cv::Mat convert(PGresult* &val)
    {
        cv::Mat mat;
        PGarray mat_dims_arr = { 0 };

        try
        {
            // get cvmat members
            PGint4 mat_type = 0;
            PGbytea mat_data_bytea = { 0 };
            if (!PQgetf(val, 0, "%int4 %int4[] %bytea",
                        0, &mat_type, 1, &mat_dims_arr, 2, &mat_data_bytea))
                throw RuntimeException("Cannot get cvmat header");

            // create dimensions array
            int mat_dims = PQntuples(mat_dims_arr.res);
            if (!mat_dims) throw RuntimeException("CvMat has no dimensions");

            std::vector<int> mat_dim_sizes(mat_dims);
            for (int i = 0; i < mat_dims; i++) {
                if (!PQgetf(mat_dims_arr.res, i, "%int4", 0, &mat_dim_sizes[i]))
                    throw RuntimeException("Failed to get value: unexpected value in CvMat dimensions");
            }

            // create matrix and copy data
            mat = cv::Mat(mat_dims, mat_dim_sizes.data(), mat_type);
            std::copy(mat_data_bytea.data, mat_data_bytea.data + mat_data_bytea.len, mat.data);

            PQclear(mat_dims_arr.res);
            PQclear(val);
        }
        catch (RuntimeException &)
        {
            if (mat_dims_arr.res)
                PQclear(mat_dims_arr.res);
            PQclear(val);
            throw;
        }

        return mat;
    }
};

template <>
class TypeConverterDatabase<PGresult*,IntervalEvent>
{
public:
    static IntervalEvent convert(PGresult * &val)
    {
        IntervalEvent event;

        try
        {
            // get event members
            PGint4 ev_group_id = 0;
            PGint4 ev_class_id = 0;
            PGbool ev_is_root = false;
            PGfloat8 ev_score = 0.0;
            PGbox ev_region = { 0 };
            PGbytea ev_data = { 0 };
            if (!PQgetf(val, 0, "%int4 %int4 %bool %box %float8 %bytea",
                        0, &ev_group_id, 1, &ev_class_id, 2, &ev_is_root,
                        3, &ev_region, 4, &ev_score, 5, &ev_data))
                throw RuntimeException("Failed to get value: unexpected value in event header");

            event.group_id = static_cast<int>(ev_group_id);
            event.class_id = static_cast<int>(ev_class_id);
            event.is_root = static_cast<bool>(ev_is_root);
            event.score = static_cast<double>(ev_score);
            event.region = IntervalEvent::Box(ev_region.high.x, ev_region.high.y,
                                              ev_region.low.x, ev_region.low.y);
            if (ev_data.len > 0)
                event.user_data = std::vector<char>(ev_data.data, ev_data.data + ev_data.len);

            PQclear(val);
        }
        catch(RuntimeException &)
        {
            PQclear(val);
            throw;
        }

        return event;
    }
};


template <>
class TypeConverterDatabase<PGresult *, EyedeaEdfDescriptor>
{
public:
    static EyedeaEdfDescriptor convert(PGresult * &val)
    {
        EyedeaEdfDescriptor edfdesc;

        try
        {
            // get event members
            PGint4 version = 0;
            PGbytea data = { 0 };
            if (!PQgetf(val, 0, "%int4 %bytea",
                        0, &version, 1, edfdesc.data))
                throw RuntimeException("Failed to get value: unexpected value in edfdescriptor header");

            edfdesc.version = static_cast<int>(version);

            if (data.len > 0) {
                edfdesc.data = std::vector<unsigned char>(data.data, data.data + data.len);
            }

            PQclear(val);
        }
        catch(RuntimeException &)
        {
            PQclear(val);
            throw;
        }

        return edfdesc;
    }
};


template <>
class TypeConverterDatabase<PGresult*,ProcessState>
{
public:
    static ProcessState convert(PGresult * &val)
    {
        ProcessState pstate;

        try
        {
            // get event members
            PGvarchar ps_status = NULL, ps_curritem = NULL, ps_lasterror = NULL;
            PGfloat8 ps_progress = 0;
            if (! PQgetf(val, 0, "%public.pstatus %float8 %varchar %varchar",
                         0, &ps_status, 1, &ps_progress,
                         2, &ps_curritem, 3, &ps_lasterror))
                throw RuntimeException("Failed to get value: unexpected value in pstate header");

            if (ps_status)
                pstate.status = pstate.toStatusValue(ps_status);
            if (ps_curritem)
                pstate.current_item = ps_curritem;
            if (ps_lasterror)
                pstate.last_error = ps_lasterror;
            pstate.progress = ps_progress;

            PQclear(val);
        }
        catch(RuntimeException &e)
        {
            PQclear(val);
            throw;
        }

        return pstate;
    }
};

template <>
class TypeConverterDatabase< PGbytea,std::vector<char> >
{
public:
    static std::vector<char> convert(PGbytea &val)
    {
        if (val.len > 0)
            return std::vector<char>(val.data, val.data + val.len);
        else
            return std::vector<char>();
    }
};



template <>
class TypeConverterDatabase<PGpoint,Point>
{
public:
    static Point convert(PGpoint &val)
    {
        return Point(val.x, val.y);
    }
};


template<typename TDB, typename TOUT>
class TypeConverterOutput
{
public:
    static TDB convert(const TOUT &val)
    {
        return static_cast<TDB>(val);
    }
};


// /////////////////////////////////////////////////
// GETTER


template<typename TDB, typename TOUT>
class GetterSingle
{
public:
    static TOUT get(const PGresult *res,
                    int pos, int col,
                    const char *def)
    {
        TDB value;

        if (!res)
            throw RuntimeException("Failed to get value:: result set uninitialized");
        else if (pos < 0)
            throw RuntimeException("Failed to get value: result set position is invalid");
        else if (col < 0)
            throw RuntimeException("Failed to get value: colum index is invalid");
        else if (!PQgetf(res, pos, def, col, &value))
            throw RuntimeException(std::string("Failed to get value: type mismatch for ") + def);

        return TypeConverterDatabase<TDB,TOUT>::convert(value);
    }
};

template<typename TDB>
class GetterSingle<TDB,std::string>
{
public:
    static std::string get(const PGresult *res,
                           int pos, int col,
                           const char *def)
    {
        if (!res)
            throw RuntimeException("Failed to get value:: result set uninitialized");
        else if (pos < 0)
            throw RuntimeException("Failed to get value: result set position is invalid");
        else if (col < 0)
            throw RuntimeException("Failed to get value: colum index is invalid");

        // no conversions with libpqtypes, just get the raw value
        char *value = PQgetvalue(res, pos, col);
        return value ? std::string(value) : std::string();
    }
};

template<typename TDB, typename TOUT>
class GetterVector
{
public:
    static std::vector<TOUT> get(const PGresult *res,
                                 int pos, int col,
                                 const char *def, const char *def_arr)
    {
        std::vector<TOUT> values;
        PGarray tmp = { 0 };

        try
        {
            if (!res)
                throw RuntimeException("Failed to get value:: result set uninitialized");
            else if (pos < 0)
                throw RuntimeException("Failed to get value: result set position is invalid");
            else if (col < 0)
                throw RuntimeException("Failed to get value: colum index is invalid");
            else if (!PQgetf(res, pos, def_arr, col, &tmp))
                throw RuntimeException(std::string("Failed to get value: type mismatch for ") + def);

            int size = PQntuples(tmp.res);
            values.resize(size);
            for (int i = 0; i < size; i++)
                values[i] = GetterSingle<TDB,TOUT>::get(tmp.res, i, 0, def);

            PQclear(tmp.res);
        }
        catch (RuntimeException &e)
        {
            if (tmp.res) PQclear(tmp.res);
            throw;
        }

        return values;
    }
};


}
