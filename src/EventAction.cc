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
    G4cout << "Processed Event " << evt->GetEventID() << G4endl;
  }
}
