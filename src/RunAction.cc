//run action implementation

#include "RunAction.hh"
#include <iostream>

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
  file_ = new TFile("test.root", "recreate");
  t_ = new TTree("t","t");
  hist_ = new TH2D("hist2d", "hist2d", 9, 0, 9, 6, 0, 6);

  zeroEDeps();

  t_->Branch("crystalDeps", crystalDeps_, "crystalDeps[54]/D");
}

void Run::recordEndOfEvent(){
  t_->Fill();
  for(int row = 0; row < 6; ++row){
    for(int col = 0; col < 9; ++col){
      hist_->Fill(col + 0.5, 5.5 - row, crystalDeps_[row*9 + col]);
    }
  }
  zeroEDeps();
}

void Run::closeFile(){
  t_->Write();
  hist_->Scale(1.0/hist_->GetMaximum());
  hist_->Write();
  file_->Close();
}


