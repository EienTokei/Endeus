#include "Engine.hpp"

namespace endeus {

	Engine::Engine() : m_window(sf::VideoMode({ 1280, 720 }), "Endeus - The Janus Door within EnD.C")
		, m_runtime(m_eventBus, m_window), m_director(m_runtime, m_eventBus) {
		m_window.setFramerateLimit(60);   // 限制 60 FPS
		loadAssets();
		buildScripts();
		m_director.init(m_script);
	}

	void Engine::run() {
		sf::Clock clock;
		while (m_window.isOpen()) {
			processEvents();
			float dt = clock.restart().asSeconds();		// 重置起始点为当前时刻，并返回重置前经过的时间
			m_director.update(dt);
			m_window.clear(sf::Color::Black);			// 黑色填充后备缓冲区
			m_runtime.draw(m_window);					// 绘制到后备缓冲区
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
					m_eventBus.publish(Event::MouseClicked{ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});
				}
			}
		}
	}

	void Engine::loadAssets() {
		sf::Font font;
		if (!font.openFromFile("assets/simkai.ttf")) {
			throw std::runtime_error("Failed to open font");
		}
		m_runtime.setFont(font);

		sf::Texture texBg;
		if (!texBg.loadFromFile("assets/bg_janus.png")) {
			throw std::runtime_error("Missing bg_janus.png");
		}
		m_runtime.registerTexture("bg_janus", std::move(texBg));

		sf::Texture texDoor;
		if (!texDoor.loadFromFile("assets/door_sprite.png")) {
			throw std::runtime_error("Missing door_sprite.png");
		}
		m_runtime.registerTexture("door_sprite", std::move(texDoor));

		sf::Texture texReader;
		if (!texReader.loadFromFile("assets/ev_reader.png")) {
			throw std::runtime_error("Missing ev_reader.png");
		}
		m_runtime.registerTexture("ev_reader", std::move(texReader));

		sf::Texture texWriter;
		if (!texWriter.loadFromFile("assets/ev_writer.png")) {
			throw std::runtime_error("Missing ev_writer.png");
		}
		m_runtime.registerTexture("ev_writer", std::move(texWriter));
	}

	void Engine::buildScripts() {
		using Instr = Instruction;
		m_script = {
			// 初始画面
			Instr(Instr::ShowLayer{"bg_janus", "bg_janus", {0,0}, 1.f, 0, {}}),
			Instr(Instr::SetSpeaker{"Endeus"}),
			Instr(Instr::SetContent{"......", false}),
			Instr(Instr::Wait{0.f, true}),     // 等待点击
			Instr(Instr::SetContent{"............", false}),
			Instr(Instr::Wait{0.f, true}),
			Instr(Instr::Label{"restart"}),
			Instr(Instr::ShowLayer{"door", "door_sprite", {600,200}, 0.5f, 1, {}}),
			Instr(Instr::SetContent{"写者借我构建故事。", false}),
			Instr(Instr::Wait{0.f, true}),
			Instr(Instr::MoveLayer{"door", {400, 200}, 3.f}),
			Instr(Instr::SetContent{"读者借我踏入世界。", true}),
			Instr(Instr::Wait{0.f, true}),
			Instr(Instr::MoveLayer{"door", {400, 400}, 0.f}),
			Instr(Instr::SetContent{"他们从未见面，却完成同一次传递。", false}),
			Instr(Instr::Wait{0.f, true}),
			Instr(Instr::SetContent{"你希望以何种身份经过这扇门？", false}),
			Instr(Instr::Wait{0.f, true}),
			// 选项
			Instr(Instr::Choice{{{"写者", "writer"}, {"读者", "reader"}, {"我再想想", "restart"}}}),
			// 分支
			Instr(Instr::Label{"writer"}),
			Instr(Instr::HideLayer {"bg"}),
			Instr(Instr::ShowLayer{"ev_writer", "ev_writer", {0,0}, 1.f, 0, {}}),
			Instr(Instr::SetContent{"你选择了写者。故事因你而生。", false}),
			Instr(Instr::Wait{0.f, true}),
			Instr(Instr::End{true}),
			Instr(Instr::Label{"reader"}),
			Instr(Instr::HideLayer {"bg"}),
			Instr(Instr::ShowLayer{"ev_reader", "ev_reader", {0,0}, 1.f, 0, {}}),
			Instr(Instr::SetContent{"你选择了读者。世界因你而在。", false}),
			Instr(Instr::Wait{0.f, true}),
			Instr(Instr::End{true}),
		};
	}

} // namespace endeus
