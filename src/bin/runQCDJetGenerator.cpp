#include <vector>
#include <string>

#include "pythiaEvent.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/SharedPtr.hh"


int main(int argc, char* argv[]){
  int nEvents = 1;
  for(int i=0; i<nEvents; i++){
    pythiaEvent pythia(120, -3, 3, false);
    pythia.next();

    std::vector<fastjet::PseudoJet> particles = pythia.particles();
    std::vector<fastjet::PseudoJet> initiators = pythia.initiators();

    // std::cout << "particles: " << std::endl;
    // for(auto&& p : particles){
    //   std::cout << p.e() << std::endl;
    // }

    std::cout << "initiators: " << std::endl;
    for(auto&& p : initiators){
      std::cout << p.user_info<MyInfo>().pdg_id() << std::endl;
    }
  }

  return 0;
}
