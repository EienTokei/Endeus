#pragma once

#include "IAnemos.hpp"

namespace endeus {

	class FadeLayerAnemos : public IAnemos {
	public:
		FadeLayerAnemos(std::string layerId, float from, float to, float duration)
			: m_layerId(std::move(layerId)), m_from(from), m_to(to), m_current(from), m_duration(duration), m_elapsed(0.f) {}

		bool update(float dt) override {
			m_elapsed += dt;
			float t = interp::normalize(m_elapsed, m_duration);
			m_current = interp::lerp<float>(m_from, m_to, t);
			return m_elapsed >= m_duration;
		}

		void skip() override {
			m_current = m_to;
			m_elapsed = m_duration;
		}

		void reset() override {
			m_current = m_from;
			m_elapsed = 0;
		}

		AnemosKey getKey() const override {
			return { m_layerId, AnemosKey::Property::Alpha };
		}

		float getCurrent() const {
			return m_current;
		}

	private:
		std::string m_layerId;
		float m_from, m_to, m_current;
		float m_duration, m_elapsed;
	};

} // namespace endeus
