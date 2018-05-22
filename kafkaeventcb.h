#ifndef KAFKAEVENTCB_H
#define KAFKAEVENTCB_H

#include <functional>
#include <librdkafka/rdkafkacpp.h>


class KafkaEventCb : public RdKafka::EventCb {
public:
    using Cb = std::function<void(const std::string & str)>;

    KafkaEventCb(Cb failureCb);

    virtual void event_cb(RdKafka::Event &event);

private:
    Cb _failureCb;
};

#endif // KAFKAEVENTCB_H
