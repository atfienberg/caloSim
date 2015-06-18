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
/// \file electromagnetic/TestEm2/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//
// $Id: PrimaryGeneratorAction.cc 83431 2014-08-21 15:49:56Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction (std::shared_ptr<SimConfiguration> simConf)
:G4VUserPrimaryGeneratorAction(), 
 particleGun_(new G4ParticleGun(1)),
 simConf_(simConf)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  Run* run =  
    static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  double startingZ = -0.75*simConf_->calo.length;

  bool first = true;
  for(const auto& conf : simConf_->genVector){    
    double posX;
    double posY;
    double energy;
    
    if(!conf.randomize){  //no randomization
      posX = conf.posX;
      posY = conf.posY;
      energy = conf.energy;
    }
    
    else if ( (!first) && (G4UniformRand() < 0.5) ){ //if not first particle, randomize whether we include it
      continue;
    }
    
    else{ //if included, randomize positions and energy
      posX = 100*G4UniformRand() - 100;
      posY = 75*G4UniformRand() - 37.5;
      energy = 2800*G4UniformRand() + 200;
    }
    first = false;

    G4ParticleDefinition* particle
      = G4ParticleTable::GetParticleTable()->FindParticle(conf.particleType);
    particleGun_->SetParticleDefinition(particle);
    particleGun_->SetParticleEnergy(energy);  

    G4ThreeVector startingPosition(G4ThreeVector(posX,posY,startingZ));    
    if(conf.impactAngle == 0){
      particleGun_->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
      particleGun_->SetParticlePosition(startingPosition);
    }
    
    else{
      particleGun_->
	SetParticleMomentumDirection(G4ThreeVector(std::sin(conf.impactAngle*deg)*std::cos(conf.impactPhi*deg),
						   std::sin(conf.impactAngle*deg)*std::sin(conf.impactPhi*deg),
						   std::cos(conf.impactAngle*deg)));

      G4ThreeVector startingPositionCorrection(std::tan(conf.impactAngle*deg)*std::cos(conf.impactPhi*deg),
					       std::tan(conf.impactAngle*deg)*std::sin(conf.impactPhi*deg),
					       0);
      
      startingPosition = startingPosition + 
	(startingZ + 0.5*simConf_->calo.length)*startingPositionCorrection;
      particleGun_->SetParticlePosition(startingPosition);
    }
						   
      
    particleGun_->GeneratePrimaryVertex(anEvent);
    run->registerPositron(posX, posY, energy);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

