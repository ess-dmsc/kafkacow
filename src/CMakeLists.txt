add_executable(kafkacow main.cpp
        RequestHandler.cpp
        RequestHandler.h
        RequestHandlerInterface.h
        ConnectKafka.cpp
        ConnectKafka.h
        ConnectKafkaInterface.h
        ConnectKafkaFake.cpp
        ConnectKafkaFake.h
        ArgumentsException.h
        UserArgumentsStruct.h
        OffsetsStruct.h
        TopicMetadataStruct.h
        FlatbuffersTranslator.cpp
        FlatbuffersTranslator.h
        JSONPrinting.cpp
        JSONPrinting.h)
target_link_libraries(kafkacow CONAN_PKG::cli11
        CONAN_PKG::librdkafka
        CONAN_PKG::FlatBuffers
        CONAN_PKG::boost_filesystem
        CONAN_PKG::spdlog
        CONAN_PKG::yaml-cpp
        CONAN_PKG::jsonformoderncpp
        CONAN_PKG::fmt)
