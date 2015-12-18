#include "StackingAction.hh"
#include "G4VProcess.hh"
#include "RunAction.hh"
#include "G4RunManager.hh"
#include <string>



StackingAction::StackingAction() :
  G4UserStackingAction() {}

StackingAction::~StackingAction() {}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track){
  Run* run =  
    static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  std::string particle(track->GetDefinition()->GetParticleName());
  if(track->GetCreatorProcess()){
    std::string process(track->GetCreatorProcess()->GetProcessName());
    run->recordParticleProcess(particle, process);
  }
  else{
    run->recordParticleProcess(particle, "none");
  }

  return fUrgent;
}
