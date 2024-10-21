#include <iostream>
#include <thread>
#include <list>
#include <memory>

class WorkCenter
{
private:
    Worker *leader;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::vector<std::shared_ptr<Worker>> workers_;

public:
    // Default constructor
    WorkCenter() : leader(nullptr) {}

    // Copy constructor
    WorkCenter(const WorkCenter &other) : leader(other.leader)
    {
        for (const auto &worker : other.workers_)
        {
            workers_.push_back(std::make_unique<Worker>(*worker));
        }
    }

    // Assignment operator
    WorkCenter &operator=(const WorkCenter &other)
    {
        if (this != &other)
        {
            delete leader;
            leader = nullptr;
            workers_.clear();

            for (const auto &worker : other.workers_)
            {
                workers_.push_back(std::make_unique<Worker>(*worker));
            }
        }
        return *this;
    }

    // Destructor
    ~WorkCenter()
    {
        delete leader;
        for (auto &worker : workers_)
        {
            delete worker.get();
        }
    }

    void createWorkers(int numberOfWorkers, TaskSet taskSet, TaskHandler taskHandler)
    {
        for (int id = 1; id <= numberOfWorkers; id++)
        {
            Worker *worker = new Worker(id, this, taskSet, taskHandler);
            workers_.push_back(std::make_unique<Worker>(*worker));
        }
        promoteLeader();
    }

    void promoteLeader()
    {
        if (!workers_.empty())
        {
            leader = workers_.front().get();
        }
    }
};

#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>

class WorkCenter
{
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::vector<std::shared_ptr<Worker>> workers_;

public:
    // ... (other methods remain the same)
};

class Worker : public std::enable_shared_from_this<Worker>
{
private:
    long id;
    std::weak_ptr<WorkCenter> workCenter;
    std::shared_ptr<TaskSet> taskSet;
    std::shared_ptr<TaskHandler> taskHandler;

public:
    Worker(long id, std::weak_ptr<WorkCenter> workCenter, std::shared_ptr<TaskSet> taskSet, std::shared_ptr<TaskHandler> taskHandler)
        : id(id), workCenter(workCenter), taskSet(taskSet), taskHandler(taskHandler) {}

    void run()
    {
        while (!std::this_thread::is_interruped())
        {
            try
            {
                std::lock_guard<std::mutex> lock(workCenter->mutex_);
                if (workCenter->getLeader() && workCenter->getLeader() != shared_from_this())
                {
                    workCenter->cv_.wait(lock);
                    continue;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << std::endl;
                break;
            }
            std::lock_guard<std::mutex> lock(workCenter->mutex_);
            auto task = taskSet->getTask();
            workCenter->removeWorker(shared_from_this());
            workCenter->promoteLeader();
            workCenter->cv_.notify_all();
            taskHandler->handleTask(*task);
            workCenter->addWorker(shared_from_this());
        }
    }
};
