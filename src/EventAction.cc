#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "RunAction.hh"

EventAction::EventAction():
  G4UserEventAction()
{
}

EventAction::~EventAction(){
}

void EventAction::EndOfEventAction(const G4Event*){
  Run* run =  
    static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  run->recordEndOfEvent();
}
  

SteppingAction::SteppingAction():
  G4UserSteppingAction()
{
}

SteppingAction::~SteppingAction(){
}

void SteppingAction::UserSteppingAction(const G4Step* s){
  G4VPhysicalVolume* pv = s->GetPostStepPoint()->GetPhysicalVolume();
  if(pv && (pv->GetName() != "World")){
      Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
      run->accountEdep(pv->GetCopyNo(), s->GetTotalEnergyDeposit());
  } 
}
