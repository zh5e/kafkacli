#include "kafkaconfig.h"

KafkaConfig::KafkaConfig()
{

}

const KafkaConfig::ConfigMap &KafkaConfig::configMap()
{
    return _configMap;
}

KafkaConfig &KafkaConfig::operator <<(const std::pair<std::string, std::string> &pa)
{
    _configMap[pa.first] = pa.second;
    return *this;
}

KafkaConfig &KafkaConfig::operator <<(KafkaEventCb *cb)
{
    _kafkaEventCbPtr = cb;
    return *this;
}

KafkaEventCb *KafkaConfig::kafkaEventCbPtr()
{
    return _kafkaEventCbPtr;
}
