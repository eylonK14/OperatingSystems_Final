CXXFLAGS = -pg -g -Wall -Werror -Wextra -std=c++17 -pthread -fprofile-arcs -ftest-coverage -lgcov
OBJS_LF = leaderfollower/MainLeaderFollower.o leaderfollower/ServerLeaderFollower.o
OBJS_PL = pipeline/MainPipeline.o pipeline/ActiveObject.o pipeline/ExamplePipelineStage.o pipeline/Pipeline.o pipeline/ServerPipelineActiveObject.o
OBJS_STRUCT = structures/Graph.o MST.o structures/MSTAlgorithmFactory.o structures/UnionFind.o

all: MainLeaderFollower MainPipeline

MainLeaderFollower: $(OBJS) $(OBJS_LF)
	$(CXX) $(CXXFLAGS) $(OBJS) $(OBJS_LF) -o $@ $^

MainPipeline: $(OBJS) $(OBJS_PL)
	$(CXX) $(CXXFLAGS) $(OBJS) $(OBJS_PL) -o $@ $^

*.o: *.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o gmon.out *.gcov *.gcda *.gcno MainLeaderFollower MainPipeline