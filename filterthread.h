#ifndef FILTERTHREAD_H
#define FILTERTHREAD_H

#include <atomic>
#include <string>
#include <functional>
#include <memory>

#include <QThread>

#include "kafkaconsumer.h"
#include "libfuncmgr.h"


class FilterThread : public QThread
{
    Q_OBJECT

public:
    using Ptr = std::shared_ptr<FilterThread>;

    FilterThread();
    ~FilterThread();

    void consumerPtr(KafkaConsumer::Ptr p);
    KafkaConsumer::Ptr consumerPtr() const;

    void topic(const std::string &topic);
    const std::string &topic() const;

    void partition(int32_t p);
    int32_t partition() const;

    void offset(int64_t of);
    int64_t offset() const;

    void filterValue(const std::string &value);
    const std::string &filterValue() const;

    void stop();

private:
    void run() override;

signals:
    void filterResultSignal(const QString &);

    void reportOffsetSignal(const QString&);

private:
    KafkaConsumer::Ptr _consumerPtr;
    std::string _topic;
    int32_t _partition;
    int64_t _offset;
    FilterFunc::Func _filterFunc;
    std::string _filterValue;
    std::atomic_bool _stop;
};

#endif // FILTERTHREAD_H
