CXXSTD = -std=c++17
CPPFLAGS = -pedantic -I../../json/include -Iinclude
CXXFLAGS = -Wall -Wextra -Werror -O3

SOURCES := $(shell find . -name '*.cpp')
DEPENDS := $(SOURCES:%.cpp=build/%.d)
BINARIES := $(SOURCES:%.cpp=build/%)

TESTFILES := $(shell find unit_tests -name '*.config')
TESTCASES := $(TESTFILES:%.config=%)

.PHONY: all test

all: test $(BINARIES)

test: build/unit_tests/unit_tests
	build/unit_tests/unit_tests $(TESTCASES)

build/%.d: %.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(CPPFLAGS) -MM -MQ $@ $< -o $@

build/%: %.cpp build/%.d
	$(CXX) $(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

-include $(DEPENDS)
