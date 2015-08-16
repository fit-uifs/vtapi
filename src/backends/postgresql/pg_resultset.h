#pragma once

#include <libpq-fe.h>
#include <libpqtypes.h>
#include <vtapi/plugins/backend_resultset.h>

namespace vtapi {


class PGResultSet : public ResultSet
{
public:
    explicit PGResultSet(DBTYPES_MAP *dbtypes);
    ~PGResultSet();

    void newResult(void *res) override;

    int countRows() override;
    int countCols() override;
    bool isOk() override;
    void clear() override;

    TKey getKey(int col) override;
    TKeys* getKeys() override;
    std::string getKeyType(const int col) override;
    int getKeyIndex(const std::string& key) override;

    std::string getValue(const int col) override;

    char getChar(const int pos) override;
    std::string getString(const int col) override;
    bool getBool(const int col) override;
    int getInt(const int col) override;
    long long getInt8(const int col) override;
    int* getIntA(const int col, int& size) override;
    std::vector<int>* getIntV(const int col) override;
    long long* getInt8A(const int col, int& size) override;
    std::vector<long long>* getInt8V(const int col) override;
    float getFloat(const int col) override;
    double getFloat8(const int col) override;
    float* getFloatA(const int col, int& size) override;
    std::vector<float>* getFloatV(const int col) override;
    double* getFloat8A(const int col, int& size) override;
    std::vector<double>* getFloat8V(const int col) override;
    time_t getTimestamp(const int col) override;
    IntervalEvent *getIntervalEvent(const int col) override;
    ProcessState *getProcessState(const int col) override;
    Point getPoint(const int col) override;
    Point* getPointA(const int col, int& size) override;
    std::vector<Point>*  getPointV(const int col) override;
    cv::Mat *getCvMat(const int col) override;
    void *getBlob(const int col, int &size) override;

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
