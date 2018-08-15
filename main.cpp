#include "ConnectKafka.h"
#include "ConsumerPrepare.h"
#include "RequestHandler.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

int main(int argc, char **argv) {
  CLI::App App{"From Kafka with love"};

  std::string Name;
  std::string Broker;
  std::string InstrumentName = "test";
  std::int16_t GoBack = -1;
  bool ShowAllTopics;

  App.add_option("-g, --go", GoBack, "How many records back to show");
  App.add_option("-t, --topic", Name, "Show records of specified topic");
  App.add_option("-b,--Broker", Broker, "Hostname or IP of Kafka broker");
  App.add_flag("-a, --all", ShowAllTopics, "Show a list of topics");
  App.set_config("-c,--config_file", "", "Read configuration from an ini file",
                 false);
  CLI11_PARSE(App, argc, argv);
  std::string ErrStr;

  ConsumerPrepare CP(Broker, ErrStr);
  auto Consumer = CP.GetConsumer();

  RequestHandler RH(Consumer);

  RH.PrintAllTopics();
  return 0;
}