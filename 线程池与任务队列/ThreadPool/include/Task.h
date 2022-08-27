#ifndef TASK_H_
#define TASK_H_

#include <set>

#include <mutex>
#include "ThreadPool.h"

class ThreadPoolInterface;

class Task {
 public:
  friend class ThreadPoolInterface;

  using WorkItem = std::function<void()>;
  enum State { NEW, DISPATCHED, DEPENDENCIES_COMPLETED, RUNNING, COMPLETED };

  /**
    NEW：新建任务, 还未schedule到线程池
    DISPATCHED： 任务已经schedule 到线程池
    DEPENDENCIES_COMPLETED： 任务依赖已经执行完成
    RUNNING： 任务执行中
    COMPLETED： 任务完成

    对任一个任务的状态转换顺序为：
    NEW->DISPATCHED->DEPENDENCIES_COMPLETED->RUNNING->COMPLETED
  */

  Task() = default;
  ~Task();

  State GetState() ;

  // State must be 'NEW'.
  void SetWorkItem(const WorkItem& work_item);

  // State must be 'NEW'. 'dependency' may be nullptr, in which case it is
  // assumed completed.
  void AddDependency(std::weak_ptr<Task> dependency);

 private:
  // Allowed in all states.
  void AddDependentTask(Task* dependent_task);

  // State must be 'DEPENDENCIES_COMPLETED' and becomes 'COMPLETED'.
  void Execute();

  // State must be 'NEW' and becomes 'DISPATCHED' or 'DEPENDENCIES_COMPLETED'.
  void SetThreadPool(ThreadPoolInterface* thread_pool);

  // State must be 'NEW' or 'DISPATCHED'. If 'DISPATCHED', may become
  // 'DEPENDENCIES_COMPLETED'.
  void OnDependenyCompleted();

  // 需要执行的任务
  WorkItem work_item_;
  ThreadPoolInterface* thread_pool_to_notify_ = nullptr;
  // 初始状态为NEW
  State state_ = NEW;
  // 本任务依赖的任务的个数
  unsigned int uncompleted_dependencies_ = 0;
  // 依赖本任务的其他任务
  std::set<Task*> dependent_tasks_;

  std::mutex mutex_;
};

#endif 
