add_executable(kafkacow main.cpp
        RequestHandler.cpp
        RequestHandler.h
        KafkaW/Consumer.cpp
        KafkaW/Consumer.h
        KafkaW/ConsumerInterface.h
        KafkaW/FakeConsumer.cpp
        KafkaW/FakeConsumer.h
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
        KafkaW/Producer.cpp
        KafkaW/Producer.h
        KafkaW/KafkaConfig.h
        KafkaW/ConsumerFactory.cpp
        KafkaW/ConsumerFactory.h
        KafkaW/ProducerInterface.h
        KafkaW/ProducerFactory.cpp
        KafkaW/ProducerFactory.h KafkaW/FakeProducer.cpp KafkaW/FakeProducer.h KafkaW/Message.h)

target_link_libraries(kafkacow CONAN_PKG::cli11
        CONAN_PKG::librdkafka
        CONAN_PKG::flatbuffers
        CONAN_PKG::boost_system
        CONAN_PKG::boost_filesystem
        CONAN_PKG::boost_dll
        CONAN_PKG::spdlog
        CONAN_PKG::jsonformoderncpp
        CONAN_PKG::libgit2
        CONAN_PKG::fmt
        CONAN_PKG::streaming-data-types)
