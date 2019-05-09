add_executable(kafkacow main.cpp
        RequestHandler.cpp
        RequestHandler.h
        ConnectKafka.cpp
        ConnectKafka.h
        ConnectKafkaInterface.h
        ConnectKafkaFake.cpp
        ConnectKafkaFake.h
        CustomExceptions.h
        UserArgumentsStruct.h
        OffsetsStruct.h
        TopicMetadataStruct.h
        FlatbuffersTranslator.cpp
        FlatbuffersTranslator.h
        JSONPrinting.cpp
        JSONPrinting.h
        UpdateSchemas.cpp
        UpdateSchemas.h)

target_link_libraries(kafkacow CONAN_PKG::cli11
        CONAN_PKG::librdkafka
        CONAN_PKG::flatbuffers
        CONAN_PKG::boost_system
        CONAN_PKG::boost_filesystem
        CONAN_PKG::boost_dll
        CONAN_PKG::spdlog
        CONAN_PKG::jsonformoderncpp
        CONAN_PKG::libgit2
        CONAN_PKG::fmt)
