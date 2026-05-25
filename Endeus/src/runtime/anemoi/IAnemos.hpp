#pragma once

#include "../WorldModel.hpp"
#include "AnemosTypes.hpp"

namespace endeus {

	/// <summary>
	/// 风, 代指动画
	/// </summary>
	class IAnemos {
	public:
		// 虚析构方法, 防止通过基类指针删除对象时调用基类的虚构
		virtual ~IAnemos() = default;

		virtual bool update(float dt, WorldModel& world) = 0;

		virtual void skip(WorldModel& world) = 0;

		virtual AnemosKey getKey() const = 0;
	};

}
