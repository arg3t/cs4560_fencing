CXX = ../build/bin/clang++
SRC = $(wildcard *.cpp)
LLVM_IR = $(SRC:.cpp=.ll)
BITCODE = $(SRC:.cpp=.bc)
CXXFLAGS = -O0 -std=c++17 -emit-llvm

all: $(LLVM_IR) $(BITCODE)

%.ll: %.cpp
	$(CXX) $(CXXFLAGS) -S $< -o $@

%.bc: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(LLVM_IR) $(BITCODE)
