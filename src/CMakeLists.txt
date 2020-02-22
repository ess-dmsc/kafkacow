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
        GetSchemaPath.cpp
        GetSchemaPath.h
        Filesystem.h
        Kafka/Producer.cpp
        Kafka/Producer.h
        Kafka/KafkaConfig.h
        Kafka/ConsumerFactory.cpp
        Kafka/ConsumerFactory.h
        Kafka/ProducerInterface.h
        Kafka/ProducerFactory.cpp
        Kafka/ProducerFactory.h
        Kafka/FakeProducer.cpp
        Kafka/FakeProducer.h
        Kafka/Message.h)

target_link_libraries(kafkacow PRIVATE
        ${CONAN_PACKAGES}
        CONAN_PKG::fmt)

# Link stdc++fs to get std::experimental::filesystem when necessary
target_link_libraries(kafkacow PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)
