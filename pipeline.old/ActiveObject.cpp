#include "ActiveObject.hpp"

ActiveObject::ActiveObject() : worker(&ActiveObject::run, this), running(true) {}

ActiveObject::~ActiveObject()
{
    running = false;
    tasksCondVar.notify_all();
    if (worker.joinable())
        worker.join();
}

void ActiveObject::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        tasks.push(task);
    }
    tasksCondVar.notify_one();
}

void ActiveObject::run()
{
    while (running)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            tasksCondVar.wait(lock, [this]
                              { return !tasks.empty() || !running; });
            if (!running && tasks.empty())
                return;
            task = tasks.front();
            tasks.pop();
        }
        if (task)
            task();
    }
}
