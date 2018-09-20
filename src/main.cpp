#include "ArgumentsException.h"
#include "ConnectKafka.h"
#include "RequestHandler.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
  CLI::App App{"From Kafka with love"};

  UserArgumentStruct UserArguments;
  std::string Broker;

  App.add_option(
      "-g, --go", UserArguments.GoBack,
      "How many records back to show from partition \"-p\". Mutually "
      "exclusive with \"--Offset\"");
  App.add_option("-t, --topic", UserArguments.Name,
                 "Show records of specified topic");
  App.add_option("-b,--broker", Broker, "Hostname or IP of Kafka broker");
  App.add_option("-o,--offset", UserArguments.OffsetToStart,
                 "Start consuming from an offset. Otherwise retrieve entire "
                 "topic. Mutually exclusive with \"--go\"");
  App.add_option("-p,--partition", UserArguments.PartitionToConsume,
                 "Partition to retrieve messages from");

  App.add_flag("-a, --all", UserArguments.ShowAllTopics,
               "Show a list of topics");
  App.add_flag("-C, --consumer", UserArguments.ConsumerMode,
               "Run the program in the consumer mode");
  App.add_flag("-L, --list", UserArguments.MetadataMode,
               "Metadata mode. Show all topics and partitions. If \"-t\" "
               "specified, shows partition offsets.");
  App.add_flag("-E, --entire", UserArguments.ShowEntireMessage,
               "Show all records of a message(truncated by default)");

  App.set_config("-c,--config_file", "", "Read configuration from an ini file",
                 false);

  CLI11_PARSE(App, argc, argv);
  std::string ErrStr;
  auto KafkaConnection = std::make_unique<ConnectKafka>(Broker, ErrStr);

  // setup logger
  auto Logger = spdlog::stderr_color_mt("LOG");
  Logger->info("Welcome to spdlog!");

  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);
  try {
    NewRequestHandler.checkAndRun();
  } catch (ArgumentsException &E) {
    E.what();
  } catch (std::exception &E) {
    std::cout << E.what() << std::endl;
  }
  return 0;
}
