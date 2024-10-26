# OS Project: MST Server

In this project, we've implemented a server that takes requests from multiple clients and returning responses reagrading and MST object.

A Minimum Spanning Tree (MST) is a selection of edges from a connected, undirected graph that links all vertices with no cycles and with the lowest possible total edge weight. Essentially, an MST is a spanning tree where the total of edge weights is minimized. MSTs are helpful in fields like network design, where minimizing the cost of connecting all nodes is important, such as in the layout of electrical circuits, computer networks, and transportation routes. Kruskal's and Prim's algorithms are widely used for finding the MST by systematically identifying the edges that contribute to the minimum spanning tree.

## Table of Contents

- [OS Project: MST Server](#os-project-mst-server)
  - [Table of Contents](#table-of-contents)
  - [Design Patterns](#design-patterns)
  - [How to use](#how-to-use)
  - [Project Folder Structure](#project-folder-structure)
    - [Example Folder Structure](#example-folder-structure)
    - [Explanation](#explanation)
  - [Running the Program](#running-the-program)
  - [Authors](#authors)

## Design Patterns

The server is implemented using two different design patterns:

1. The Leader-Follower design pattern is a concurrency strategy used to manage multiple threads for efficient task handling. In this pattern, one thread assumes the role of "leader" and is tasked with waiting for and processing incoming events. When this thread is occupied with processing, it designates a "follower" thread to take over as the new leader, ensuring continuous handling of incoming tasks. Once the initial leader finishes its work, it transitions back to a follower role, waiting to be promoted again as needed. This method limits contention and reduces synchronization demands, as only one thread handles events at any given moment. It is useful in environments where tasks arrive at irregular intervals, as it allows for prompt, orderly task processing, such as in server applications managing multiple client requests.

2. The Pipeline design pattern, when combined with the Active Object pattern, organizes a series of processing steps where each step carries out a specific task. In this setup, tasks move through sequential stages, each represented by an Active Object. An Active Object separates the execution of a method from its invocation, improving concurrency. Each stage in the pipeline runs on its own thread, independently processing tasks. Tasks are queued at each stage, with the Active Object pattern managing their asynchronous execution. This approach suits applications that require sequential but independent processing steps, like data transformation or processing pipelines, enabling tasks to be handled concurrently and in a structured manner.

## How to use

Those are the available server commands:

``` cpp
Create New Graph           - `newgraph v e`;
Add Edge                   - `addedge i j w`;
Remove Edge                - `removeedge i j`;
Compute MST                - `boruvka`/`kruskal`/`prim`;
Get Longest Path           - `longestpath`;
Get Shortest Path          - `shortestpath`;
Get Average Path           - `avgpath`;
Get Total Path             - `totalweight`;
Print                      - `print`;
Exit                       - `exit`;
Help (show this help text) - `help`;
```

Certainly! Here is an explanation of the project's folder structure based on the provided context and typical organization for such projects:

## Project Folder Structure

The project is organized into several directories, each serving a specific purpose to maintain a clean and manageable codebase. Below is an overview of the typical structure and the role of each directory:

1. **root directory**:
   - Contains the main entry point of the application, such as Main.cpp , and the primary build configuration files like Makefile.

2. **leaderfollower**:
   - This directory contains the implementation of the Leader-Follower design pattern.
   - Key files might include:
     - `ServerLeaderFollower.cpp` and `ServerLeaderFollower.hpp`: Implementation and header files for the server using the Leader-Follower pattern.
     - `MainLeaderFollower.cpp`: Contains the `main` function to run the server using the Leader-Follower pattern.

3. **pipeline**:
   - This directory contains the implementation of the Pipeline design pattern with Active Objects.
   - Key files might include:
     - `ServerPipelineActiveObject.cpp` and `ServerPipelineActiveObject.hpp`: Implementation and header files for the server using the Pipeline with Active Object pattern.
     - `ActiveObject.cpp` and `ActiveObject.hpp`: Implementation and header files for the Active Object pattern.
     - `ExamplePipelineStage.cpp` and `ExamplePipelineStage.hpp`: Example stages in the pipeline.
     - `MainPipeline.cpp`: Contains the `main` function to run the server using the Pipeline pattern.

4. **structures**:
   - This directory contains the data structures and algorithms used in the project.
   - Key files might include:
     - `Graph.cpp` and `Graph.hpp`: Implementation and header files for the graph data structure.
     - `MST.cpp` and `MST.hpp`: Implementation and header files for the Minimum Spanning Tree algorithms.
     - `MSTAlgorithmFactory.cpp` and `MSTAlgorithmFactory.hpp`: Factory for creating MST algorithms.
     - `UnionFind.cpp` and `UnionFind.hpp`: Implementation and header files for the Union-Find data structure used in MST algorithms.

5. **analysis**:
   - This directory contains the analysis and benchmarking code for the project.

### Example Folder Structure

```sh
project-root/
├── analysis/
│   ├── coverage/
│   │   ├── ActiveObject.cpp.gcov
│   │   ├── ServerPipelineActiveObject.cpp.gcov
│   │   ├── ...
│   ├── valgrind/
│   │   ├── callgrindLF.txt
│   │   ├── ...
├── leaderfollower/
│   ├── ServerLeaderFollower.cpp
│   ├── ServerLeaderFollower.hpp
├── pipeline/
│   ├── ServerPipelineActiveObject.cpp
│   ├── ServerPipelineActiveObject.hpp
│   ├── ActiveObject.cpp
│   ├── ActiveObject.hpp
│   ├── ExamplePipelineStage.cpp
│   ├── ExamplePipelineStage.hpp
├── structures/
│   ├── Graph.cpp
│   ├── Graph.hpp
│   ├── MST.cpp
│   ├── MST.hpp
│   ├── MSTAlgorithmFactory.cpp
│   ├── MSTAlgorithmFactory.hpp
│   ├── UnionFind.cpp
│   ├── UnionFind.hpp
├── README.md
├── Main.cpp
├── Makefile
```

### Explanation

- **leaderfollower**: Contains the implementation of the Leader-Follower design pattern, including the server and main entry point for running the server.
- **pipeline**: Contains the implementation of the Pipeline design pattern with Active Objects, including the server, active objects, pipeline stages, and main entry point for running the server.
- **structures**: Contains the data structures and algorithms used in the project, such as graphs, MST algorithms, and Union-Find.
- **README.md**: Provides documentation on how to use the project, including available server commands and explanations of the design patterns.
- **Main.cpp**: The main entry point of the application.
- **Makefile**: The build configuration file for compiling the project.

This structure helps to keep the code organized, making it easier to manage and understand the different components of the project.

To run the program, you can follow these steps:

## Running the Program

1. **Build the Project**:
   - Navigate to the root directory of the project.
   - Use the provided Makefile to compile the project by running following command in the terminal:

     ```sh
     make / make all / make .PHONY
     ```

2. **Run the Server**:
   - After building the project, you can run the server using the appropriate executable.
   - For the Leader-Follower server, run:

     ```sh
     ./Main --lf <amount_of_threads> --port <port_number>
     ```

   - For the Pipeline with Active Object server, run:

     ```sh
     ./Main --pl --port <port_number>
     ```

3. Connect a Client

   - We recommend using the `netcat` program as a way to connect a client to the server

        ```sh
     nc localhost <port_number>
     ```

## Authors

- [Noam Leshem](https://github.com/noamleshem)
- [Eylon Yaakov Katan](https://github.com/eylonk14)
