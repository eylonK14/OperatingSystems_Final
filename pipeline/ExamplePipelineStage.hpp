#pragma once

#include "PipelineStage.hpp"
#include "ActiveObject.hpp"
#include "../structures/MST.hpp"

class ExamplePipelineStage : public PipelineStage {
public:
    ExamplePipelineStage();
    void process(const std::string& input, std::function<void(const std::string&)> outputCallback) override;

private:
    bool newGraphFlag;
    int counter;
    ActiveObject activeObject;
    Graph* graph;
    MST* mst;
    std::mutex mtx;

    std::string parse(std::string);
    void addEdge(std::vector<std::string> &);
};