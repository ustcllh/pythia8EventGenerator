// main01.cc is a part of the PYTHIA event generator.
// Copyright (C) 2018 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is a simple test program. It fits on one slide in a talk.
// It studies the charged multiplicity distribution at the LHC.

#include "Pythia8/Pythia.h"
using namespace Pythia8;
int main() {
  // Generator. Process selection. LHC initialization. Histogram.

  // Constructor. (See Pythia.cc file.)
  //Pythia(string xmlDir = "../xmldoc", bool printBanner = true);

  // disable banner
  Pythia pythia("./xmldoc", false);

  pythia.readString("Beams:eCM = 5020.");
  pythia.readString("HardQCD:all = on");
  pythia.readString("PhaseSpace:pTHatMin = 100.");

  pythia.readString("Init:showProcesses = 0");
  pythia.readString("Init:showMultipartonInteractions = 0");
  pythia.readString("Init:showChangedSettings = 0");
  pythia.readString("Init:showChangedParticleData = 0");

  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberShowEvent = 0");

  pythia.init();

  std::cout << "no.\t" << "pid\t" << "name\t" << "status\t" << "px\t" << "py\t" << "pz\t" << "e\t" << std::endl;

  for (int iEvent = 0; iEvent < 2; ++iEvent) {
    if (!pythia.next()) continue;
    for (int i = 0; i < pythia.event.size(); ++i){
      auto p = pythia.event[i];
      if (p.status()==-23){

        std::cout <<
        iEvent << "\t" <<
        p.id() << "\t" <<
        p.name() << "\t" <<
        p.status() << "\t";
        std::cout << std::setprecision(2) << std::fixed <<
        p.px() << "\t" <<
        p.py() << "\t" <<
        p.pz() << "\t" <<
        p.e() << "\t" <<
        std::endl;

      }
    }

  }

  // pythia.stat();
  return 0;
}
