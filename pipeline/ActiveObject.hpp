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

class ActiveObject
{
public:
    ActiveObject();
    ~ActiveObject();
    void enqueue(int);
    std::function<void(int)> currentTask;
    bool previousTaskFinished;

private:
    void run();
    std::thread worker;
    std::queue<std::function<void(int)>> tasks;
    std::mutex tasksMutex;
    std::condition_variable tasksCondVar;
    std::atomic<bool> running;
    int fd;
    
    
};
