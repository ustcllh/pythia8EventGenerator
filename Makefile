CXX=/usr/local/Cellar/gcc/9.1.0/bin/g++-9
CXXFLAGS=-g -std=c++11 -Wall -pedantic

PYTHIA_PATH=/Users/liulihan/Library/Pythia8240
PYTHIA_FLAG=-I$(PYTHIA_PATH)/include -L$(PYTHIA_PATH)/lib -lpythia8

FASTJET_PATH=/Users/liulihan/Library/fastjet
FASTJET_FLAG=-I$(FASTJET_PATH)/include -L$(FASTJET_PATH)/lib -lfastjet

SRC=src
INC=include

all: test runQCDJetGenerator image_generator

test: $(SRC)/bin/test.cc
	$(CXX) $(CXXFLAGS) $(PYTHIA_FLAG) $(FASTJET_FLAG) -o $@ $<

runQCDJetGenerator: $(SRC)/bin/runQCDJetGenerator.cpp
	$(CXX) $(CXXFLAGS) $(PYTHIA_FLAG) $(FASTJET_FLAG) -I$(INC) -o $@ $<

image_generator: $(SRC)/bin/image_generator.cpp
	$(CXX) $(CXXFLAGS) $(PYTHIA_FLAG) $(FASTJET_FLAG) -I$(INC) -o $@ $<

.PHONY:
	clean
	all

clean:
	rm -rf test
	rm -rf runQCDJetGenerator
	rm -rf image_generator
