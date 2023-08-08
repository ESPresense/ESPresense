#include <MqttQuery/QueryReport.h>
class MiFloraReport : public  QueryReport {

    virtual void GetTopic(std::string* input, std::string roomsTopic) {

        if (topic != nullptr) {

            *input = roomsTopic + std::string("/miflora/") +  getAddress()->toString();
            topic = input;

        }
    }

   private:

   public:
    MiFloraReport(NimBLEAddress* address) : QueryReport(address){};
};
