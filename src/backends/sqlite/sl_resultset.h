#pragma once

#include <sqlite3.h>
#include <vtapi/plugins/backend_resultset.h>

namespace vtapi {

class SLResultSet : public ResultSet
{
public:

    explicit SLResultSet();
    ~SLResultSet();

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
};

typedef struct {
    std::string database;
} SLparam;

typedef struct {
    int rows;
    int cols;
    char **res;
} SLres;


}
