#include "GraphicalInterface.h"
#include "Kafka/Consumer.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <memory>

namespace {
void brokerTable(std::unique_ptr<RdKafka::Metadata> const &Metadata) {
  ImGui::TreeNode("Brokers");
  ImGui::Columns(3, "brokers_table");
  ImGui::Separator();
  // clang-format off
  ImGui::Text("Broker"); ImGui::NextColumn();
  ImGui::Text("Host"); ImGui::NextColumn();
  ImGui::Text("Port"); ImGui::NextColumn();
  // clang-format on
  ImGui::Separator();
  for (auto Broker : *Metadata->brokers()) {
    // clang-format off
    ImGui::Text("%d", Broker->id()); ImGui::NextColumn();
    ImGui::Text("%s", Broker->host().c_str()); ImGui::NextColumn();
    ImGui::Text("%d", Broker->port()); ImGui::NextColumn();
    // clang-format on
    ImGui::Separator();
  }
  ImGui::Columns(1);
  ImGui::Separator();
}

void topicsTable(std::unique_ptr<Kafka::Consumer> &KafkaConsumer,
                 std::unique_ptr<RdKafka::Metadata> const &Metadata) {
  ImGui::TreeNode("Topics");

  for (auto Topic : *Metadata->topics()) {
    ImGui::TreeNode(Topic->topic().c_str());
    ImGui::Columns(6);
    ImGui::Separator();
    // clang-format off
    ImGui::Text("Partition"); ImGui::NextColumn();
    ImGui::Text("Low Offset"); ImGui::NextColumn();
    ImGui::Text("High Offset"); ImGui::NextColumn();
    ImGui::Text("Leader"); ImGui::NextColumn();
    ImGui::Text("Replicas"); ImGui::NextColumn();
    ImGui::Text("ISRS"); ImGui::NextColumn();
    // clang-format on
    ImGui::Separator();
    for (auto Partition : *Topic->partitions()) {
      // TODO MUST CACHE OFFSETS, can't look them up on every redraw!!
      OffsetsStruct PartitionOffsets =
          KafkaConsumer->getPartitionHighAndLowOffsets(Topic->topic(),
                                                       Partition->id());
      std::stringstream Replicas;
      std::copy(Partition->replicas()->begin(), Partition->replicas()->end(),
                std::ostream_iterator<int32_t>(Replicas, ", "));
      std::stringstream ISRSs;
      std::copy(Partition->isrs()->begin(), Partition->isrs()->end(),
                std::ostream_iterator<int32_t>(ISRSs, ", "));
      // clang-format off
      ImGui::Text("%d", Partition->id()); ImGui::NextColumn();
      ImGui::Text("%ld", PartitionOffsets.LowOffset); ImGui::NextColumn();
      ImGui::Text("%ld", PartitionOffsets.HighOffset); ImGui::NextColumn();
      ImGui::Text("%d", Partition->leader()); ImGui::NextColumn();
      ImGui::Text("%s", Replicas.str().c_str()); ImGui::NextColumn();
      ImGui::Text("%s", ISRSs.str().c_str()); ImGui::NextColumn();
      // clang-format on
      ImGui::Separator();
    }

    ImGui::Columns(1);
    ImGui::Separator();
  }
}

void metadataWindow(std::unique_ptr<Kafka::Consumer> &KafkaConsumer,
                    std::unique_ptr<RdKafka::Metadata> &Metadata) {
  ImGui::Begin("Metadata");

  auto Refresh = ImGui::Button("Refresh");
  if (Refresh) {
    Metadata = KafkaConsumer->queryMetadata();
  }

  brokerTable(Metadata);
  topicsTable(KafkaConsumer, Metadata);

  ImGui::End();
}
} // namespace

void initGUI(std::string const &Broker) {
  auto KafkaConsumer = std::make_unique<Kafka::Consumer>(Broker);
  auto Metadata = KafkaConsumer->queryMetadata();

  sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "kafkacow");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);

  sf::Clock deltaClock;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    ImGui::SFML::Update(window, deltaClock.restart());

    bool ShowDemoWindow = true;
    ImGui::ShowDemoWindow(&ShowDemoWindow);

    metadataWindow(KafkaConsumer, Metadata);

    window.clear(sf::Color(100, 200, 255, 255));
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
}
