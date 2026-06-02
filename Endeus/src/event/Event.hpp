#pragma once

#include <string>
#include <variant>
#include "../utils/Types.hpp"

namespace endeus {
	class Event {
	public:
		/// <summary>
		///  鼠标点击
		/// </summary>
		struct MouseClicked {
			Vec2f position;
		};

		/// <summary>
		/// 表示操作已完成的空结构体
		/// </summary>
		struct ActionCompleted {};

		/// <summary>
		/// 选中选项
		/// </summary>
		struct ChoiceSelected {
			std::string targetLabel;
			int optionIndex = -1;
		};

		template <typename T>
		Event(const T& subtype) : m_data(subtype) {}

		template <typename T>
		[[nodiscard]] bool is() const {
			static_assert(isEventSubtype<T>, "T is not a valid Event type");
			return std::holds_alternative<T>(m_data);
		}

		template <typename T>
		[[nodiscard]] const T* getIf() const {
			static_assert(isEventSubtype<T>, "T is not a valid Event type");
			return std::get_if<T>(&m_data);
		}

		template <typename Visitor>
		decltype(auto) visit(Visitor&& visitor) const {
			static_assert(isEventHandler<Visitor>, "Visitor must be callable with all Event types");
			return std::visit(std::forward<Visitor>(visitor), m_data);
		}

		// 获取当前事件的类型索引
		size_t index() const { return m_data.index(); }

		// 获取事件类型总数（编译期常量）
		static constexpr size_t typeCount() {
			return std::variant_size_v<Variant>;
		}

		// 获取特定事件类型的编译期索引
		template<typename T>
		static constexpr size_t typeIndex() {
			static_assert(isEventSubtype<T>, "Unsupported event type");
			return Variant(std::in_place_type<T>).index();
		}

	private:
		using Variant = std::variant<
			MouseClicked,
			ActionCompleted,
			ChoiceSelected
		>;
		Variant m_data;

		template <typename T, typename... Ts>
		[[nodiscard]] static constexpr bool isInParameterPack(const std::variant<Ts...>*) {
			return std::disjunction_v<std::is_same<T, Ts>...>;
		}

		template <typename T>
		static constexpr bool isEventSubtype = isInParameterPack<T>(static_cast<Variant*>(nullptr));

		template <typename Handler, typename... Ts>
		[[nodiscard]] static constexpr bool isInvocableWithEvent(const std::variant<Ts...>*) {
			return std::conjunction_v<std::is_invocable<Handler&, Ts&>...>;
		}

		template <typename Handler>
		static constexpr bool isEventHandler = isInvocableWithEvent<Handler>(static_cast<Variant*>(nullptr));

	};

}
