#include "Engine.hpp"

namespace endeus {

	Engine::Engine() :window(sf::VideoMode({ 1280, 720 }), "Endeus - The Janus Door within EnD.C") {
		window.setFramerateLimit(60);   // 限制 60 FPS
		loadAssets();
		runScripts();
	}

	void Engine::run() {
		while (window.isOpen()) {
			handleInput();
			render();
		}
	}

	void Engine::handleInput() {
		while (auto optEvent = window.pollEvent()) {
			const auto& event = *optEvent;

			if (event.is<sf::Event::Closed>()) {
				window.close();
			}

			else if (auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
				if (pressed->button == sf::Mouse::Button::Left) {
					if (!waitingForChoice) {
						currentStep++;
						switch (currentStep) {
						case 1:
							dialogueText->setString(toSF(u8"写者借我构建故事。"));
							break;
						case 2:
							dialogueText->setString(toSF(u8"读者借我踏入世界。"));
							break;
						case 3:
							dialogueText->setString(toSF(u8"他们从未见面，却完成同一次传递。"));
							break;
						case 4:
						{
							waitingForChoice = true;
							dialogueText->setString(toSF(u8"你希望以何种身份经过这扇门？"));
							choiceTexts.clear();

							sf::Text opt1(font, toSF(u8"写者"), 28);
							opt1.setFillColor(sf::Color::Yellow);
							opt1.setPosition({ 500, 300 });

							sf::Text opt2(font, toSF(u8"读者"), 28);
							opt2.setFillColor(sf::Color::Cyan);
							opt2.setPosition({ 500, 400 });

							choiceTexts.push_back(std::move(opt1));
							choiceTexts.push_back(std::move(opt2));
							break;
						}
						default:
							window.close();
							break;
						}
					}
					else {
						auto mousePos = sf::Mouse::getPosition(window);
						for (const auto& opt : choiceTexts) {
							if (opt.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
								if (opt.getString() == toSF(u8"写者")) {
									dialogueText->setString(toSF(u8"你选择了写者。故事因你而生。\n点击任意处结束。"));
								}
								else {
									dialogueText->setString(toSF(u8"你选择了读者。世界因你而在。\n点击任意处结束。"));
								}
								waitingForChoice = false;
								choiceTexts.clear();
								currentStep = 999;
								break;
							}
						}
					}
				}
			}
		}
	}

	void Engine::render() {
		window.clear(sf::Color::Black);
		if (background) window.draw(*background);
		if (character) window.draw(*character);
		if (nameText) window.draw(*nameText);
		if (dialogueText) window.draw(*dialogueText);
		for (const auto& opt : choiceTexts) {
			window.draw(opt);
		}
		window.display();
	}

	void Engine::loadAssets() {
		if (!font.openFromFile("assets/simkai.ttf")) {
			throw std::runtime_error("Failed to open simkai.ttf");
		}

		sf::Texture texBg;
		if (!texBg.loadFromFile("assets/bg_janus.png")) {
			throw std::runtime_error("Missing bg_janus.png");
		}
		textures.emplace("bg_janus", std::move(texBg));

		sf::Texture texDoor;
		if (!texDoor.loadFromFile("assets/door_sprite.png")) {
			throw std::runtime_error("Missing door_sprite.png");
		}
		textures.emplace("door_sprite", std::move(texDoor));

		background = std::make_unique<sf::Sprite>(textures["bg_janus"]);
		character = std::make_unique<sf::Sprite>(textures["door_sprite"]);
		character->setPosition({ 800, 200 });

		nameText = std::make_unique<sf::Text>(font, "", 30);
		dialogueText = std::make_unique<sf::Text>(font, "", 24);
	}

	void Engine::runScripts() {

		nameText->setFont(font);
		nameText->setString("Endeus");
		nameText->setCharacterSize(30);
		nameText->setFillColor(sf::Color::Black);
		nameText->setPosition({ 50, 550 });

		dialogueText->setFont(font);
		dialogueText->setCharacterSize(24);
		dialogueText->setFillColor(sf::Color(35, 35, 35));
		dialogueText->setPosition({ 100, 600 });

		currentStep = 0;
		waitingForChoice = false;
		dialogueText->setString(toSF(u8"我是这里的\"雅努斯之门\"。写者与读者各自经过，而我不会介入他们。"));
	}

} // namespace endeus
