#ifndef KAFKACONSUMER_H
#define KAFKACONSUMER_H

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>

#include <librdkafka/rdkafkacpp.h>
#include <QMessageBox>

#include "kafkaconfig.h"


struct PartitionMetadata {
    int32_t id;
    int32_t leader;
    std::vector<int32_t> replicasVector;
    std::vector<int32_t> isrsVector;
};

struct TopicMetadata {
    std::string topic;
    std::vector<PartitionMetadata> partitions;
};


class KafkaConsumer
{
public:
    using Ptr = std::shared_ptr<KafkaConsumer>;
    using TopicVec = std::vector<TopicMetadata>;

    KafkaConsumer( KafkaConfig &config );

    bool init();

    // 获取所有topics
    bool topics( TopicVec &topics );

    bool queryWatermarkOffsets( const std::string &topic, int partition,
                                int64_t &low, int64_t &high );

    // 查看当前位置kafka信息
    bool messageAtOffset( const std::string &topic, int partition, int64_t offset,
                          std::string &message, std::string &errstr );
    bool assign( const std::initializer_list<RdKafka::TopicPartition *> &partitions );
    bool assign( const std::vector<RdKafka::TopicPartition *> &partitions );
    bool unassign();
    bool consume( std::string &data, std::string &topic, int32_t &partition, int64_t &offset,
                  int timeoutMs = 3000 );
    bool consume( std::shared_ptr<RdKafka::Message> &msgptr );

protected:
    RdKafka::KafkaConsumer *consumerPtr() {
        return _pConsumer;
    }

    KafkaConfig &kafkaConfig();

private:
    KafkaConfig &_kafkaConfig;
    RdKafka::KafkaConsumer *_pConsumer;
};

#endif // KAFKACONSUMER_H
