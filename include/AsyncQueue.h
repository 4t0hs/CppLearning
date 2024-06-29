#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

template<typename T>
class AsyncQueue {
	std::mutex _mutex;
	std::condition_variable _condition;
	std::deque<T> _queue{};

	bool _TimedPop(T *buffer, const std::chrono::milliseconds milliSec) {
		std::unique_lock<std::mutex> lock(_mutex);
		if (milliSec == std::chrono::milliseconds::zero()) {
			_condition.wait(lock, [&] {
				return !this->_queue.empty();
			});
		} else {
			bool ret = _condition.wait_for(lock, milliSec, [&] {
				return !this->_queue.empty();
			});
			if (!ret) {
				return false;
			}
		}
		*buffer = std::move(_queue.front());
		_queue.pop_front();
		return true;
	}
public:
	AsyncQueue() = default;

	void Push(const T &item) {
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.push_back(item);
		_condition.notify_all();
	}

	void Push(const T &&item) {
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.emplace_back(std::move(item));
		_condition.notify_all();
	}

	void Emplace(const T &item) {
		_queue.emplace_back(item);
	}

	bool IsEmpty() {
		std::lock_guard<std::mutex> lock(_mutex);
		return _queue.empty();
	}

	T Pop() {
		T buffer;
		TimedPop(&buffer, std::chrono::milliseconds::zero());
		return std::move(buffer);
	}

	bool TimedPop(T *buffer, const std::chrono::milliseconds milliSec) {
		if (!buffer) {
			return false;
		}
		return _TimedPop(buffer, milliSec);
	}
};
