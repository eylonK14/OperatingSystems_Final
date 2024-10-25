#include "Pipeline.hpp"

void Pipeline::addStage(std::shared_ptr<PipelineStage> stage)
{
    stages.push_back(stage);
}

void Pipeline::process(const std::string &input, std::function<void(const std::string &)> outputCallback)
{
    if (stages.empty())
    {
        outputCallback(input);
        return;
    }

    std::cout << "Processing pipeline: " << input << std::endl;

    // Recursive lambda to process the stages
    std::function<void(size_t, const std::string &)> processStage;
    processStage = [this, &processStage, &outputCallback](size_t index, const std::string &data)
    {
        if (index < stages.size())
        {
            stages[index]->process(data, [this, index, &processStage, &outputCallback](const std::string &output)
                                   { processStage(index + 1, output); });
        }
        else
        {
            outputCallback(data);
        }
    };

    processStage(0, input);
}
