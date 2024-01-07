CCSOURCES  := $(wildcard *.cc)
CPPSOURCES := $(wildcard *.cpp)

CCOBJECTS  := $(patsubst %.cc,%.o,$(CCSOURCES))
CPPOBJECTS := $(patsubst %.cpp,%.o,$(CPPSOURCES))

CCDEPENDS  := $(patsubst %.cc,%.d,$(CCSOURCES))
CPPDEPENDS := $(patsubst %.cpp,%.d,$(CPPSOURCES))

LDFLAGS := -ggdb3 -lm
CXXFLAGS := -g3 -Og -fdiagnostics-color=always

.PHONY: all clean

all: program

clean:
	@$(RM) $(CCDEPENDS) $(CPPDEPENDS) $(CCOBJECTS) $(CPPOBJECTS) program

program: $(CCOBJECTS) $(CPPOBJECTS)
	@printf "\nLinking %s...\n" $@
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@printf "Done!\n"

-include $(CCDEPENDS) $(CPPDEPENDS)

%.o: %.cc Makefile
	@printf "Compiling %s...\n" $<
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(LDFLAGS)

%.o: %.cpp Makefile
	@printf "Compiling %s...\n" $<
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(LDFLAGS)
