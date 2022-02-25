#include "TeTaskScheduler.h"

namespace te
{
    Task::Task(const String& name, std::function<void()> taskWorker, std::function<void()> callback)
        : _name(name)
        , _taskWorker(std::move(taskWorker))
        , _callback(std::move(callback))
    { }

    SPtr<Task> Task::Create(const String& name, std::function<void()> taskWorker, std::function<void()> callback)
    {
        return te_shared_ptr_new<Task>(name, std::move(taskWorker), std::move(callback));
    }

    bool Task::IsComplete() const
    {
        return _state == 2;
    }

    bool Task::IsCanceled() const
    {
        return _state == 3;
    }

    bool Task::IsExecuting() const
    {
        return _state == 1;
    }

    void Task::Cancel()
    {
        _state = 3;
    }

    void Task::Execute()
    {
        if (!IsCanceled() && _taskWorker)
        {
            _state = 1;
            _taskWorker();
            if (_callback) _callback();
            _state = 2;
        }
    }

    TE_MODULE_STATIC_MEMBER(TaskScheduler)

    TaskScheduler::TaskScheduler()
        : _shutdown(false)
        , _threadCount(0)
        , _threadCountSupport(TE_THREAD_HARDWARE_CONCURRENCY)
    {
        _threadCount = _threadCountSupport - 1; // exclude the main (this) thread
        
        for (UINT32 i = 0; i < _threadCount; i++)
        {
            _threads.emplace_back(Thread(&TaskScheduler::RunThread, this));
        }
    }

    TaskScheduler::~TaskScheduler()
    {
        Flush();

        {
            Lock lock(_mutexTasks);
            _shutdown = true;
        }

        // Wake up all threads.
        _conditionVar.notify_all();

        // Join all threads.
        for (auto& thread : _threads)
        {
            thread.join();
        }

        // Empty worker threads.
        _threads.clear();
    }

    void TaskScheduler::AddTask(SPtr<Task> task)
    {
        if (_threads.empty())
        {
            TE_DEBUG("No available threads, function will execute in the same thread");
            task->Execute();
            return;
        }

        {
            Lock lock(_mutexTasks);
            _tasks.push_back(task);
        }

        // Wake up a thread
        _conditionVar.notify_one();
    }

    void TaskScheduler::RunThread()
    {
        SPtr<Task> task;
        while (true)
        {
            {
                // Lock tasks mutex
                Lock lock(_mutexTasks);

                // Check condition on notification
                _conditionVar.wait(lock, [this] { return !_tasks.empty() || _shutdown; });

                // If m_stopping is true, it's time to shut everything down
                if (_shutdown && _tasks.empty())
                    return;

                // Get next task in the queue.
                task = _tasks.front();

                // Remove it from the queue.
                _tasks.pop_front();

                // Unlock the mutex
                lock.unlock();
            }

            // Execute the task.
            task->Execute();
        }
    }

    void TaskScheduler::Flush()
    {
        // Clear any queued tasks
        _tasks.clear();

        // If so, wait for them
        while (AreTasksRunning())
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    UINT32 TaskScheduler::GetThreadsAvailable() const
    {
        UINT32 availableThreads = _threadCount;

        for (const auto& task : _tasks)
            availableThreads -= task->IsExecuting() ? 1 : 0;

        return availableThreads;
    }

    TaskScheduler& gTaskScheduler()
    {
        return TaskScheduler::Instance();
    }
}
