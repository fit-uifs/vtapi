#pragma once

#include <libpq-fe.h>
#include <libpqtypes.h>
#include <vtapi/plugins/vtapi_backend_resultset.h>

namespace vtapi {


class PGResultSet : public ResultSet
{
public:
    explicit PGResultSet(DBTYPES_MAP *dbtypes);
    ~PGResultSet();

    void newResult(void *res);

    int countRows();
    int countCols();
    bool isOk();
    void clear();

    TKey getKey(int col);
    TKeys* getKeys();
    std::string getKeyType(const int col);
    int getKeyIndex(const std::string& key);

    std::string getValue(const int col);

    char getChar(const int pos);
    std::string getString(const int col);
    bool getBool(const int col);
    int getInt(const int col);
    long long getInt8(const int col);
    int* getIntA(const int col, int& size);
    std::vector<int>* getIntV(const int col);
    long long* getInt8A(const int col, int& size);
    std::vector<long long>* getInt8V(const int col);
    float getFloat(const int col);
    double getFloat8(const int col);
    float* getFloatA(const int col, int& size);
    std::vector<float>* getFloatV(const int col);
    double* getFloat8A(const int col, int& size);
    std::vector<double>* getFloat8V(const int col);
    time_t getTimestamp(const int col);
    IntervalEvent *getIntervalEvent(const int col);
    ProcessState *getProcessState(const int col);
    Point getPoint(const int col);
    Point* getPointA(const int col, int& size);
    std::vector<Point>*  getPointV(const int col);
    cv::Mat *getCvMat(const int col);
    void *getBlob(const int col, int &size);

private:
    short getKeyTypeLength(const int col, const short def);

    template<typename TDB, typename TOUT>
    TOUT getSingleValue(const int col, const char *def);
    template<typename TDB, typename TOUT>
    TOUT *getArray(const int col, int &size, const char *def);
    template<typename TDB, typename TOUT>
    std::vector<TOUT> *getVector(const int col, const char *def);
};

}
