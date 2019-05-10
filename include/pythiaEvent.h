// class of pythia event generator
// modified from https://github.com/mverwe/JetToyHI/blob/master/include/pythiaEvent.hh

#ifndef __PYTHIAEVENT_H__
#define __PYTHIAEVENT_H__

#include <iostream>
#include <vector>
#include <string>

#include "Pythia8/Pythia.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/SharedPtr.hh"

class MyInfo: public fastjet::PseudoJet::UserInfoBase {
public:
     MyInfo(int id) : _pdg_id(id){};
     int pdg_id() const {return _pdg_id;};
private:
     int _pdg_id;
};

class pythiaEvent {
private :
  Pythia8::Pythia pythia;
  double _pthat;
  // tunes disabled
  // unsigned int tune_;
  double _rapMin;
  double _rapMax;
  bool   _partonLevel;

  // vector of final state particles
  std::vector<fastjet::PseudoJet> _particles;
  // vector of jet initiators
  std::vector<fastjet::PseudoJet> _initiators;

public :

  pythiaEvent(double pthat, double rapMin, double rapMax, bool partonLevel);
  ~pythiaEvent();
  void next();
  std::vector<fastjet::PseudoJet> particles();
  std::vector<fastjet::PseudoJet> initiators();


};

// tunes disabled
// pythiaEvent(double pthat = 120., unsigned int tune = 14, double rapMin = -3., double rapMax = 3., bool partonLevel = false) : pthat_(pthat), tune_(tune), rapMin_(rapMin), rapMax_(rapMax), partonLevel_(partonLevel)

pythiaEvent::pythiaEvent(double pthat = 120., double rapMin = -3., double rapMax = 3., bool partonLevel = false) : _pthat(pthat), _rapMin(rapMin), _rapMax(rapMax), _partonLevel(partonLevel)
{

  // Generator. LHC process and output selection. Initialization.
  // tunes: http://home.thep.lu.se/~torbjorn/pythia82html/Tunes.html

  pythia.readString("Beams:eCM = 5020.");
  pythia.readString("HardQCD:all = on");

  std::string pthat_s = "PhaseSpace:pTHatMin = ";
  pthat_s += std::to_string(_pthat);
  pythia.readString(pthat_s);

  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberShowEvent = 0");

  // tunes disabled
  //pythia.readString(Form("Tune:pp = %d",tune_));
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 0");
  if(_partonLevel) {
    pythia.readString("HadronLevel:all = off");
  }
  _particles.clear();
  _initiators.clear();
  pythia.init();

}

pythiaEvent::~pythiaEvent(){}

void pythiaEvent::next() {
  _particles.clear();
  _initiators.clear();
  pythia.next();

  // loop over particles
  for (int i = 0; i < pythia.event.size(); ++i) {
    auto particle = pythia.event[i];

    if (particle.isFinal()) {
      fastjet::PseudoJet p( particle.px(), particle.py(), particle.pz(), particle.e());
      // extra info disabled
      //p.set_user_info(new extraInfo(pythia.event[i].id(), 0));

      if(p.rap()>_rapMin && p.rap()<_rapMax)
        _particles.push_back(p);
    }
    // select outgoing parton from HardQCD process
    else if(particle.status()==-23){
      fastjet::PseudoJet p( particle.px(), particle.py(), particle.pz(), particle.e());
      auto pid = particle.id();
      p.set_user_info(new MyInfo(pid));
      _initiators.push_back(p);
    }
  }
}

std::vector<fastjet::PseudoJet> pythiaEvent::particles(){
  return _particles;
}

std::vector<fastjet::PseudoJet> pythiaEvent::initiators(){
  return _initiators;
}

#endif //__PYTHIAEVENT_H__
