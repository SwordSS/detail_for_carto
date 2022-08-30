#ifndef WORK_QUEUE_H_
#define WORK_QUEUE_H_

#include <chrono>
#include <deque>
#include <functional>

struct WorkItem {
  enum class Result {
    kDoNotRunOptimization, // 执行任务结束后不需要执行优化
    kRunOptimization,      // 执行任务结束后需要执行优化
  };

  std::chrono::steady_clock::time_point time;

  // task为 一个函数的名字, 这个函数返回值类型为Result, 参数列表为(), 为空
  std::function<Result()> task;
};

// 双端队列
using WorkQueue = std::deque<WorkItem>;


#endif 
