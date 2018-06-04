#include "kafkaconsumer.h"

#include "logger.h"


KafkaConsumer::KafkaConsumer(KafkaConfig &config)
    : _kafkaConfig(config) {
}

bool KafkaConsumer::init()
{
    std::string errstr;
    _pConsumer = RdKafka::KafkaConsumer::create(kafkaConfig().config(),
                                                errstr);
    if (!_pConsumer) {
        DLOG << "failed to create kafka consumer";
        return false;
    }

    return true;
}

bool KafkaConsumer::topics(TopicVec &topics) {
    RdKafka::Metadata *pMetadata;
    auto ret = consumerPtr()->metadata(true, nullptr, &pMetadata, 1000*10);
    if (ret!= RdKafka::ERR_NO_ERROR) {
        DLOG << "failed to get metadata: " << ret << " : "
             << RdKafka::err2str(ret).c_str();
        return false;
    }

    for (const auto &pTopicMetadata : *pMetadata->topics()) {
        TopicMetadata tmd;
        tmd.topic = pTopicMetadata->topic();
        const auto *partitions = pTopicMetadata->partitions();
        for (const auto &pPartitionMetadata : *partitions) {
            PartitionMetadata pmd;
            pmd.id = pPartitionMetadata->id();
            pmd.leader = pPartitionMetadata->leader();
            pmd.isrsVector = *pPartitionMetadata->isrs();
            pmd.replicasVector = *pPartitionMetadata->replicas();

            tmd.partitions.push_back(pmd);

            std::sort(tmd.partitions.begin(), tmd.partitions.end(),
                      [](const PartitionMetadata &l,
                      const PartitionMetadata &r){
                return l.id < r.id;
            });
        }

        topics.push_back(tmd);
    }

    // 排序
    std::sort(topics.begin(), topics.end(), [](const TopicMetadata &l,
              const TopicMetadata &r) {
        return l.topic < r.topic;
    } );

    return true;
}

bool KafkaConsumer::queryWatermarkOffsets(const std::string &topic, int partition, int64_t &low, int64_t &high) {
    auto ret = consumerPtr()->query_watermark_offsets(topic, partition,
                                                      &low, &high, 10000);
    return ret == RdKafka::ERR_NO_ERROR;
}

bool KafkaConsumer::messageAtOffset(const std::string &topic, int partition, int64_t offset, std::string &message, std::string &errstr) {

    std::vector<RdKafka::TopicPartition*> partitions;
    if (RdKafka::ERR_NO_ERROR != consumerPtr()->assignment(partitions)) {
        WLOG << "failed to exec assignment";
        return false;
    }

    for (auto *p : partitions) {
        if (p) {
            delete p;
        }
    }

    auto ret = consumerPtr()->unassign();
    if (ret != RdKafka::ERR_NO_ERROR) {
        DLOG << "failed unassign " << ret << ", errstr: "
             << RdKafka::err2str(ret).c_str();
    }

    auto *pPartition = RdKafka::TopicPartition::create(topic, partition, offset);
    ret = consumerPtr()->assign({pPartition});
    if (ret != RdKafka::ERR_NO_ERROR) {
        DLOG << "failed to assign " << ret << ", errstr: "
             << RdKafka::err2str(ret).c_str();
        errstr = RdKafka::err2str(ret);
        return false;
    }

    std::shared_ptr<RdKafka::Message> pMessage(consumerPtr()->consume(1000*30));
    if (pMessage->err() != RdKafka::ERR_NO_ERROR) {
        DLOG << "failed to assign " << pMessage->err() << ", errstr: "
             << pMessage->errstr().c_str();
        errstr = pMessage->errstr();
        return false;
    }
    message.assign(static_cast<char*>(pMessage->payload()),
                   pMessage->len());

    return true;
}

KafkaConfig &KafkaConsumer::kafkaConfig()
{
    return _kafkaConfig;
}

bool KafkaConsumer::assign( const std::initializer_list<RdKafka::TopicPartition*> &partitions ) {
    auto ret = consumerPtr()->assign( partitions );
    if ( ret != RdKafka::ERR_NO_ERROR ) {
        FLOG << "failed assign partions, ret " << ret;
        return false;
    }

    return true;
}

bool KafkaConsumer::assign( const std::vector<RdKafka::TopicPartition*> &partitions ) {
    auto ret = consumerPtr()->assign( partitions );
    if ( ret != RdKafka::ERR_NO_ERROR ) {
        FLOG << "failed assign partions, ret " << ret;
        return false;
    }

    return true;
}

bool KafkaConsumer::unassign() {
    std::vector<RdKafka::TopicPartition*> partitions;
    if (RdKafka::ERR_NO_ERROR != consumerPtr()->assignment(partitions)) {
        WLOG << "failed to exec assignment";
        return false;
    }

    for (auto *p : partitions) {
        if (p) {
            delete p;
        }
    }

    auto ret = consumerPtr()->unassign();
    if ( ret != RdKafka::ERR_NO_ERROR ) {
        FLOG << "failed to unassign, ret: " << ret;
        return false;
    }

    return true;
}

bool KafkaConsumer::consume( std::string &data, std::string &topic,
                             int32_t &partition, int64_t &offset, int timeoutMs) {
    std::shared_ptr<RdKafka::Message> pMessage(
                consumerPtr()->consume( timeoutMs ),
                []( RdKafka::Message * p ) {
        delete p;
        DLOG << "delete kafka message";
    } );

    if ( pMessage->err() != RdKafka::ERR_NO_ERROR ) {
        // 消费超时
        if ( pMessage->err() == RdKafka::ERR__TIMED_OUT ) {
            ILOG << "consumed  err: " << pMessage->err()
                 << ", errstr: " << QString::fromStdString(pMessage->errstr());
            return true;
        }
        // 读到队尾
        else if ( pMessage->err() == RdKafka::ERR__PARTITION_EOF ) {
            ILOG << "consumed  err: " << pMessage->err()
                 << ", errstr: " << QString::fromStdString(pMessage->errstr())
                 << ", offset: " << pMessage->offset();
            topic = pMessage->topic_name();
            partition = pMessage->partition();
            offset = pMessage->offset();
            return true;
        }

        FLOG << "failed to consume, " << pMessage->err()
             << ", errstr: " << QString::fromStdString(pMessage->errstr());

        return false;
    }

    data.assign( static_cast<char*>( pMessage->payload() ),
                 pMessage->len() );
    topic = pMessage->topic_name();
    partition = pMessage->partition();
    offset = pMessage->offset();

    return true;
}
