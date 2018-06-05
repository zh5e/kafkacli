#include "filterthread.h"

#include "logger.h"
#include "filterfuncloader.h"
#include "parserfuncloader.h"


FilterThread::FilterThread()
    : _partition(0), _offset(0), _stop(false)
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

void FilterThread::filterFunc(const FilterFunc &func)
{
    _filterFunc = func;
}

const FilterFunc &FilterThread::filterFunc() const
{
    return _filterFunc;
}

void FilterThread::parserFunc(const ParserFunc &func)
{
    _parserFunc = func;
}

const ParserFunc &FilterThread::parserFunc() const
{
    return _parserFunc;
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

    FilterFuncLoader funcLoader(_filterFunc);
    auto filterFunc = funcLoader.resove();

    while (isRunning() && !_stop) {
        data.clear();
        if (!consumerPtr()->consume(data, topic, partition, offset)) {
            ELOG << "failed consume kafka topic: " << QString::fromStdString(topic)
                 << ", partition: " << partition
                 << ", offset: " << offset;
            bFound = false;
            break;
        }

        // 超时等
        if (data.empty()) {
            continue;
        }

        emit reportOffsetSignal(QString::number(offset));

        if (filterFunc && filterFunc(data, filterValue())) {
            bFound = true;
            break;
        }
    }

    // 主动停止不输出结果
    if (!_stop) {
        ParserFuncLoader funcLoader(_parserFunc);
        auto parserFunc = funcLoader.resolve();
        if (parserFunc) {
            auto message = parserFunc(data);
            emit filterResultSignal(QString::fromStdString(message));
        } else {
            emit filterResultSignal(QString::fromStdString(data));
        }
    }
}
