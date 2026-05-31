#include "Engine.hpp"
#include <iostream>

namespace endeus {

	Engine::Engine() : m_window(sf::VideoMode({ 1280, 720 }), "Endeus - The Janus Door within EnD.C")
		, m_executor(m_worldManager, m_anemoi)
		, m_director(m_executor, m_leyline)
		, m_renderer(m_leyline, m_window, m_anemoi)
		, m_album(m_director, m_worldManager){
		m_window.setFramerateLimit(60);   // 限制 60 FPS
		loadAssets();
		buildScripts();
		m_director.init(m_script);

		// 订阅选项
		m_leyline.subscribe<Event::ChoiceSelected>([this](const Event& e) {
			auto* choice = e.getIf<Event::ChoiceSelected>();
			if (!choice) {
				return;
			}

			const std::string& target = choice->targetLabel;
			if (m_album.hasLeaf(target)) {
				m_anemoi.resetAll();
				m_renderer.clear();
				m_album.recall(target);
			}
			// 没有记忆则不回滚
		});
	}

	void Engine::run() {
		sf::Clock clock;
		while (m_window.isOpen()) {
			processEvents();
			float dt = clock.restart().asSeconds();		// 重置起始点为当前时刻，并返回重置前经过的时间

			auto result = m_director.update(dt);
			switch (result.action) {
			case DirectorAction::Memorize:
				std::cout << "Memorize" << std::endl;
				m_album.memorize(result.targetLabel.value());
				break;
			case DirectorAction::Recall:
				if (m_album.hasLeaf(result.targetLabel.value())) {
					m_anemoi.resetAll();
					m_renderer.clear();
					m_album.recall(result.targetLabel.value());
				}
				else {	// 没有这一页, 可能是未来的事情

				}
				break;
			case DirectorAction::Waiting:
				break;
			case DirectorAction::Terminated:
				break;
			}

			m_anemoi.update(dt);
			m_window.clear(sf::Color::Black);			// 黑色填充后备缓冲区
			m_renderer.draw(m_worldManager.getWorld(), m_worldManager.takeOptions());	// 绘制到后备缓冲区
			m_window.display();							// 交换前后缓冲区 ( SFML 采用双缓冲技术)
			if (m_director.isFinished()) {
				m_window.close();
			}
		}
	}

	void Engine::processEvents() {
		while (auto event = m_window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				m_window.close();
			}
			else if (auto* pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (pressed->button == sf::Mouse::Button::Left) {
					sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
					m_leyline.publish(Event::MouseClicked{ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});
				}
			}
		}
	}

	void Engine::loadAssets() {
		sf::Font font;
		if (!font.openFromFile("assets/simkai.ttf")) {
			throw std::runtime_error("Failed to open font");
		}
		m_renderer.setFont(font);

		sf::Texture texBg;
		if (!texBg.loadFromFile("assets/bg_janus.png")) {
			throw std::runtime_error("Missing bg_janus.png");
		}
		m_renderer.registerTexture("bg_janus", std::move(texBg));

		sf::Texture texDoor;
		if (!texDoor.loadFromFile("assets/door_sprite.png")) {
			throw std::runtime_error("Missing door_sprite.png");
		}
		m_renderer.registerTexture("door_sprite", std::move(texDoor));

		sf::Texture texReader;
		if (!texReader.loadFromFile("assets/ev_reader.png")) {
			throw std::runtime_error("Missing ev_reader.png");
		}
		m_renderer.registerTexture("ev_reader", std::move(texReader));

		sf::Texture texWriter;
		if (!texWriter.loadFromFile("assets/ev_writer.png")) {
			throw std::runtime_error("Missing ev_writer.png");
		}
		m_renderer.registerTexture("ev_writer", std::move(texWriter));
	}

	void Engine::buildScripts() {
		using Instr = Instruction;
		m_script = {
			// 初始画面
			Instr(Instr::ShowLayer{"bg_janus", "bg_janus", {0,0}, 0.f, 0, {}}),
			Instr(Instr::FadeLayer{"bg_janus", 1.0f, 1.f}),
			Instr(Instr::SetSpeaker{"Endeus"}),
			Instr(Instr::SetContent{"......", false}),
			Instr(Instr::Wait()),		// 等待点击
			Instr(Instr::SetContent{"............", false}),
			Instr(Instr::Wait()),
			Instr(Instr::Label{"restart"}),
			Instr(Instr::ShowLayer{"door", "door_sprite", {600,200}, 0.f, 1, {}}),
			Instr(Instr::FadeLayer{"door", 0.8f, 1.f}),
			Instr(Instr::SetContent{"写者借我构建故事。", false}),
			Instr(Instr::Wait()),
			Instr(Instr::MoveLayer{"door", {200, 200}, 3.f}),
			Instr(Instr::SetContent{"读者借我踏入世界。", true}),
			Instr(Instr::Wait()),
			//Instr(Instr::MoveLayer{"door", {400, 400}, 0.f}),
			Instr(Instr::SetContent{"他们从未见面，却完成同一次传递。", false}),
			Instr(Instr::Wait()),
			Instr(Instr::SetContent{"你希望以何种身份经过这扇门？", false}),
			Instr(Instr::Wait()),
			Instr(Instr::FadeLayer{"bg_janus", 0.0f, 1.f}),
			//Instr(Instr::HideLayer {"bg_janus"}),
			// 选项
			Instr(Instr::Choice{{{"写者", "writer"}, {"读者", "reader"}, {"我再想想", "restart"}}}),
			// 分支
			Instr(Instr::Label{"writer"}),
			Instr(Instr::FadeLayer{"door", 0.f, 1.f}),
			Instr(Instr::ShowLayer{"ev_writer", "ev_writer", {0,0}, 0.f, 0, {}}),
			Instr(Instr::FadeLayer{"ev_writer", 1.0f, 1.f}),
			Instr(Instr::SetContent{"你选择了写者。故事因你而生。", false}),
			Instr(Instr::Wait()),
			Instr(Instr::End{true}),
			Instr(Instr::Label{"reader"}),
			Instr(Instr::FadeLayer{"door", 0.f, 1.f}),
			Instr(Instr::ShowLayer{"ev_reader", "ev_reader", {0,0}, 0.f, 0, {}}),
			Instr(Instr::FadeLayer{"ev_reader", 1.0f, 1.f}),
			Instr(Instr::SetContent{"你选择了读者。世界因你而在。", false}),
			Instr(Instr::Wait()),
			Instr(Instr::End{true}),
		};
	}

} // namespace endeus
