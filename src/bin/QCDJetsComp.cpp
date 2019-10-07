#include <vector>
#include <string>

#include "pythiaEvent.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/SharedPtr.hh"

#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"

int main(int argc, char* argv[]){

  // configuration
  double pthat = 120.;
  int nEvents = 10000;

  // Jet definition
  double R = 0.4;
  double ptmin = 130.;
  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);

  // jets container
  std::vector<fastjet::PseudoJet> qjets;
  std::vector<fastjet::PseudoJet> gjets;

  // histograms
  TH1F* h1_n = new TH1F("h1_n", "Initiators Count;No. initiators;counts", 5, 0, 5);
  TH1F* h1_dphi = new TH1F("h1_dphi", "Hard Scattering;#Delta#phi;counts", 100, 0, 5.);

  TH1F* h1_qm = new TH1F("h1_qm", "Quark Jets Invariant Mass;m;counts", 120, 0, 60);
  TH1F* h1_gm = new TH1F("h1_gm", "gluon Jets Invariant Mass;m;counts", 120, 0, 60);


  // pthat, rapmin, rapmax, partonlevel
  pythiaEvent pythia(pthat, -3, 3, false);
  for(int i=0; i<nEvents; i++){
    qjets.clear();
    gjets.clear();
    pythia.next();

    std::vector<fastjet::PseudoJet> particles = pythia.particles();
    std::vector<fastjet::PseudoJet> initiators = pythia.initiators();

    h1_n->Fill(initiators.size());
    if(initiators.size()>=2){
      h1_dphi->Fill(std::abs(initiators[0].phi() - initiators[1].phi()));
    }

    // inclusive jets
    fastjet::ClusterSequence cs(particles, jet_def);
    std::vector<fastjet::PseudoJet> jets = cs.inclusive_jets(ptmin);

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

      // quark/gluon jet tagging
      auto pid = p.user_info<MyInfo>().pdg_id();
      if(pid == 21){
        gjets.push_back(j_matched);
        float m = j_matched.m();
        if(m>5)
          h1_gm->Fill(m);
      }
      else{
        qjets.push_back(j_matched);
        float m = j_matched.m();
        if(m>5)
          h1_qm->Fill(m);
      }
    } // loop over initiators
  } // loop over events

  TCanvas* c = new TCanvas("c", "c", 800, 600);
  gStyle->SetOptStat(0);
  c->cd();
  h1_n->Draw();
  c->SaveAs("n.pdf");
  h1_dphi->Draw();
  c->SaveAs("dphi.pdf");
  h1_qm->Draw();
  c->SaveAs("quarkjet_m.pdf");
  h1_gm->Draw();
  c->SaveAs("gluonjet_m.pdf");

  h1_qm->Scale(1./h1_qm->Integral());
  h1_gm->Scale(1./h1_gm->Integral());
  h1_qm->GetYaxis()->SetTitle("Prob.");
  h1_gm->GetYaxis()->SetTitle("Prob.");

  c->cd();
  h1_qm->SetMarkerStyle(20);
  h1_qm->SetMarkerColor(2);

  h1_gm->SetMarkerStyle(21);
  h1_gm->SetMarkerColor(4);

  h1_qm->Draw("p");
  h1_gm->Draw("psame");
  TLegend* lg = new TLegend(0.7, 0.7, 0.9, 0.9);
  lg->AddEntry(h1_qm, "quark jets", "lep");
  lg->AddEntry(h1_gm, "Gluon jets", "lep");
  lg->Draw("same");
  c->SaveAs("m.pdf");

  return 0;
}
