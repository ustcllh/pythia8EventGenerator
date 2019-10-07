#include <vector>
#include <string>

#include "pythiaEvent.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/SharedPtr.hh"

#include "Pixeliser.h"

int main(int argc, char* argv[]){
  if(argc != 3){
    std::cout << "Usage: ./image_generator <outDir> <out_prefix>" << std::endl;
    exit(1);
  }

  // configuration
  double pthat = 150.;
  int nEvents = 60000;

  // output
  std::string outdir = argv[1];
  std::string prefix = argv[2];

  // Jet definition
  double R = 0.4;
  double ptmin = 160.;
  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);

  // pthat, rapmin, rapmax, partonlevel
  pythiaEvent pythia(pthat, -3, 3, false);

  // count for jet images
  int njet = 0;

  for(int i=0; i<nEvents; i++){
    pythia.next();

    std::vector<fastjet::PseudoJet> particles = pythia.particles();

    // inclusive jets
    fastjet::ClusterSequence cs(particles, jet_def);
    std::vector<fastjet::PseudoJet> jets = cs.inclusive_jets(ptmin);
/*
    for(auto it=jets.begin(); it!=jets.end(); ){
      if(!(it->has_structure())) {
        it = jets.erase(it);
      }
      else {
        it++;
      }
    }
*/
    // generate image
    for(auto&& jet: jets){
      pixeliser<float> p (5, 33, 33);
      p.set_title("E;Multi.;|px|;|py|;|pz|");
      float max;
      for(auto&& particle : jet.constituents()){
        float deta = particle.eta() - jet.eta();
        float dphi = particle.phi() - jet.phi();
        // std::cout << deta << "\t" << dphi << "\t" << std::endl;

        max = max>particle.e()? max : particle.e();

        p.fill(0, 0.4, deta, dphi, particle.e());
        p.fill(1, 0.4, deta, dphi, 1.);
        p.fill(2, 0.4, deta, dphi, std::abs(particle.px()));
        p.fill(3, 0.4, deta, dphi, std::abs(particle.py()));
        p.fill(4, 0.4, deta, dphi, std::abs(particle.pz()));
      } // loop over particles
      p.set_maximum(max);
      // p.plot(outdir, prefix + std::to_string(njet));
      // p.print();
      p.save(outdir, prefix + std::to_string(njet));

      njet++;
    } // loop over  jets

    if((i+1) % 100 == 0) {
      std::cout<< "Events Done: " << i+1 << std::endl;
    }

  } // loop over events


  return 0;
}
