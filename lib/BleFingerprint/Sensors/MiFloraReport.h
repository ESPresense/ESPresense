#include "../QueryReport.h"

class MiFloraReport : public QueryReport {
   public:
    virtual void GetTopic(std::string* input, std::string roomsTopic) {
        *input = roomsTopic + std::string("/miflora/") + GetAddress()->toString();
    };

    explicit MiFloraReport(NimBLEAddress* address) : QueryReport(address){};
};
