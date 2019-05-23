#include <vector>
#include <string>

#include "pythiaEvent.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/SharedPtr.hh"

#include "Pixeliser.h"

int main(int argc, char* argv[]){
  if(argc != 2){
    std::cout << "Usage: ./image_generator <outDir>" << std::endl;
    exit(1);
  }

  // configuration
  double pthat = 120.;
  int nEvents = 100;

  // output
  std::string outdir = argv[1];

  // Jet definition
  double R = 0.4;
  double ptmin = 130.;
  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);

  // jets container
  std::vector<fastjet::PseudoJet> qjets;
  std::vector<fastjet::PseudoJet> gjets;

  // pthat, rapmin, rapmax, partonlevel
  pythiaEvent pythia(pthat, -3, 3, false);

  // count for jet images
  int nq=0;
  int ng=0;

  for(int i=0; i<nEvents; i++){
    qjets.clear();
    gjets.clear();
    pythia.next();

    std::vector<fastjet::PseudoJet> particles = pythia.particles();
    std::vector<fastjet::PseudoJet> initiators = pythia.initiators();

    // inclusive jets
    fastjet::ClusterSequence cs(particles, jet_def);
    std::vector<fastjet::PseudoJet> jets = cs.inclusive_jets(ptmin);

    for(auto&& it=jets.begin(); it!=jets.end(); it++){
      if(!(it->has_structure())) jets.erase(it);
    }

    // matching and tagging
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
      if(dr_min>0.4) continue;

      // quark/gluon jet tagging
      auto pid = p.user_info<MyInfo>().pdg_id();
      if(pid == 21){
        gjets.push_back(j_matched);
      }
      else{
        qjets.push_back(j_matched);
      }
    } // loop over initiators

    // generate image
    for(auto&& jet: gjets){
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
      p.plot(outdir, "gluon_jet_" + std::to_string(ng));
      // p.print();
      p.save(outdir, "gluon_jet_" + std::to_string(ng));

      ng++;
    } // loop over gluon jets

    for(auto&& jet: qjets){
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
      p.plot(outdir, "quark_jet_" + std::to_string(nq));
      // p.print();
      p.save(outdir, "quark_jet_" + std::to_string(nq));
      nq++;
    } // loop over gluon jets



  } // loop over events


  return 0;
}
