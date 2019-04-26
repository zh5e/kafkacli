#include "filterthread.h"

#include "logger.h"
#include "filterfuncloader.h"
#include "parserfuncloader.h"


FilterThread::FilterThread()
    : _partition( 0 ), _offset( 0 ), _stop( false )
{
}

FilterThread::~FilterThread()
{
    if ( consumerPtr() ) {
        consumerPtr()->unassign();
        consumerPtr().reset();
    }

    _topic.clear();
    _partition = 0;
    _offset = 0;
    _filterValue.clear();
}

void FilterThread::consumerPtr( KafkaConsumer::Ptr p )
{
    _consumerPtr = p;
}

KafkaConsumer::Ptr FilterThread::consumerPtr() const
{
    return _consumerPtr;

}

void FilterThread::topic( const std::string &topic )
{
    _topic = topic;
}

const std::string &FilterThread::topic() const
{
    return _topic;
}

void FilterThread::partition( int32_t p )
{
    _partition = p;
}

int32_t FilterThread::partition() const
{
    return _partition;
}

void FilterThread::offset( int64_t of )
{
    _offset = of;
}

int64_t FilterThread::offset() const
{
    return _offset;
}

void FilterThread::filterValue( const std::string &value )
{
    _filterValue = value;
}

const std::string &FilterThread::filterValue() const
{
    return _filterValue;
}

void FilterThread::filterFunc( const FilterFunc &func )
{
    _filterFunc = func;
}

const FilterFunc &FilterThread::filterFunc() const
{
    return _filterFunc;
}

void FilterThread::parserFunc( const ParserFunc &func )
{
    _parserFunc = func;
}

const ParserFunc &FilterThread::parserFunc() const
{
    return _parserFunc;
}

void FilterThread::stop()
{
    _stop.store( true );
}

void FilterThread::run()
{
    consumerPtr()->unassign();

    if ( !consumerPtr()->assign( {RdKafka::TopicPartition::create(
                                      topic(), partition(), offset() )} ) ) {
        ELOG << "failed assign kafka topic: " << QString::fromStdString( topic() )
             << ", partition: " << partition()
             << ", offset: " << offset();
        return;
    }

    FilterFuncLoader funcLoader( _filterFunc );
    auto filterFunc = funcLoader.resove();

    if ( !filterFunc ) {
        emit filterResultSignal( QString( "找不到查找方法" ) );
        return;
    }

    while ( isRunning() ) {
        std::shared_ptr<RdKafka::Message> msgptr;

        if ( !consumerPtr()->consume( msgptr ) ) {
            ELOG << "failed consume kafka topic";
            break;
        }

        // 超时等
        if ( msgptr->err() == RdKafka::ERR__TIMED_OUT && !_stop ) {
            continue;
        }

        DLOG << "offset: " << msgptr->offset();
        emit reportOffsetSignal( QString::number( msgptr->offset() ) );

        std::string data( static_cast< char* >( msgptr->payload() ),
                          msgptr->len() );

        if ( filterFunc && filterFunc( data, filterValue() ) ) {
            DLOG << "filter value: " << QString::fromStdString( filterValue() );
            ParserFuncLoader funcLoader( _parserFunc );
            auto parserFunc = funcLoader.resolve();

            if ( parserFunc ) {
                auto message = parserFunc( data );
                emit filterResultSignal( QString::fromStdString( message ) );
            } else {
                emit filterResultSignal( QString::fromStdString( data ) );
            }

            break;
        }

        // 消费结束
        if ( msgptr->err() == RdKafka::ERR__PARTITION_EOF ) {
            DLOG << "eof";
            emit filterResultSignal( QString( "未找到消息" ) );
            break;
        }

        // 主动停止不输出结果
        if ( _stop ) {
            DLOG << "stop";
            emit filterResultSignal( QString() );
            break;
        }
    }
}
