#pragma once

#include "FlatbuffersTranslator.h"
#include "RequestHandlerInterface.h"

class RequestHandler : public RequestHandlerInterface {
private:
  std::unique_ptr<ConnectKafkaInterface> KafkaConnection;
  void consumePartitions(std::pair<std::string, bool> MessageAndEOF,
                         int &EOFPartitionCounter,
                         FlatbuffersTranslator &FlatBuffers);

public:
  explicit RequestHandler(
      std::unique_ptr<ConnectKafkaInterface> KafkaConnection)
      : KafkaConnection(std::move(KafkaConnection)) {}

  void checkAndRun(UserArgumentStruct UserArguments) override;

  void checkConsumerModeArguments(UserArgumentStruct UserArguments) override;

  void checkMetadataModeArguments(UserArgumentStruct UserArguments) override;

  void showTopicPartitionOffsets(UserArgumentStruct UserArguments) override;

  void subscribeConsumeAtOffset(std::string TopicName, int64_t Offset) override;
  void subscribeConsumeNLastMessages(std::string TopicName,
                                     int64_t NumberOfMessages) override;
};
