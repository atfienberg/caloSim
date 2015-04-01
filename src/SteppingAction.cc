#include "SteppingAction.hh"
#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "RunAction.hh"
#include "G4String.hh"
#include <iostream>

SteppingAction::SteppingAction():
  G4UserSteppingAction()
{
}

SteppingAction::~SteppingAction(){
}

void SteppingAction::UserSteppingAction(const G4Step* s){
  G4VPhysicalVolume* pv = s->GetPostStepPoint()->GetPhysicalVolume();
  if(!pv){
    return;
  }
  double eDep = s->GetTotalEnergyDeposit();

  Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  G4String pvName = pv->GetName();
  if(pvName == "crystal"){
    G4ThreeVector pos1 = s->GetPreStepPoint()->GetPosition();
    G4ThreeVector pos2 = s->GetPostStepPoint()->GetPosition();
    G4ThreeVector pos = (pos1 + pos2)/2.0; 
    
    run->accountEdep(pv->GetCopyNo(), eDep);
    run->fillHists(pos, eDep);
    return;
  } 
  else if(s->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "crystal"){
    G4Track* t = s->GetTrack();
    if(pvName == "lateral"){
      run->accountLateral(t->GetKineticEnergy());
      t->SetTrackStatus(fStopAndKill);
    }
    else if(pvName == "albedo"){
      run->accountAlbedo(t->GetKineticEnergy());
      t->SetTrackStatus(fStopAndKill);
    }
    else if(pvName == "longitudinal"){
      run->accountLongitudinal(t->GetKineticEnergy());
      t->SetTrackStatus(fStopAndKill);
    }
  }
}
