#include "kafkabasicconfig.h"

#include <string>

#include "logger.h"


KafkaBasicConfig::KafkaBasicConfig()
{

}

RdKafka::Conf *KafkaBasicConfig::config()
{
    auto *pConf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (!pConf) {
        return nullptr;
    }

    DLOG << "kafka config:";

    std::string errstr;
    for (const auto &pa : configMap()) {
        if (RdKafka::Conf::CONF_OK != pConf->set(pa.first, pa.second, errstr)){
            WLOG << "faield to set config, key: "
                 << QString::fromStdString(pa.first)
                 << ", second: "
                 << QString::fromStdString(pa.second)
                 << ", errstr: "  << QString::fromStdString(errstr);
            return nullptr;
        }
        DLOG << QString::fromStdString(pa.first) << " : "
             << QString::fromStdString(pa.second);
    }

    if (kafkaEventCbPtr()) {
        if (RdKafka::Conf::CONF_OK != pConf->set("event_cb", kafkaEventCbPtr(),
                                                 errstr)) {
            DLOG << "failed to set event_cb, errstr: "
                 << QString::fromStdString(errstr);
            return nullptr;
        }
    }

    return pConf;
}
