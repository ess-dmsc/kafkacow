add_executable(kafkaCow main.cpp
        RequestHandler.cpp
        RequestHandler.h
        RequestHandlerInterface.h
        ConnectKafka.cpp
        ConnectKafka.h
        ConnectKafkaInterface.h
        ConnectKafkaFake.cpp
        ConnectKafkaFake.h)
target_link_libraries(kafkaCow CONAN_PKG::cli11 CONAN_PKG::librdkafka)
