#include "SteppingAction.hh"
#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "RunAction.hh"
#include "G4String.hh"



SteppingAction::SteppingAction():
  G4UserSteppingAction()
{
}

SteppingAction::~SteppingAction(){
}

void SteppingAction::UserSteppingAction(const G4Step* s){
  G4VPhysicalVolume* pv = s->GetPreStepPoint()->GetPhysicalVolume();
  if(!pv){
    return;
  }
  double eDep = s->GetTotalEnergyDeposit();

  Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  if(pv->GetName() == "crystal"){
    G4ThreeVector pos1 = s->GetPreStepPoint()->GetPosition();
    G4ThreeVector pos2 = s->GetPostStepPoint()->GetPosition();
    G4ThreeVector pos = (pos1 + pos2)/2.0; 
    
    run->accountEdep(pv->GetCopyNo(), eDep);
    run->fillHists(pos, eDep);

    G4VPhysicalVolume* pvPost = s->GetPostStepPoint()->GetPhysicalVolume();
    G4String pvPostName = pvPost->GetName();
    if(!pvPost){
      return;
    }
    if(pvPostName != "crystal"){
      G4Track* t = s->GetTrack();
      if(pvPostName == "lateral"){
	run->accountLateral(t->GetKineticEnergy());
	t->SetTrackStatus(fStopAndKill);
      }
      else if(pvPostName == "albedo"){
	run->accountAlbedo(t->GetKineticEnergy());
	t->SetTrackStatus(fStopAndKill);
      }
      else if(pvPostName == "longitudinal"){
	run->accountLongitudinal(t->GetKineticEnergy());
	t->SetTrackStatus(fStopAndKill);
      }
    }
  }
}
