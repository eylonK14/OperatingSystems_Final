
#pragma once

#include <string>
#include <functional>

/**
 * @class PipelineStage
 * @brief Abstract base class representing a stage in a processing pipeline.
 *
 * The PipelineStage class defines a common interface for all stages in a pipeline.
 * Each stage is responsible for processing input data and passing the result to the next stage.
 */
class PipelineStage
{
public:
    /**
     * @brief Virtual destructor for the PipelineStage class.
     *
     * Ensures that derived class destructors are called properly.
     */
    virtual ~PipelineStage() {}

    /**
     * @brief Processes the input data and passes the result to the output callback.
     *
     * This pure virtual function must be implemented by derived classes to define
     * the specific processing logic for the pipeline stage.
     *
     * @param input The input data to be processed.
     * @param outputCallback A callback function to which the processed output data is passed.
     */
    virtual void process(const std::string &input, std::function<void(const std::string &)> outputCallback) = 0;
};
