#include "filterthread.h"

#include "logger.h"


FilterThread::FilterThread()
    : _partition(0), _offset(0), _filterFunc(nullptr), _stop(false)
{
}

FilterThread::~FilterThread()
{
    if (consumerPtr()) {
        consumerPtr()->unassign();
        consumerPtr().reset();
    }
    _topic.clear();
    _partition = 0;
    _offset = 0;
    _filterFunc = nullptr;
    _filterValue.clear();
}

void FilterThread::consumerPtr(KafkaConsumer::Ptr p)
{
    _consumerPtr = p;
}

KafkaConsumer::Ptr FilterThread::consumerPtr() const
{
    return _consumerPtr;

}

void FilterThread::topic(const std::string &topic)
{
    _topic = topic;
}

const std::string &FilterThread::topic() const
{
    return _topic;
}

void FilterThread::partition(int32_t p)
{
    _partition = p;
}

int32_t FilterThread::partition() const
{
    return _partition;
}

void FilterThread::offset(int64_t of)
{
    _offset = of;
}

int64_t FilterThread::offset() const
{
    return _offset;
}

void FilterThread::filterValue(const std::string &value)
{
    _filterValue = value;
}

const std::string &FilterThread::filterValue() const
{
    return _filterValue;
}

void FilterThread::stop()
{
    _stop.store(true);
}

void FilterThread::run()
{
    consumerPtr()->unassign();

    if (!consumerPtr()->assign( {RdKafka::TopicPartition::create(
                                    topic(), partition(), offset())} )) {
        ELOG << "failed assign kafka topic: " << QString::fromStdString(topic())
             << ", partition: " << partition()
             << ", offset: " << offset();
        return;
    }

    std::string topic;
    int32_t partition = 0;
    int64_t offset = 0;

    bool bFound = false;
    std::string data;

    while (isRunning() && !_stop) {
        if (!consumerPtr()->consume(data, topic, partition, offset)) {
            ELOG << "failed consume kafka topic: " << QString::fromStdString(topic)
                 << ", partition: " << partition
                 << ", offset: " << offset;
            bFound = false;
            break;
        }

        emit reportOffsetSignal(QString::number(offset));

        if (_filterFunc && _filterFunc(data, filterValue())) {
            bFound = true;
            break;
        }
    }

    emit filterResultSignal(QString::fromStdString(data));
}
