#include "ThreadPool.h"
#include <algorithm>
#include <chrono>
#include <numeric>

#include <memory>
#include "Task.h"

// 执行传入的 task的Execute()函数
void ThreadPoolInterface::Execute(Task* task) { task->Execute(); }

// 执行传入的 task的SetThreadPool()函数
void ThreadPoolInterface::SetThreadPool(Task* task) {
  task->SetThreadPool(this);
}

// 根据传入的数字, 进行线程池的构造, DoWork()函数开始了一个始终执行的for循环
ThreadPool::ThreadPool(int num_threads) {
  //CHECK_GT(num_threads, 0) << "ThreadPool requires a positive num_threads!";
  std::lock_guard<std::mutex> lock(mutex_);
  for (int i = 0; i != num_threads; ++i) {
    pool_.emplace_back([this]() { ThreadPool::DoWork(); });
  }
}

// 只有等待 pool_ 结束所有的线程(join是等待直到线程结束),ThreadPool才能析构完成
ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    //CHECK(running_);
    running_ = false;
  }
  for (std::thread& thread : pool_) {
    thread.join();
  }
}

// task的依赖都结束了, 可以将task放入可执行任务的队列task_queue_中了
void ThreadPool::NotifyDependenciesCompleted(Task* task) {
  std::lock_guard<std::mutex> lock(mutex_);

  // 找到task的索引
  auto it = tasks_not_ready_.find(task);
  //CHECK(it != tasks_not_ready_.end());

  // 加入到任务队列中
  task_queue_.push_back(it->second);
  // 从未准备好的任务队列中删除task
  tasks_not_ready_.erase(it);
}

// 将task插入到tasks_not_ready_队列中, 并执行task的SetThreadPool()函数
std::weak_ptr<Task> ThreadPool::Schedule(std::unique_ptr<Task> task) {
  std::shared_ptr<Task> shared_task;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    auto insert_result =
        tasks_not_ready_.insert(std::make_pair(task.get(), std::move(task)));

    // map::insert() 会返回pair<map::iterator,bool> 类型, 
    // 第一个值为迭代器, 第二个值为插入操作是否成功
    //CHECK(insert_result.second) << "Schedule called twice";
    shared_task = insert_result.first->second;
  }
  SetThreadPool(shared_task.get());
  return shared_task;
}

// 开始一个不停止的for循环, 如果任务队列不为空, 就执行第一个task
void ThreadPool::DoWork() {
#ifdef __linux__
  // This changes the per-thread nice level of the current thread on Linux. We
  // do this so that the background work done by the thread pool is not taking
  // away CPU resources from more important foreground threads.
  //CHECK_NE(nice(10), -1);
#endif

  const auto predicate = [this](){
    return !task_queue_.empty() || !running_;
  };

  // 始终执行, 直到running_为false时停止执行
  for (;;) {
    std::shared_ptr<Task> task;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      //mutex_.Await(absl::Condition(&predicate));//等condition修改

      // map_builder.lua中设置的线程数, 4个线程处理同一个task_queue_
      // 如果任务队列不为空, 那就取出第一个task
      if (!task_queue_.empty()) {
        task = std::move(task_queue_.front());
        task_queue_.pop_front();
      } else if (!running_) {
        return;
      }
    }
    // CHECK(task);
    // CHECK_EQ(task->GetState(), common::Task::DEPENDENCIES_COMPLETED);

    // 执行task
    Execute(task.get());
  }
}
