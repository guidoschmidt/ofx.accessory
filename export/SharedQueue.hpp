#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class SharedQueue {
private:
  std::queue<T>           _q;
  std::mutex              _mutex;
  std::condition_variable _condition_var;
  
public:
  T& front();
  void pop();
  void emplace(T& item);

  int size();
  bool empty();
};

template <typename T>
T& SharedQueue<T>::front() {
  std::unique_lock<std::mutex> lock(_mutex);
  while (_q.empty()) {
    _condition_var.wait(lock);
  }
  return _q.front();
}

template <typename T>
void SharedQueue<T>::pop() {
  std::unique_lock<std::mutex> lock(_mutex);
  while (_q.empty()) {
    _condition_var.wait(lock);
  }
  _q.pop();
}

template <typename T>
void SharedQueue<T>::emplace(T& item) {
  std::unique_lock<std::mutex> lock(_mutex);
  _q.emplace(item);
  lock.unlock();
  _condition_var.notify_one();
}

template <typename T>
int SharedQueue<T>::size() {
  std::unique_lock<std::mutex> lock(_mutex);
  int size = _q.size();
  lock.unlock();
  return size;
}

template <typename T>
bool SharedQueue<T>::empty() {
  std::unique_lock<std::mutex> lock(_mutex);
  bool is_empty = _q.empty();
  lock.unlock();
  return is_empty;
}
