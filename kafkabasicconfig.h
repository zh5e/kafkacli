#ifndef KAFKABASICCONFIG_H
#define KAFKABASICCONFIG_H

#include "kafkaconfig.h"


class KafkaBasicConfig : public KafkaConfig
{
public:
    KafkaBasicConfig();

    virtual RdKafka::Conf *config();

private:
};

#endif // KAFKACONSUMERCONFIG_H
