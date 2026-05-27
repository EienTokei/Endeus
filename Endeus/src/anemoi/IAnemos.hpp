#pragma once

#include "AnemosTypes.hpp"
#include "../utils/Interpolate.hpp"	// 插值函数头文件, 传递依赖, 避免在每个子类包含

namespace endeus {

	/// <summary>
	/// 风, 代指动画
	/// </summary>
	class IAnemos {
	public:
		// 虚析构方法, 防止通过基类指针删除对象时调用基类的虚构
		virtual ~IAnemos() = default;

		virtual bool update(float dt) = 0;

		virtual void skip() = 0;

		virtual void reset() = 0;

		virtual AnemosKey getKey() const = 0;
	};

}
