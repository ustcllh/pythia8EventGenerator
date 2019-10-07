CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic

PYTHIA_PATH=/home/liul11/pythia8240-install
PYTHIA_FLAG=-I$(PYTHIA_PATH)/include 
PYTHIA_LIB=-L$(PYTHIA_PATH)/lib -lpythia8

FASTJET_PATH=/home/liul11/fastjet-insall
FASTJET_FLAG=-I$(FASTJET_PATH)/include 
FASTJET_LIB=-L$(FASTJET_PATH)/lib -lfastjet

ROOT_FLAG=`root-config --cflags`
ROOT_LIB=`root-config --libs`

CPPFLAGS=$(PYTHIA_FLAG) $(FASTJET_FLAG) $(ROOT_FLAG)
LIBS=$(PYTHIA_LIB) $(FASTJET_LIB) $(ROOT_LIB)

SRC=src
INC=include

all: test runQCDJetGenerator image_generator QCDJetsComp

test: $(SRC)/bin/test.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $< $(LIBS)

runQCDJetGenerator: $(SRC)/bin/runQCDJetGenerator.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -I$(INC) -o $@ $< $(LIBS)

image_generator: $(SRC)/bin/image_generator.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -I$(INC) -o $@ $< $(LIBS)

QCDJetsComp: $(SRC)/bin/QCDJetsComp.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -I$(INC) -o $@ $< $(LIBS)

.PHONY:
	clean
	all

clean:
	rm -rf test
	rm -rf runQCDJetGenerator
	rm -rf image_generator
	rm -rf QCDJetsComp
