#include "GraphicalInterface.h"
#include "Kafka/Consumer.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <memory>

void metadataWindow(std::unique_ptr<Kafka::Consumer> &KafkaConsumer,
                    std::string &MetadataString) {
  ImGui::Begin("Metadata");

  auto Refresh = ImGui::Button("Refresh");
  if (Refresh) {
    KafkaConsumer->updateMetadata();
    MetadataString = KafkaConsumer->showAllMetadata();
  }
  ImGui::Text("%s", MetadataString.c_str());
  ImGui::End();
}

void initGUI(std::string const &Broker) {
  auto KafkaConsumer = std::make_unique<Kafka::Consumer>(Broker);
  auto MetadataString = KafkaConsumer->showAllMetadata();

  sf::RenderWindow window(sf::VideoMode(640, 480), "kafkacow");
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

    metadataWindow(KafkaConsumer, MetadataString);

    window.clear(sf::Color(0, 50, 150, 255));
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
}
