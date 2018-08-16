add_executable(kafkaCow main.cpp
        RequestHandler.cpp
        RequestHandler.h
        ConnectKafka.cpp
        ConnectKafka.h)
target_link_libraries(kafkaCow CONAN_PKG::cli11 CONAN_PKG::librdkafka)
