//run action implementation

#include "RunAction.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include <time.h>
#include <cmath>


#include <string>


RunAction::RunAction(std::shared_ptr<SimConfiguration> simConf) :
  G4UserRunAction(),
  theRun_(nullptr),
  simConf_(simConf)
{
}

RunAction::~RunAction(){
}

G4Run* RunAction::GenerateRun(){
  theRun_ = new Run(simConf_);
  return theRun_;
}

void RunAction::BeginOfRunAction(const G4Run*){
  simConf_->update();
  theRun_->initializeTreeAndHist();
}

void RunAction::EndOfRunAction(const G4Run*){
  theRun_->closeFile();
}

Run::Run(std::shared_ptr<SimConfiguration> simConf):
  t_(nullptr),
  crystalHist_(nullptr),
  radialHist_(nullptr),
  radialLongitudinalHist_(nullptr),
  particleProcessHist_(nullptr),
  file_(nullptr),
  simConf_(simConf)
{
}

Run::~Run(){
}

void Run::initializeTreeAndHist(){
  int nRows = simConf_->calo.nRows;
  int nCols = simConf_->calo.nCols;
  
  int nEvents = G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed();

  time_t rawtime;
  struct tm* timeinfo;
  char buffer[80];
  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime(buffer,80,"%F_%H_%M_%S",timeinfo);
  
  std::string fileName = "out_" + std::to_string(nEvents) +"_events_" + std::string(buffer) + ".root";

  file_ = new TFile(fileName.c_str(), "recreate");
  t_ = new TTree("t","t");
  crystalHist_ = new TH2D("crystal", "crystal", nCols, 0, nCols, nRows, 0, nRows);
  
  radialHist_ = new TH2D("xyhist", "xyhist", 250, -112.5, 112.5, 100, -(75 - 12.5), 75+12.5);
  radialLongitudinalHist_ = new TH2D("radLongHist", "radLongHist", 250, 0, 140, 50, 0, 50);
  particleProcessHist_ = new TH2D("particleProcessHist", "particleProcessHist", 100, 0.0, 0.0,
				  100, 0.0, 0.0);

  zeroEDeps();

  t_->Branch("crystalDeps", crystalDeps_, "crystalDeps[54]/D");
  t_->Branch("totalDep", &totalDep_, "totalDep/D");
  t_->Branch("lateralLeakage", &lateralLeakage_, "lateralLeakage/D");
  t_->Branch("albedo", &albedo_, "albedo/D");
  t_->Branch("longitudinalLeakage", &longitudinalLeakage_, "longitudinalLeakage/D");
  t_->Branch("nPositrons", &nPositrons_, "nPositrons/I");
  t_->Branch("positronPositions", positions_, "positronPositions[4]/D");
  t_->Branch("positronEnergies", energies_, "positronEnergies[2]/D");
}

void Run::recordEndOfEvent(){
  t_->Fill();

  int nRows = simConf_->calo.nRows;
  int nCols = simConf_->calo.nCols;
  for(int row = 0; row < nRows; ++row){
    for(int col = 0; col < nCols; ++col){
      crystalHist_->Fill(col + 0.5, nRows - 0.5 - row, crystalDeps_[row*nCols + col]);
    }
  }
  zeroEDeps();
}

void Run::fillHists(const G4ThreeVector& pos, G4double eDep){
  radialHist_->Fill(pos.x(), pos.y(), eDep);
  radialLongitudinalHist_->Fill(pos.z() + simConf_->calo.length/2.0, std::sqrt(pos.x()*pos.x() + pos.y()*pos.y()), eDep);
}

void Run::recordParticleProcess(std::string particle, std::string process){
  particleProcessHist_->Fill(particle.c_str(), process.c_str(), 1);
}

void Run::closeFile(){
  t_->Write();
  crystalHist_->Scale(1.0/crystalHist_->GetMaximum());
  crystalHist_->Write();
  file_->Write();
  file_->Close();
}

void Run::zeroEDeps(){
  for(int i = 0; i < 54; crystalDeps_[i++] = 0); 
  totalDep_ = 0;
  lateralLeakage_ = 0;  
  albedo_ = 0;
  longitudinalLeakage_ = 0;
  nPositrons_ = 0;
  for(int i = 0; i < 4; positions_[i++] = 0);
  for(int i = 0; i < 2; energies_[i++] = 0);
}

void Run::accountEdep(G4int crystalId, G4double eDep){
  crystalDeps_[crystalId] += eDep; 
  totalDep_ += eDep;
}

void Run::registerPositron(G4double impactX, G4double impactY, G4double energy){
  ++nPositrons_;
  if(energies_[0] == 0){
    positions_[0] = impactX;
    positions_[1] = impactY;
    energies_[0] = energy;
  }
  else{
    positions_[2] = impactX;
    positions_[3] = impactY;
    energies_[1] = energy;
  }
}
