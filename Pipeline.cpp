#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

class Pipeline
{
public:
    void addStage(std::function<void(std::vector<int> &)> stage)
    {
        stages.push_back(stage);
    }

    void run(std::vector<int> &data)
    {
        for (auto &stage : stages)
        {
            stage(data);
        }
    }

private:
    std::vector<std::function<void(std::vector<int> &)>> stages;
};

void stage1(std::vector<int> &data)
{
    for (auto &item : data)
    {
        item *= 2; // Example operation: multiply each element by 2
    }
}

void stage2(std::vector<int> &data)
{
    data.erase(std::remove_if(data.begin(), data.end(), [](int n)
                              { return n < 5; }),
               data.end());
}

int main()
{
    Pipeline pipeline;
    pipeline.addStage(stage1);
    pipeline.addStage(stage2);

    std::vector<int> data = {1, 2, 3, 4, 5};

    pipeline.run(data);

    for (int item : data)
    {
        std::cout << item << " ";
    }

    return 0;
}