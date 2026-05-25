#pragma once

#include "IAnemos.hpp"
#include "../WorldModel.hpp"

namespace endeus {

	class MoveLayerAnemos : public IAnemos {
	public:
		MoveLayerAnemos(std::string layerId, Vec2f to, float duration) 
			: m_layerId(std::move(layerId)), m_to(to), m_duration(duration), m_elapsed(0.f) {}

		bool update(float dt, WorldModel& world) override {
			if (!m_initialized) {
				if (auto* layer = world.getLayer(m_layerId)) {
					m_from = layer->position;
					m_initialized = true;
				}
				else {
					return true; // 图层不存在，动画直接结束
				}
			}
			m_elapsed += dt;
			float t = std::min(1.0f, m_elapsed / m_duration);	// 0~1
			Vec2f newPos = m_from + (m_to - m_from) * t;
			world.setLayerPosition(m_layerId, newPos);
			return m_elapsed >= m_duration;
		}

		void skip(WorldModel& world) override {
			world.setLayerPosition(m_layerId, m_to);
			m_elapsed = m_duration;
		}

		AnemosKey getKey() const override {
			return { m_layerId, AnemosKey::Property::Position };
		}

	private:
		bool m_initialized = false;
		std::string m_layerId;
		Vec2f m_from, m_to;
		float m_duration, m_elapsed;
	};
}
