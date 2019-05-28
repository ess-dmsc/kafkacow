file(GLOB SRC_FILES
        ../src/*.cpp
        ../src/*.h)

list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp$")
list(FILTER SRC_FILES EXCLUDE REGEX ".*Consumer.cpp$")

add_executable(UnitTests
        ${SRC_FILES}
        RequestHandlerTest.cpp
        FlatbuffersTranslatorTest.cpp
        JSONPrintingTest.cpp
        UnitTests.cpp
        ../src/KafkaW/FakeConsumer.cpp
        ../src/KafkaW/FakeConsumer.h
        ../src/KafkaW/Consumer.h
        ../src/KafkaW/Consumer.cpp
        ../src/KafkaW/ConsumerFactory.h
        ../src/KafkaW/ConsumerFactory.cpp
        ../src/KafkaW/Producer.h
        ../src/KafkaW/Producer.cpp
        ../src/KafkaW/ProducerFactory.h
        ../src/KafkaW/ProducerFactory.cpp
        ../src/KafkaW/FakeProducer.cpp
        ../src/KafkaW/FakeProducer.h)

target_link_libraries(UnitTests
        ${CONAN_PACKAGES}
        CONAN_PKG::gtest)
