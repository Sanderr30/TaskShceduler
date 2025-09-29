#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

#include "hlprs_std/any.h"
#include "hlprs_std/invoke.h"
#include "hlprs_std/tuple.h"
#include "hlprs_std/apply.h"


template<typename T>
class FutureResult;


class TTaskScheduler {
public:
    using SchedulerTaskId = size_t;

public:
    TTaskScheduler() = default;

    ~TTaskScheduler() {
        tasks_.clear();
    }

    TTaskScheduler(const TTaskScheduler& other) = delete;

    TTaskScheduler& operator=(const TTaskScheduler& other) = delete;

    TTaskScheduler(TTaskScheduler&& other) noexcept
        : tasks_(std::move(other.tasks_))
        , task_id_(std::move(other.task_id_))
        , dependency_graph_(std::move(other.dependency_graph_)) 
    {}

    TTaskScheduler& operator=(TTaskScheduler&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        tasks_.clear();
        tasks_ = std::move(other.tasks_);
        task_id_ = std::move(other.task_id_);
        dependency_graph_ = std::move(other.dependency_graph_);
        return *this;
    }

public:
    template<typename CallableObj, typename... Args>
    auto add(CallableObj&& callable_object, Args&&... args) {
        using TskImplmnttn = TaskImplementation<
                                std::decay_t<CallableObj>,
                                std::decay_t<Args>...>;

        SchedulerTaskId new_id = tasks_.size();

        auto task_ptr = std::make_unique<TskImplmnttn>(
            this,
            std::forward<CallableObj>(callable_object),
            std::forward<Args>(args)...
        );

        std::unordered_set<SchedulerTaskId> deps;
        AddDependencies(deps, args...);

        dependency_graph_[new_id] = std::move(deps);

        if (DetectCycle(new_id)) {
            dependency_graph_.erase(new_id);
            throw std::runtime_error("Detected cycle");
        }

        tasks_.push_back(std::move(task_ptr));
        return new_id;
    }

    template<typename T>
    FutureResult<T> getFutureResult(SchedulerTaskId id) {
        return FutureResult<T>(this, id);
    }

    template<typename T>
    T getResult(SchedulerTaskId id) {
        
        static_assert(!std::is_void<T>::value, "Impossible to get void value");

        auto& task = *tasks_.at(id);
        if (!task.executed) {
            task.Execute();
        }
        return dts::AnyCast<T>(task.getResult());
    }

    void executeAll() {
        for (auto& task : tasks_) {
            if (!task->executed) {
                task->Execute();
            }
        }
    }

private:
    class Task {
    public:
        bool executed = false;
        virtual void Execute() = 0;
        virtual dts::Any& getResult() = 0;
        virtual ~Task() = default;
    };

    template<typename Callable, typename... Args>
    class TaskImplementation : public Task {
    public:
        TaskImplementation(TTaskScheduler* owner, Callable func, Args... args)
            : scheduler_ptr_(owner)
            , function_(std::move(func))
            , task_arguments_(dts::MakeTuple(std::move(args)...)) {}
    
    public:
        void Execute() override {
            if (this->executed) return;

            dts::Apply([this](auto&&... tuple_args) {
                auto args = dts::MakeTuple(
                    scheduler_ptr_->ResolveArg(
                        std::forward<decltype(tuple_args)>(tuple_args)
                    )...
                );
                this->task_result_ = dts::Apply(function_, std::move(args));
            }, task_arguments_);

            this->executed = true;
        }

        dts::Any& getResult() override {
            return task_result_;
        }

    private:
        TTaskScheduler* scheduler_ptr_;
        Callable function_;
        dts::Tuple<Args...> task_arguments_;
        dts::Any task_result_;
    };

private:
    template <typename T>
    T ResolveArg(T&& value) {
        return std::forward<T>(value);
    }

    template <typename T>
    T ResolveArg(const FutureResult<T>& future) {
        return future.get();
    }

    template<typename T>
    void AddDependency(std::unordered_set<SchedulerTaskId>&, T&&) {
    }

    template<typename T>
    void AddDependency(std::unordered_set<SchedulerTaskId>& deps, const FutureResult<T>& fut) {
        deps.insert(fut.task_id_);
    }

    template<typename... Args>
    void AddDependencies(std::unordered_set<SchedulerTaskId>&) {}

    template<typename First, typename... Args>
    void AddDependencies(std::unordered_set<SchedulerTaskId>& deps, First&& first, Args&&... args) {
        AddDependency(deps, std::forward<First>(first));
        AddDependencies(deps, std::forward<Args>(args)...);
    }

    bool DetectCycle(SchedulerTaskId start) {
        std::unordered_set<SchedulerTaskId> visited;
        std::unordered_set<SchedulerTaskId> nodes;
        return DFS(start, visited, nodes);
    }

    bool DFS(SchedulerTaskId node, std::unordered_set<SchedulerTaskId>& visited,
                                        std::unordered_set<SchedulerTaskId>& nodes) {
        if (nodes.count(node)) {
            return true;
        }
        
        if (visited.count(node)) {
            return false;
        }

        visited.insert(node);
        nodes.insert(node);

        for (SchedulerTaskId dep : dependency_graph_[node]) {
            if (DFS(dep, visited, nodes)) {
                return true;
            }
        }

        nodes.erase(node);
        return false;
    }

private:
    std::vector<std::unique_ptr<Task>> tasks_;
    SchedulerTaskId task_id_;
    std::unordered_map<SchedulerTaskId, std::unordered_set<SchedulerTaskId>> dependency_graph_;
};


template<typename T>
class FutureResult {
public:
    using SchedulerTaskId = size_t;

public:
    FutureResult(TTaskScheduler* tsk_schdlr_ptr, SchedulerTaskId id)
        : task_scheduller_ptr_(tsk_schdlr_ptr)
        , task_id_(id) {}

    FutureResult(const FutureResult& other)
        : task_scheduller_ptr_(other.task_scheduller_ptr_)
        , task_id_(other.task_id_) {}

    FutureResult& operator=(const FutureResult& other) {
        task_scheduller_ptr_ = other.task_scheduller_ptr_;
        task_id_ = other.task_id_;
        return *this;
    }

    FutureResult(FutureResult&& other) noexcept
        : task_scheduller_ptr_(std::move(other.task_scheduller_ptr_))
        , task_id_(std::move(other.task_id_)) {}

    FutureResult& operator=(FutureResult&& other) noexcept {
        task_scheduller_ptr_ = std::move(other.task_scheduller_ptr_);
        task_id_ = std::move(other.task_id_);
        return *this;
    }

    ~FutureResult() = default;

public:
    T get() const {
        return task_scheduller_ptr_->getResult<T>(task_id_);
    }

    operator T() const {
        return get();
    }

    friend class TTaskScheduler;

private:
    TTaskScheduler* task_scheduller_ptr_;
    SchedulerTaskId task_id_;
};