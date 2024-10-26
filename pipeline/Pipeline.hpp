#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "PipelineStage.hpp"

class Pipeline
{
public:
    void addStage(std::shared_ptr<PipelineStage> stage);
    void process(const std::string &input, std::function<void(const std::string &)> outputCallback);

private:
    std::vector<std::shared_ptr<PipelineStage>> stages;
};
