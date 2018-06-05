#ifndef FILTERTHREAD_H
#define FILTERTHREAD_H

#include <atomic>
#include <string>
#include <functional>
#include <memory>

#include <QThread>

#include "kafkaconsumer.h"
#include "filterfunc.h"
#include "parserfunc.h"


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

    void filterFunc(const FilterFunc &func);
    const FilterFunc &filterFunc() const;

    void parserFunc(const ParserFunc &func);
    const ParserFunc &parserFunc() const;

    void stop();

private:
    void run() override;

signals:
    void filterResultSignal(const QString &);

    void reportOffsetSignal(const QString &);

private:
    KafkaConsumer::Ptr _consumerPtr;
    std::string _topic;
    int32_t _partition;
    int64_t _offset;
    FilterFunc _filterFunc;
    ParserFunc _parserFunc;
    std::string _filterValue;
    std::atomic_bool _stop;
};

#endif // FILTERTHREAD_H
