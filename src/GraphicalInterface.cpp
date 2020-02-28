#include "GraphicalInterface.h"
#include "Kafka/Consumer.h"
#include "Metadata.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <memory>

namespace {
void brokerTable(Metadata::Cluster const &Metadata) {
  ImGui::SetNextItemOpen(true);
  if (ImGui::CollapsingHeader("Brokers")) {
    ImGui::Columns(3, "brokers_table");
    ImGui::Separator();
    // clang-format off
    ImGui::Text("Broker"); ImGui::NextColumn();
    ImGui::Text("Host"); ImGui::NextColumn();
    ImGui::Text("Port"); ImGui::NextColumn();
    // clang-format on
    ImGui::Separator();
    for (auto const &Broker : Metadata.Brokers) {
      // clang-format off
      ImGui::Text("%d", Broker.ID); ImGui::NextColumn();
      ImGui::Text("%s", Broker.Host.c_str()); ImGui::NextColumn();
      ImGui::Text("%d", Broker.Port); ImGui::NextColumn();
      // clang-format on
      ImGui::Separator();
    }
    ImGui::Columns(1);
    ImGui::Separator();
  }
}

void topicsTable(Metadata::Cluster const &Metadata,
                 std::map<std::string, bool> &TopicViewsEnabled) {
  ImGui::SetNextItemOpen(true);
  if (ImGui::CollapsingHeader("Topics")) {

    static ImGuiTextFilter filter;
    filter.Draw();

    for (auto const &Topic : Metadata.Topics) {
      if (filter.PassFilter(Topic.Name.c_str())) {
        ImGui::Text("%s", Topic.Name.c_str());
        ImGui::SameLine();
        auto ToggleViewData = ImGui::Button("Toggle view data");
        if (ToggleViewData) {
          TopicViewsEnabled[Topic.Name] = !TopicViewsEnabled[Topic.Name];
        }
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
        for (auto const &Partition : Topic.Partitions) {
          // clang-format off
          ImGui::Text("%d", Partition.ID); ImGui::NextColumn();
          ImGui::Text("%ld", Partition.LowOffset); ImGui::NextColumn();
          ImGui::Text("%ld", Partition.HighOffset); ImGui::NextColumn();
          ImGui::NextColumn(); ImGui::NextColumn(); ImGui::NextColumn();
  //      ImGui::Text("%d", Partition->leader()); ImGui::NextColumn();
  //      ImGui::Text("%s", Replicas.str().c_str()); ImGui::NextColumn();
  //      ImGui::Text("%s", ISRSs.str().c_str()); ImGui::NextColumn();
          // clang-format on
          ImGui::Separator();
        }

        ImGui::Columns(1);
        ImGui::Separator();
      }
    }
  }
}

void metadataWindow(Kafka::Consumer const &KafkaConsumer,
                    std::unique_ptr<RdKafka::Metadata> &KafkaMetadata,
                    Metadata::Cluster &ClusterMetadata,
                    std::map<std::string, bool> &TopicViewsEnabled) {
  ImVec2 window_pos = ImVec2(10, 10);
  ImGui::SetNextWindowPos(window_pos);

  ImGui::Begin("Metadata");

  auto Refresh = ImGui::Button("Refresh");
  if (Refresh) {
    KafkaMetadata = KafkaConsumer.queryMetadata();
    ClusterMetadata = {KafkaConsumer, KafkaMetadata, TopicViewsEnabled};
  }

  brokerTable(ClusterMetadata);
  topicsTable(ClusterMetadata, TopicViewsEnabled);

  ImGui::End();
}

void viewDataWindows(std::map<std::string, bool> &TopicViewsEnabled) {
  for (auto const &TopicAndViewEnabled : TopicViewsEnabled) {
    if (TopicAndViewEnabled.second) {
      std::cout << TopicAndViewEnabled.first << "\n";
    }
  }
}
} // namespace

void initGUI(std::string const &Broker) {
  std::map<std::string, bool> TopicViewsEnabled;
  Kafka::Consumer KafkaConsumer{Broker};
  auto KafkaMetadata = KafkaConsumer.queryMetadata();
  Metadata::Cluster ClusterMetadata{KafkaConsumer, KafkaMetadata,
                                    TopicViewsEnabled};

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

    metadataWindow(KafkaConsumer, KafkaMetadata, ClusterMetadata,
                   TopicViewsEnabled);
    viewDataWindows(TopicViewsEnabled);

    window.clear(sf::Color(100, 200, 255, 255));
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
}
