CXXFLAGS = -g -Wall -Werror -Wextra -std=c++17 -pthread -fprofile-arcs -ftest-coverage -lgcov
CXX = g++
TARGET = Main

SRCS = $(wildcard leaderfollower/*.cpp pipeline/*.cpp structures/*.cpp Main.cpp)
OBJS = $(SRCS:.cpp=.o)
VALGRIND_FLAGS=-v --leak-check=full --show-leak-kinds=all --track-origins=yes  -s  --error-exitcode=99

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

tidy:
	clang-tidy $(SRCS) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,readability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o gmon.out *.gcov *.gcda *.gcno $(TARGET)
	rm -rf leaderfollower/*.o leaderfollower/*.gcda leaderfollower/*.gcno
	rm -rf pipeline/*.o pipeline/*.gcda pipeline/*.gcno
	rm -rf structures/*.o structures/*.gcda structures/*.gcno

.PHONY: clean all