#include "Pipeline.hpp"

void Pipeline::addStage(std::shared_ptr<PipelineStage> stage)
{
    stages.push_back(stage);
}

void Pipeline::process(const std::string& input, std::function<void(const std::string&)> outputCallback) {
    if (stages.empty()) {
        outputCallback(input);
        return;
    }

    // Use a shared_ptr to allow the lambda to capture itself by value
    auto processStage = std::make_shared<std::function<void(size_t, const std::string&)>>();
    *processStage = [this, processStage, outputCallback](size_t index, const std::string& data) {
        if (index < stages.size()) {
            stages[index]->process(data, [this, processStage, index, outputCallback](const std::string& output) {
                (*processStage)(index + 1, output);
            });
        } else {
            outputCallback(data);
        }
    };

    // Start processing
    (*processStage)(0, input);
}
