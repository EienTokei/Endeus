#pragma once

#include "IAnemos.hpp"

namespace endeus {

	class FadeLayerAnemos : public IAnemos {
	public:
		FadeLayerAnemos(std::string layerId, float to, float duration)
			: m_layerId(std::move(layerId)), m_to(to), m_duration(duration), m_elapsed(0.f) {}

		bool update(float dt, WorldModel& world) override {
			if (!m_initialized) {
				if (auto* layer = world.getLayer(m_layerId)) {
					m_from = layer->alpha;
					m_initialized = true;
				}
				else {
					return true; // 图层不存在，动画直接结束
				}
			}
			m_elapsed += dt;
			float t = interp::normalize(m_elapsed, m_duration);
			world.setLayerAlpha(m_layerId, interp::lerp<float>(m_from, m_to, t));
			return m_elapsed >= m_duration;
		}

		void skip(WorldModel& world) override {
			world.setLayerAlpha(m_layerId, m_to);
			m_elapsed = m_duration;
		}

		void reset(WorldModel& world) override {
			if (!m_initialized) {
				return;
			}
			world.setLayerAlpha(m_layerId, m_from);
			m_elapsed = 0;
		}

		AnemosKey getKey() const override {
			return { m_layerId, AnemosKey::Property::Alpha };
		}

	private:
		bool m_initialized = false;
		std::string m_layerId;
		float m_from, m_to;
		float m_duration, m_elapsed;
	};
}
