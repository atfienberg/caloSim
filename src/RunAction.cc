//run action implementation

#include "RunAction.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>
#include <iostream>
#include <string>


RunAction::RunAction() :
  G4UserRunAction() {
}

RunAction::~RunAction(){
}

G4Run* RunAction::GenerateRun(){
  theRun_ = new Run;
  return theRun_;
}

void RunAction::BeginOfRunAction(const G4Run*){
  theRun_->initializeTreeAndHist();
}

void RunAction::EndOfRunAction(const G4Run*){
  theRun_->closeFile();
}

Run::Run()
{
}

Run::~Run(){
}

void Run::initializeTreeAndHist(){
  int rand = G4UniformRand()*1000000;
  
  int nEvents = G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed();
  
  std::string fileName = "out_" + std::to_string(nEvents) +"_events_" + std::to_string(rand) + ".root";


  file_ = new TFile(fileName.c_str(), "recreate");
  t_ = new TTree("t","t");
  crystalHist_ = new TH2D("crystal", "crystal", 9, 0, 9, 6, 0, 6);
  
  radialHist_ = new TH2D("xyhist", "xyhist", 250, -100, 250, 100, -(75 - 12.5), 75+12.5);
  radialLongitudinalHist_ = new TH2D("radLongHist", "radLongHist", 250, 0, 140, 50, 0, 50);

  zeroEDeps();

  t_->Branch("crystalDeps", crystalDeps_, "crystalDeps[54]/D");
}

void Run::recordEndOfEvent(){
  t_->Fill();
  for(int row = 0; row < 6; ++row){
    for(int col = 0; col < 9; ++col){
      crystalHist_->Fill(col + 0.5, 5.5 - row, crystalDeps_[row*9 + col]);
    }
  }
  zeroEDeps();
}

void Run::fillHists(const G4ThreeVector& pos, G4double eDep){
  radialHist_->Fill(pos.x(), pos.y(), eDep);
  radialLongitudinalHist_->Fill(pos.z()+70*mm, std::sqrt(pos.x()*pos.x() + pos.y()*pos.y()), eDep);
}

void Run::closeFile(){
  t_->Write();
  crystalHist_->Scale(1.0/crystalHist_->GetMaximum());
  crystalHist_->Write();
  file_->Write();
  file_->Close();
}


