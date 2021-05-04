#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Threading/TeThreading.h"
#include "Utility/TeModule.h"

#include <functional>
#include <atomic>

namespace te
{
    class TE_UTILITY_EXPORT Task
    {
    public:
        Task(const String& name, std::function<void()> taskWorker, std::function<void()> callback = nullptr);

        /**
         * Creates a new task. Task should be provided to TaskScheduler in order for it to start.
         *
         * @param[in]	name		Name you can use to more easily identify the task.
         * @param[in]	taskWorker	Worker method that does all of the work in the task.
         * @param[in]	callback  	(optional) Method to call when task is complete
         */
        static SPtr<Task> Create(const String& name, std::function<void()> taskWorker, std::function<void()> callback = nullptr);

        /** Returns true if the task has completed. */
        bool IsComplete() const;

        /** Returns true if the task has started or completed execution. */
        bool IsExecuting() const;

        /**	Returns true if the task has been canceled. */
        bool IsCanceled() const;

        /** Cancels the task and removes it from the TaskSchedulers queue. */
        void Cancel();

        /** Calls worker method */
        void Execute();

    private:
        friend class TaskScheduler;

        String _name;
        std::function<void()> _taskWorker;
        std::function<void()> _callback;
        std::atomic<UINT32> _state{ 0 }; /**< 0 - Inactive, 1 - In progress, 2 - Completed, 3 - Canceled */
    };

    /**
     * Represents a task scheduler running on multiple threads. You may queue tasks on it from any thread and they will be
     * executed in user specified order on any available thread.
     *
     * @note
     * Thread safe.
     * 
     * @note
     * This type of task scheduler uses a global queue and is best used for coarse granularity of tasks. (Number of tasks
     * in the order of hundreds. Higher number of tasks might require different queuing and locking mechanism, potentially
     * at the cost of flexibility.)
     * 
     * @note
     * By default the task scheduler will create as many threads as there are physical CPU cores.
     */
    class TE_UTILITY_EXPORT TaskScheduler : public Module<TaskScheduler>
    {
    public:
        TaskScheduler();
        ~TaskScheduler();

        /** Queues a new task. */
        void AddTask(SPtr<Task> task);

    protected:
        friend class Task;

        /**	Main task scheduler method that dispatches tasks to other threads. */
        void RunThread();

        /** Waits for all executing (and queued if requested) tasks to finish */
        void Flush();

        /** Returns true if at least one task is running */
        bool AreTasksRunning() const { return GetThreadsAvailable() != GetThreadCount(); }

        /** Get the number of threads used */
        UINT32 GetThreadCount() const { return _threadCount; }

        /** Get the number of threads which are not doing any work */
        uint32_t GetThreadsAvailable() const;

    protected:
        bool _shutdown = false;
        UINT32 _threadCount;
        UINT32 _threadCountSupport;
        Vector<Thread> _threads;
        Deque<SPtr<Task>> _tasks;
        Mutex _mutexTasks;
        Signal _conditionVar;
    };

    TE_UTILITY_EXPORT TaskScheduler& gTaskScheduler();
}
