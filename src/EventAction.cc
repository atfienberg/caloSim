#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "RunAction.hh"

EventAction::EventAction():
  G4UserEventAction()
{
}

EventAction::~EventAction(){
}

void EventAction::EndOfEventAction(const G4Event* evt){
  Run* run =  
    static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  run->recordEndOfEvent();
  
  if( evt->GetEventID() % 100 == 0){
    std::cout << "Processed Event " << evt->GetEventID() << std::endl;
  }
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
      double eDep = s->GetTotalEnergyDeposit();
      G4ThreeVector pos1 = s->GetPreStepPoint()->GetPosition();
      G4ThreeVector pos2 = s->GetPostStepPoint()->GetPosition();
      G4ThreeVector pos = (pos1 + pos2)/2.0; 
	
      run->accountEdep(pv->GetCopyNo(), eDep);
      run->fillHists(pos, eDep);
  } 
}
