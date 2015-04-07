//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \brief Implementation of the PhysicsList class
//
// $Id: PhysicsList.cc 82293 2014-06-13 15:23:19Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"

#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"

#include "G4LossTableManager.hh"
#include "G4ProcessManager.hh"

#include "G4String.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList(std::shared_ptr<SimConfiguration> simConf) 
  : 
  G4VModularPhysicsList(),
  simConf_(simConf)
{
  SetVerboseLevel(1);

  // EM physics
  emPhysicsList.reset(new G4EmStandardPhysics_option3());
  decayPhysicsList.reset(new G4DecayPhysics());
  emExtraPhysicsList.reset(new G4EmExtraPhysics());

  
  G4EmExtraPhysics* extraP = static_cast<G4EmExtraPhysics*>(emExtraPhysicsList.get());
  G4String on = "on";
  G4String off = "off";

  extraP->Synch(off);
  extraP->GammaNuclear(on);
  extraP->MuonNuclear(on);
  
  G4LossTableManager::Instance();
  SetDefaultCutValue(1*mm);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructParticle()
{
  emPhysicsList->ConstructParticle();
  decayPhysicsList->ConstructParticle();

  if(simConf_->phys.emExtra){
    emExtraPhysicsList->ConstructParticle();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
  // transportation
  //
  AddTransportation();
  
  // electromagnetic physics list
  //
  emPhysicsList->ConstructProcess();
  
  // decay process
  //
  decayPhysicsList->ConstructProcess();
 
  
  if(simConf_->phys.emExtra){
    emExtraPhysicsList->ConstructProcess();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
