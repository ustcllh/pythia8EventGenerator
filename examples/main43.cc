// main43.cc is a part of the PYTHIA event generator.
// Copyright (C) 2018 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Author: Mikhail Kirsanov, Mikhail.Kirsanov@cern.ch. This program
// illustrates how a file with HepMC events can be generated by
// Pythia8.  It is similar to main42, except that it allows for
// several subruns, e.g. from related LHEF. Input and output files are
// specified on the command line, e.g. like
//     ./main43 main43.cmnd hepmcout43.dat > out
// The main program contains no analysis; this is intended to happen
// later. It therefore "never" has to be recompiled to handle
// different tasks.

// WARNING: typically one needs 25 MB/100 events at the LHC.
// Therefore large event samples may be impractical.

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

using namespace Pythia8;

int main(int argc, char* argv[]) {

  // Check that correct number of command-line arguments
  if (argc != 3) {
    cerr << " Unexpected number of command-line arguments. \n You are"
         << " expected to provide one input and one output file name. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  // Check that the provided input name corresponds to an existing file.
  ifstream is(argv[1]);
  if (!is) {
    cerr << " Command-line file " << argv[1] << " was not found. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  // Confirm that external files will be used for input and output.
  cout << "\n >>> PYTHIA settings will be read from file " << argv[1]
       << " <<< \n >>> HepMC events will be written to file "
       << argv[2] << " <<< \n" << endl;

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC ToHepMC;

  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io(argv[2], std::ios::out);

  // Generator.
  Pythia pythia;

  // Read in subrun-independent commands from external file.
  pythia.readFile( argv[1]);

  // Extract data to be used in main program. Set counters.
  int nSubrun = pythia.mode("Main:numberOfSubruns");
  int nAbort  = pythia.mode("Main:timesAllowErrors");
  int iAbort  = 0;

  // Begin loop over subruns.
  for (int iSubrun = 1; iSubrun <= nSubrun; ++iSubrun) {

    // Read in subrun-specific data from external file.
    pythia.readFile( argv[1], iSubrun);

    // Initialization.
    pythia.init();

    // Print name of Les Houches Event File.
    string lheFile = pythia.settings.word("Beams:LHEF");
    cout << "\n >>> Now begin subrun " << iSubrun
         << " with events from file " << lheFile << " <<< \n"
         << endl;

    // Begin infinite event loop - to be exited at end of file.
    for (int iEvent = 0; ; ++iEvent) {

      // Generate event.
      if (!pythia.next()) {

        // Leave event loop if at end of file.
        if (pythia.info.atEndOfFile()) break;

        // First few failures write off as "acceptable" errors, then quit.
        if (++iAbort < nAbort) continue;
        cout << " Event generation aborted prematurely, owing to error!\n";
        break;
      }

      // Construct new empty HepMC event and fill it.
      // Units will be as chosen for HepMC build, but can be changed
      // by arguments, e.g. GenEvt( HepMC::Units::GEV, HepMC::Units::MM)
      HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
      ToHepMC.fill_next_event( pythia, hepmcevt );

      // Write the HepMC event to file. Done with it.
      ascii_io << hepmcevt;
      delete hepmcevt;

    // End of event loop.
    }

  // End of subrun loop.
  }

  // Statistics.
  pythia.stat();

  // Done.
  return 0;
}
