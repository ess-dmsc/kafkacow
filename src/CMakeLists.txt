add_executable(kafkaCow main.cpp
        RequestHandler.cpp
        RequestHandler.h
        ../test/RequestHandlerParentClass.h
        ConnectKafka.cpp
        ConnectKafka.h
        ../test/RequestHandlerFakeTestClass.cpp
        ../test/RequestHandlerFakeTestClass.h
        ../test/ConnectKafkaParentClass.h
        ../test/ConnectKafkaFakeTestClass.cpp
        ../test/ConnectKafkaFakeTestClass.h)
target_link_libraries(kafkaCow CONAN_PKG::cli11 CONAN_PKG::librdkafka)
