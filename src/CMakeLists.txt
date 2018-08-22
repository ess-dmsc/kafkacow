add_executable(kafkaCow main.cpp
        RequestHandler.cpp
        RequestHandler.h
        RequestHandlerParentClass.h
        ConnectKafka.cpp
        ConnectKafka.h
        ConnectKafkaParentClass.h
        ConnectKafkaFakeTestClass.cpp
        ConnectKafkaFakeTestClass.h)
target_link_libraries(kafkaCow CONAN_PKG::cli11 CONAN_PKG::librdkafka)
