#pragma once

#include "PipelineStage.hpp"
#include "ActiveObject.hpp"
#include "../structures/MST.hpp"

/**
 * @class ExamplePipelineStage
 * @brief A class representing a stage in a pipeline that processes input strings and produces output through a callback.
 *
 * This class inherits from PipelineStage and overrides the process method to handle input strings.
 * It maintains internal state including a flag, a counter, an active object, a graph, and a minimum spanning tree (MST).
 * Thread safety is ensured using a mutex.
 */
class ExamplePipelineStage : public PipelineStage
{
public:
    /**
     * @brief Constructs a new ExamplePipelineStage object.
     */
    ExamplePipelineStage();

    /**
     * @brief Destructor for the ExamplePipelineStage class.
     *
     * This destructor is responsible for cleaning up any resources
     * that the ExamplePipelineStage object may have acquired during
     * its lifetime. It ensures that all necessary cleanup operations
     * are performed to prevent resource leaks.
     */
    ~ExamplePipelineStage();

    /**
     * @brief Processes the input string and produces output through the provided callback.
     *
     * @param input The input string to be processed.
     * @param outputCallback A callback function to handle the processed output string.
     */
    void process(const std::string &input, std::function<void(const std::string &)> outputCallback) override;

private:
    bool newGraphFlag;         ///< Flag indicating whether a new graph is being processed.
    int counter;               ///< Counter for tracking the number of processed items.
    ActiveObject activeObject; ///< An active object used in processing.
    std::unique_ptr<Graph> graph;              ///< Pointer to a graph object.
    std::unique_ptr<MST> mst;                  ///< Pointer to a minimum spanning tree (MST) object.
    std::mutex mtx;            ///< Mutex for ensuring thread safety.

    /**
     * @brief Parses the input string.
     *
     * @param input The input string to be parsed.
     * @return The parsed string.
     */
    std::string PLparse(std::string input);

    /**
     * @brief Adds an edge to the graph.
     *
     * @param node1 The first node of the edge.
     * @param node2 The second node of the edge.
     * @param weight The weight of the edge.
     */
    void PLaddedge(std::string node1, std::string node2, std::string weight);
};