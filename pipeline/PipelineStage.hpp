#pragma once

#include <string>
#include <functional>

class PipelineStage {
public:
    virtual ~PipelineStage() {}
    virtual void process(const std::string& input, std::function<void(const std::string&)> outputCallback) = 0;
};
