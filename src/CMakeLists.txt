add_executable(kafkacow main.cpp
        RequestHandler.cpp
        RequestHandler.h
Kafka/Consumer.cpp
Kafka/Consumer.h
Kafka/ConsumerInterface.h
Kafka/FakeConsumer.cpp
Kafka/FakeConsumer.h
        CustomExceptions.h
        UserArgumentsStruct.h
        OffsetsStruct.h
        TopicMetadataStruct.h
        FlatbuffersTranslator.cpp
        FlatbuffersTranslator.h
        JSONPrinting.cpp
        JSONPrinting.h
        UpdateSchemas.cpp
        UpdateSchemas.h
Kafka/Producer.cpp
Kafka/Producer.h
Kafka/KafkaConfig.h
Kafka/ConsumerFactory.cpp
Kafka/ConsumerFactory.h
Kafka/ProducerInterface.h
Kafka/ProducerFactory.cpp
Kafka/ProducerFactory.h Kafka/FakeProducer.cpp Kafka/FakeProducer.h Kafka/Message.h)

target_link_libraries(kafkacow
        ${CONAN_PACKAGES}
        CONAN_PKG::fmt)
