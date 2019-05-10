CXX=/usr/local/Cellar/gcc/9.1.0/bin/g++-9
CXXFLAGS=-g -std=c++11 -Wall -pedantic

PYTHIA_PATH=/Users/liulihan/Library/Pythia8240
PYTHIA_FLAG=-I$(PYTHIA_PATH)/include -L$(PYTHIA_PATH)/lib -lpythia8

FASTJET_PATH=/Users/liulihan/Library/fastjet
FASTJET_FLAG=-I$(FASTJET_PATH)/include -L$(FASTJET_PATH)/lib -lfastjet

ROOT_FLAG=-pthread -std=c++17 -m64 -I/usr/local/Cellar/root/6.16.00_2/include/root -L/usr/local/Cellar/root/6.16.00_2/lib/root -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lROOTVecOps -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lROOTDataFrame -lpthread -lm -ldl

SRC=src
INC=include

all: test runQCDJetGenerator image_generator QCDJetsComp

test: $(SRC)/bin/test.cc
	$(CXX) $(CXXFLAGS) $(PYTHIA_FLAG) $(FASTJET_FLAG) -o $@ $<

runQCDJetGenerator: $(SRC)/bin/runQCDJetGenerator.cpp
	$(CXX) $(CXXFLAGS) $(PYTHIA_FLAG) $(FASTJET_FLAG) -I$(INC) -o $@ $<

image_generator: $(SRC)/bin/image_generator.cpp
	$(CXX) $(CXXFLAGS) $(PYTHIA_FLAG) $(FASTJET_FLAG) $(ROOT_FLAG) -I$(INC) -o $@ $<

QCDJetsComp: $(SRC)/bin/QCDJetsComp.cpp
	$(CXX) $(CXXFLAGS) $(PYTHIA_FLAG) $(FASTJET_FLAG) $(ROOT_FLAG) -I$(INC) -o $@ $<

.PHONY:
	clean
	all

clean:
	rm -rf test
	rm -rf runQCDJetGenerator
	rm -rf image_generator
	rm -rf QCDJetsComp
