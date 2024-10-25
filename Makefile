CXXFLAGS = -pg -g -Wall -Werror -Wextra -std=c++17 -pthread -fprofile-arcs -ftest-coverage -lgcov
CXX = g++
TARGET = Main

SRCS = $(wildcard leaderfollower/*.cpp pipeline/*.cpp structures/*.cpp Main.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o gmon.out *.gcov *.gcda *.gcno $(TARGET)
	rm -rf leaderfollower/*.o leaderfollower/*.gcda leaderfollower/*.gcno
	rm -rf pipeline/*.o pipeline/*.gcda pipeline/*.gcno
	rm -rf structures/*.o structures/*.gcda structures/*.gcno

.PHONY: clean all