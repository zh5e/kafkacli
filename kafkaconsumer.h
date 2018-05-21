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

#include "logger.h"


namespace  {

class KafkaEventCb : public RdKafka::EventCb {
public:
    using Cb = std::function<void(const std::string & str)>;
    KafkaEventCb(Cb failureCb)
        : _failureCb(failureCb) {
    }

    virtual void event_cb(RdKafka::Event &event) {
        if (event.err() != RdKafka::ERR_NO_ERROR) {
            auto errstr =  RdKafka::err2str(event.err());
            if (_failureCb) {
                _failureCb(errstr);
            }
        }
    }

private:
    Cb _failureCb;
};

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
}

class KafkaConsumer {
public:
    using Ptr = std::shared_ptr<KafkaConsumer>;
    using TopicVec = std::vector<TopicMetadata>;
    using FailureCb = std::function<void(const std::string & errstr)>;

    KafkaConsumer(const std::string &broker, FailureCb failureCb)
        : _failureCb(failureCb) {
        std::map<std::string, std::string> configPair = {
            std::make_pair("metadata.broker.list", broker),
            std::make_pair("group.id", "isendkafkacli")
        };

        std::string errstr;
        auto *pConf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

        for (const auto &conf : configPair) {
            DLOG << QString::fromStdString(conf.first)
                 << QString(": ") << QString::fromStdString(conf.second);

            if (RdKafka::Conf::CONF_OK !=  pConf->set(conf.first, conf.second, errstr)) {
                DLOG << QString::asprintf("failed to set %s, %s, errstr: %s",
                                          conf.first.c_str(), conf.second.c_str(),
                                          errstr.c_str());
                return;
            }
        }

        auto ret = pConf->set("event_cb",
                              new KafkaEventCb([&](const std::string & str){
            if (_failureCb) {
                _failureCb(str);
            }
        }), errstr);
        if (ret != RdKafka::Conf::CONF_OK) {
            DLOG << "failed to set event_cb, errstr: "
                 << QString::fromStdString(errstr);
            return;
        }

        auto *pConsumer = RdKafka::KafkaConsumer::create(pConf, errstr);
        if (!pConsumer) {
            DLOG << "failed to create kafka consumer";
            return;
        }

        _pConsumer = std::shared_ptr<RdKafka::KafkaConsumer>(pConsumer);
    }

    /*
     * 获取所有topics
     */
    TopicVec topics() {
        RdKafka::Metadata *pMetadata;
        auto ret = consumerPtr()->metadata(true, nullptr, &pMetadata, 1000);
        if (ret!= RdKafka::ERR_NO_ERROR) {
            DLOG << "failed to get metadata: " << ret << " : "
                 << RdKafka::err2str(ret).c_str();
            return TopicVec();
        }

        TopicVec topics;
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

        return topics;
    }

    bool queryWatermarkOffsets(const std::string &topic, int partition,
                               int64_t &low, int64_t &high) {
        auto ret = consumerPtr()->query_watermark_offsets(topic, partition,
                                                          &low, &high, 10000);
        return ret == RdKafka::ERR_NO_ERROR;
    }

    bool messageAtOffset(const std::string &topic, int partition, int64_t offset,
                         std::string &message, std::string &errstr) {
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

        std::shared_ptr<RdKafka::Message> pMessage(consumerPtr()->consume(1000*5));
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

private:
     std::shared_ptr<RdKafka::KafkaConsumer> consumerPtr() {
        return _pConsumer;
    }

private:
    std::shared_ptr<RdKafka::KafkaConsumer> _pConsumer;
    FailureCb _failureCb;
};

#endif // KAFKACONSUMER_H
