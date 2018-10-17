CXX = g++-8
CXXSTD = -std=c++17
CPPFLAGS = -pedantic -I../../json/include
CXXFLAGS = -Wall -Wextra -Werror -O3

SOURCES := $(shell find . -name '*.cpp')
DEPENDS := $(SOURCES:%.cpp=build/%.d)
BINARIES := $(SOURCES:%.cpp=build/%)

all: $(BINARIES)

build/%.d: %.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(CPPFLAGS) -MM -MQ $@ $< -o $@

build/%: %.cpp build/%.d
	$(CXX) $(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

-include $(DEPENDS)