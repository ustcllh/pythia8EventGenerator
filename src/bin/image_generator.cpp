#include <vector>
#include <string>

#include "pythiaEvent.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/SharedPtr.hh"

// #include "Pixeliser.h"

int main(int argc, char* argv[]){
  int nEvents = 1;

  // Jet definition
  double R = 0.4;
  double ptmin = 130.;
  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);

  // jets
  std::vector<fastjet::PseudoJet> qjets;
  std::vector<fastjet::PseudoJet> gjets;

  for(int i=0; i<nEvents; i++){

    // pthat, rapmin, rapmax, partonlevel
    pythiaEvent pythia(120, -3, 3, false);
    pythia.next();

    std::vector<fastjet::PseudoJet> particles = pythia.particles();
    std::vector<fastjet::PseudoJet> initiators = pythia.initiators();

    // inclusive jets
    fastjet::ClusterSequence cs(particles, jet_def);
    std::vector<fastjet::PseudoJet> jets = cs.inclusive_jets(ptmin);


    for(auto&& p : initiators){
      float dr_min = 9999;
      fastjet::PseudoJet j_matched;
      for(auto&& jet : jets){
        float dr = jet.delta_R(p);
        if(dr < dr_min){
          dr_min = dr;
          j_matched = jet;
        }
      }

      auto pid = p.user_info<MyInfo>().pdg_id();
      if(pid == 21){
        gjets.push_back(j_matched);
        ng++;
      }
      else{
        qjets.push_back(j_matched);
        nq++;
      }
    } // loop over initiators
  } // loop over events

  std::cout << "quark jets: " << qjets.size() << std::endl;
  std::cout << "gluon jets: " << gjets.size() << std::endl;

  return 0;
}
