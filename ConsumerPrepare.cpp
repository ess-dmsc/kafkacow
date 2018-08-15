//
// Created by michal on 15/08/18.
//

#include "ConsumerPrepare.h"

ConsumerPrepare::ConsumerPrepare(std::string Broker, std::string ErrStr) {
    this->Consumer=std::shared_ptr<RdKafka::KafkaConsumer>(RdKafka::KafkaConsumer::create(createGlobalConfiguration(Broker).get(), ErrStr));

}

std::shared_ptr<RdKafka::KafkaConsumer> ConsumerPrepare::GetConsumer() {return this->Consumer;}
