#include "Pipeline.hpp"

Pipeline::~Pipeline()
{

    for (auto &stage : stages)
        stage.reset();

    stages.clear();
}

void Pipeline::addStage(std::shared_ptr<PipelineStage> stage)
{
    stages.push_back(stage);
}

// void Pipeline::process(const std::string &input, std::function<void(const std::string &)> outputCallback)
// {
//     if (stages.empty())
//     {
//         outputCallback(input);
//         return;
//     }

//     // Use a shared_ptr to allow the lambda to capture itself by value
//     auto processStage = std::make_shared<std::function<void(size_t, const std::string &)>>();
//     *processStage = [this, processStage, outputCallback](size_t index, const std::string &data)
//     {
//         if (index < stages.size())
//         {
//             stages[index]->process(data, [this, processStage, index, outputCallback](const std::string &output)
//                                    { (*processStage)(index + 1, output); });
//         }
//         else
//             outputCallback(data);
//     };

//     // Start processing
//     (*processStage)(0, input);
// }

void Pipeline::process(const std::string &input, std::function<void(const std::string &)> outputCallback)
{
    if (stages.empty())
    {
        outputCallback(input);
        return;
    }

    // Create the shared_ptr but do not capture it in the outer lambda
    auto processStage = std::make_shared<std::function<void(size_t, const std::string &)>>();
    
    *processStage = [this, outputCallback](size_t index, const std::string &data)
    {
        if (index < stages.size())
        {
            // Capture a copy of processStage in the inner lambda to keep it alive
            auto processStageCopy = processStage;
            stages[index]->process(data, [this, index, outputCallback, processStageCopy](const std::string &output)
            {
                (*processStageCopy)(index + 1, output);
            });
        }
        else
        {
            outputCallback(data);
        }
    };

    // Start processing
    (*processStage)(0, input);
}