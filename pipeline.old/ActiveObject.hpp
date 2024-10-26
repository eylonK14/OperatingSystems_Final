#pragma once

#include <algorithm>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <utility>
#include <vector>

#include <queue>
#include <condition_variable>
#include <atomic>

/**
 * @class ActiveObject
 * @brief A class that represents an active object which processes tasks asynchronously.
 *
 * The ActiveObject class provides a mechanism to enqueue tasks and process them
 * in a separate thread. It manages a queue of tasks and ensures that they are
 * executed in the order they are received.
 *
 * @note The class uses a worker thread to process tasks and employs synchronization
 * mechanisms to ensure thread safety.
 */
class ActiveObject
{
public:
    /**
     * @brief Constructor for the ActiveObject class.
     * Initializes the worker thread and sets the running flag to true.
     */
    ActiveObject();
    /**
     * @brief Destructor for the ActiveObject class.
     * Signals the worker thread to stop and waits for it to finish.
     */
    ~ActiveObject();
    /**
     * @brief Enqueues a task to be executed by the worker thread.
     *
     * @param task A std::function<void()> representing the task to be executed.
     */
    void enqueue(std::function<void()> task);
    void stop();

private:
    /**
     * @brief Executes the main logic of the ActiveObject.
     *
     * This method is responsible for running the core functionality
     * of the ActiveObject. It typically contains the loop or logic
     * that processes tasks or events.
     */
    void run();

    std::thread worker;
    std::queue<std::function<void()>> tasks;
    std::mutex tasksMutex;
    std::condition_variable tasksCondVar;
    std::atomic<bool> running;
};
