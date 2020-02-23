#include "GraphicalInterface.h"
#include "Kafka/Consumer.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <memory>

void initGUI(std::string const &Broker)
{
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

        ImGui::Begin("Metadata");

        bool ShowDemoWindow = true;
        ImGui::ShowDemoWindow(&ShowDemoWindow);

        auto Refresh = ImGui::Button("Refresh");
        if (Refresh) {
            KafkaConsumer->updateMetadata();
            MetadataString = KafkaConsumer->showAllMetadata();
        }
        ImGui::Text("%s", MetadataString.c_str());
        ImGui::End();

        window.clear(sf::Color(0, 50, 150, 255));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
