CPP=g++
CPPFLAGS=--std=c++17
CPPGCOVFLAGS=-fprofile-arcs -ftest-coverage
SRC=src/*.cpp
INC=inc
TESTDIR=test
TESTSRC=$(TESTDIR)/*.cpp
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
	$(CPP) $(CPPFLAGS) $(CPPGCOVFLAGS) $(TESTSRC) -I$(GTESTINC) -I$(INC) -L$(GTESTLIBDIR) $(GTESTLIBS) -o tests
	./tests && gcov -r $(TESTDIR)

clear:
	rm -r $(GTEST)
	rm tests
	rm *.gcov *.gcda *.gcno
