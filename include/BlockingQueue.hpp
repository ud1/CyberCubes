#ifndef BLOCKING_QUEUE_HPP
#define BLOCKING_QUEUE_HPP

#include <condition_variable>
#include <deque>

namespace cyberCubes {
	
template <typename T>
class BlockingQueue
{
public:
	void push(T const& value)
	{
		{
			std::unique_lock<std::mutex> lock(this->_mutex);
			_queue.push_front(value);
		}
		this->_condition.notify_one();
	}
	
	void push(T && value)
	{
		{
			std::unique_lock<std::mutex> lock(this->_mutex);
			_queue.push_front(std::move(value));
		}
		this->_condition.notify_one();
	}
    
	T pop()
	{
		std::unique_lock<std::mutex> lock(this->_mutex);
		this->_condition.wait(lock, [=]{ return !this->_queue.empty(); });
		T rc(std::move(this->_queue.back()));
		this->_queue.pop_back();
		return rc;
	}
    
	bool tryPop (T & v, std::chrono::milliseconds dur)
	{
		std::unique_lock<std::mutex> lock(this->_mutex);
		if (!this->_condition.wait_for(lock, dur, [=]{ return !this->_queue.empty(); }))
		{
			return false;
		}
		v = std::move (this->_queue.back());
		this->_queue.pop_back();
		return true;
	}
	
	bool peek(T & v)
	{
		std::unique_lock<std::mutex> lock(this->_mutex);
		if (!_queue.empty())
		{
			v = std::move (this->_queue.back());
			this->_queue.pop_back();
			return true;	
		}
		
		return false;
	}
    
	size_t size()
	{
		std::unique_lock<std::mutex> lock(this->_mutex);
		return _queue.size();
	}
	
private:
	std::mutex              _mutex;
	std::condition_variable _condition;
	std::deque<T>           _queue;
};

}

#endif // BLOCKING_QUEUE_HPP

