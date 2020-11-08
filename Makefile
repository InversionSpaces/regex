CPP=g++
CPPFLAGS=--std=c++17
SRC=src/*.cpp
INC=inc
TESTSRC=test/*.cpp
GTESTSRC=googletest
GTESTINC=googletest/googletest/include
GTEST=gtest
GTESTLIBDIR=$(GTEST)/lib
GTESTLIBS=-lpthread -lgtest -lgtest_main

all:
	$(CPP) $(CPPFLAGS) -I$(INC) $(SRC) -o solve

tests:
	git submodule init && git submodule update
	mkdir -p $(GTEST)
	cmake -S $(GTESTSRC) -B $(GTEST)
	make -C $(GTEST)
	$(CPP) $(CPPFLAGS) $(TESTSRC) -I$(INC) -L$(GTESTLIBDIR) -I$(GTESTINC) $(GTESTLIBS) -o tests

clear:
	rm -r $(GTEST)
