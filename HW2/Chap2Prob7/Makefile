# Makefile
CXX      = g++        # or g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

# Build any "name" from "name.cpp"
%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# "make" builds everything (all .cpp -> executables)
all: $(patsubst %.cpp,%,$(wildcard *.cpp))

clean:
	rm -f $(patsubst %.cpp,%,$(wildcard *.cpp)) *~
.PHONY: all clean