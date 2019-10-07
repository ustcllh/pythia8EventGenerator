// pythia event generator
// Author: Lihan Liu
//
// vertex column changed
// *--------------------------------------*
// | .pu14 format (modified)              |
// | # event 0                            |
// | px py pz m pid (vertex->init)        |
// | end                                  |
// | #event 1                             |
// | ...                                  |
// *--------------------------------------*
//
// note:
// initiators             -> init = 1
// final state particles  -> init = 0

#include <vector>
#include <string>
#include <fstream>

#include "pythiaEvent.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/SharedPtr.hh"


// #include "Pixeliser.h"

int main(int argc, char* argv[]){

  if(argc != 3){
    std::cout << "Usage: ./runQCDJetGenerator <outDir> <batch no.> " << std::endl;
    exit(1);
  }

  // configuration
  double pthat = 120.;
  int nEvents = 10000;

  // output
  std::string outdir = argv[1];
  int nbatch = std::stoi(argv[2]);

  if(outdir.back() == '/'){
    outdir.pop_back();
  }

  std::string ofn = outdir + "/pythia_" + std::to_string((int) pthat) + "_" + std::to_string(nbatch) + ".pu14";
  std::ofstream ofs(ofn, std::ofstream::out);

  // pthat, rapmin, rapmax, partonlevel
  pythiaEvent pythia(pthat, -3, 3, false);
  for(int i=0; i<nEvents; i++){
    pythia.next();

    std::vector<fastjet::PseudoJet> particles = pythia.particles();
    std::vector<fastjet::PseudoJet> initiators = pythia.initiators();

    ofs << "# event " << i << std::endl;
    // initiators
    for(auto&& p : initiators){
      int pid = p.user_info<MyInfo>().pdg_id();
      int init = 1;

      ofs << std::fixed << p.px() << " " << p.py() << " " << p.pz() << " " << p.m() << " ";
      ofs << pid << " " << init << "" << std::endl;
    }

    // final state particles
    for(auto&& p : particles){
      int pid = p.user_info<MyInfo>().pdg_id();
      int init = 0;

      ofs << std::fixed << p.px() << " " << p.py() << " " << p.pz() << " " << p.m() << " ";
      ofs << pid << " " << init << "" << std::endl;
    }
    ofs << "end " << std::endl;
  } // loop over events
  ofs.close();

  return 0;
}
