#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "PipelineStage.hpp"

/**
 * @class Pipeline
 * @brief Represents a sequence of processing stages.
 *
 * The Pipeline class allows adding multiple stages of processing and
 * executing them in sequence on a given input.
 */
class Pipeline
{
public:
    /**
     * @brief Destructor for the Pipeline class.
     *
     * This destructor is responsible for cleaning up any resources
     * that the Pipeline class may have allocated during its lifetime.
     * It ensures that all necessary cleanup operations are performed
     * to prevent resource leaks.
     */
    ~Pipeline();

    /**
     * @brief Adds a processing stage to the pipeline.
     *
     * @param stage A shared pointer to the PipelineStage to be added.
     */

    void addStage(std::shared_ptr<PipelineStage> stage);

    /**
     * @brief Processes the input through all stages in the pipeline.
     *
     * @param input The input string to be processed.
     * @param outputCallback A callback function to handle the output string after processing.
     */
    void process(const std::string &input, std::function<void(const std::string &)> outputCallback);

private:
    std::vector<std::shared_ptr<PipelineStage>> stages;
};
