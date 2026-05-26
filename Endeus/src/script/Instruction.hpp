#pragma once

#include <string>
#include <utility>
#include <vector>
#include <variant>
#include <type_traits>

#include "../utils/Types.hpp"

namespace endeus {

	/**
	 * @brief 指令类，用于描述引擎中的一个原子操作
	 */
	class Instruction {
	public:
		// ========================= 画面类指令 =========================

		/// 显示图层
		struct ShowLayer {
			std::string layerId;
			std::string textureKey;
			Vec2f position;
			float alpha;
			int order;
			Rect texRect;			///< 显示区域, 空表示整张纹理

		};

		/// 隐藏图层
		struct HideLayer {
			std::string layerId;
		};

		/// 移动图层
		struct MoveLayer {
			std::string layerId;
			Vec2f toPosition;
			float durationSeconds = 0.f;
		};

		/// 淡变图层透明度
		struct FadeLayer {
			std::string layerId;
			float toAlpha;                ///< 0~255
			float durationSeconds;
		};

		// ========================= 文字类指令 =========================

		/// 设置说话人
		struct SetSpeaker {
			std::string name;
		};

		/// 设置内容
		struct SetContent {
			std::string content;
			bool append = false;
		};

		// ========================= 流程控制类指令 =========================

		/// 选项分支
		struct Choice {
			struct Option {
				std::string text;
				std::string targetLabel;
			};
			std::vector<Option> options;
		};

		/// 无条件跳转
		struct Jump {
			std::string targetLabel;
		};

		/// 结束游戏
		struct End {
			bool closeWindow = true;
		};

		struct Wait {
			float seconds = 0.f;		///< 0=等待
			bool skippable = true;		///< 是否允许跳过
		};

		/// 标签
		struct Label {
			std::string name;
		};

		// ========================= 公共接口 =========================

		/**
		 * @brief 从具体指令子类型构造对象。
		 * @tparam T 指令子类型
		 * @param subtype 具体指令实例
		 */
		template <typename T>
		Instruction(const T& subtype) : m_data(subtype) {}

		/**
		 * @brief 检查当前存储的指令是否为指定子类型。
		 * @tparam T 要检查的指令类型
		 * @return true 如果当前指令正是 T 类型，否则 false
		 */
		template <typename T>
		[[nodiscard]] bool is() const {
			static_assert(isInstructionSubtype<T>, "T is not a valid Instruction type");
			return std::holds_alternative<T>(m_data);
		}

		/**
		 * @brief 获取当前存储的指令指针（只读）。
		 * @tparam T 期望的指令类型
		 * @return 如果类型匹配，返回指向内部数据的指针，否则返回 nullptr
		 */
		template <typename T>
		[[nodiscard]] const T* getIf() const {
			static_assert(isInstructionSubtype<T>, "T is not a valid Instruction type");
			return std::get_if<T>(&m_data);
		}

		/**
		 * @brief 访问者模式：应用一个可调用对象到当前指令上。
		 * @tparam Visitor 一个可以处理所有指令子类型的函数对象（重载 operator() 或使用 lambda + if constexpr）
		 * @param visitor 访问器
		 * @return 访问器返回的值
		 */
		template <typename Visitor>
		decltype(auto) visit(Visitor&& visitor) const {					// 使用 decltype(auto) 可以完美转发 std::visit 的返回值类型
			static_assert(isInstructionHandler<Visitor>, "Visitor must be callable with all Instruction types");
			return std::visit(std::forward<Visitor>(visitor), m_data);	// 根据 Visitor 推导的类型恢复原始值类别, 完美转发, 避免额外拷贝
		}

	private:
		using Variant = std::variant<
			ShowLayer,
			HideLayer,
			MoveLayer,
			FadeLayer,
			SetSpeaker,
			SetContent,
			Choice,
			Jump,
			End,
			Wait,
			Label
		>;
		Variant m_data;

		// ========================= 辅助元函数（编译期类型检查） =========================

		/**
		 * @brief 检查类型 T 是否出现在 parameter pack Ts... 中。
		 * @tparam T 要查找的类型
		 * @tparam Ts 变体中的类型列表
		 * @param 指向 std::variant<Ts...> 的指针（仅用于推导 Ts...）
		 * @return constexpr bool
		 */
		template <typename T, typename... Ts>
		[[nodiscard]] static constexpr bool isInParameterPack(const std::variant<Ts...>*) {
			// 先对 Ts 中的每个类型分别实例化 std::is_same<T, 单个类型>, 产生一个布尔值的列表
			// 然后 std::disjunction_v 对这个列表进行逻辑"或"运算
			return std::disjunction_v<std::is_same<T, Ts>...>;
		}

		/// 判断 T 是否为合法的指令子类型
		template <typename T>
		// static_cast<Variant*>(nullptr) 构造一个指向具体 Variant 类型的空指针, 只利用其类型
		// 然后传给 isInParameterPack<T>, 让编译器自动推导 Ts... 为 Variant 内部的所有类型
		// 用编译期常量布尔值的模板来封装简化调用
		static constexpr bool isInstructionSubtype = isInParameterPack<T>(static_cast<Variant*>(nullptr));

		/**
		 * @brief 检查 Handler 是否可以接受参数包 Ts... 中的所有类型。
		 * @tparam Handler 可调用对象类型（如 lambda、函数对象）
		 * @tparam Ts variant 中可能存储的类型列表
		 * @param 指向 std::variant<Ts...> 的指针（仅用于推导 Ts...）
		 * @return constexpr bool
		 */
		template <typename Handler, typename... Ts>
		[[nodiscard]] static constexpr bool isInvocableWithInstruction(const std::variant<Ts...>*) {
			return std::conjunction_v<std::is_invocable<Handler&, Ts&>...>;
		}

		/// 编译期布尔值，判断 Handler 是否为合法的 Instruction 访问器
		template <typename Handler>
		static constexpr bool isInstructionHandler = isInvocableWithInstruction<Handler>(static_cast<Variant*>(nullptr));
	};

} // namespace endeus
