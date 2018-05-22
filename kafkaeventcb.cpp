#include "kafkaeventcb.h"

KafkaEventCb::KafkaEventCb(KafkaEventCb::Cb failureCb)
    : _failureCb(failureCb) {
}

void KafkaEventCb::event_cb(RdKafka::Event &event) {
    if (event.err() != RdKafka::ERR_NO_ERROR) {
        auto errstr =  RdKafka::err2str(event.err());
        if (_failureCb) {
            _failureCb(errstr);
        }
    }
}
