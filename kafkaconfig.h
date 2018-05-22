#ifndef KAFKACONFIG_H
#define KAFKACONFIG_H

#include <map>
#include <string>

#include <librdkafka/rdkafkacpp.h>

#include "kafkaeventcb.h"


namespace kafka_config {
// kafka连接地址
const char * const BROKER_LIST = "metadata.broker.list";
const char * const SECURITY_PROTOCOL = "security.protocol";
const char * const SASL_MECHANISMS = "sasl.mechanisms";
const char * const SASL_USERNAME = "sasl.username";
const char * const SASL_PASSWORD = "sasl.password";
const char * const EVENT_CB = "event_cb";
const char * const DR_CB = "dr_cb";
const char * const GROUP_ID = "group.id";
}

class KafkaConfig
{
public:
    using ConfigMap = std::map<std::string, std::string>;


    KafkaConfig();

    virtual RdKafka::Conf *config() = 0;

    const ConfigMap &configMap();

    // 配置属性
    KafkaConfig &operator << (const std::pair<std::string, std::string> &pa);
    KafkaConfig &operator << (KafkaEventCb *cb);

protected:
    KafkaEventCb *kafkaEventCbPtr();

private:
    ConfigMap _configMap;
    KafkaEventCb *_kafkaEventCbPtr;
};

#endif // KAFKACONFIG_H
