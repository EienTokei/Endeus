#pragma once

#include <memory>
namespace endeus {

	template<typename T>
	class ResourceHandle {
		std::shared_ptr<T> m_ptr;

	public:
		ResourceHandle() = default;
		explicit ResourceHandle(std::shared_ptr<T> ptr) : m_ptr(std::move(ptr)) {}

		T* get() const {
			return m_ptr.get();
		}

		T* operator->() const {
			return m_ptr.get();
		}

		explicit operator bool() const {
			return m_ptr != nullptr;
		}
	};

} // namespace endeus