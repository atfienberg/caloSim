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
/// \file electromagnetic/TestEm2/include/RunAction.hh
/// \brief Definition of the RunAction class
//
// $Id: RunAction.hh 78550 2014-01-07 09:43:41Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4ThreeVector.hh"
#include "G4Run.hh"
#include "SimConfiguration.hh"
#include "TTree.h"
#include "TFile.h"
#include "TH2.h"
#include <string>
#include <vector>
#include <memory>

class Run : public G4Run{
public:
  Run(std::shared_ptr<SimConfiguration> simConf);
  virtual ~Run();
  
  void initializeTreeAndHist();
  void recordEndOfEvent();
  void fillHists(const G4ThreeVector& pos, G4double eDep);
  void recordParticleProcess(std::string particle, std::string process);
  void closeFile();

  void zeroEDeps();
  
  void accountEdep(G4int crystalId, G4double eDep);
  void accountLateral(G4double eDep) {lateralLeakage_ += eDep;}
  void accountAlbedo(G4double eDep) {albedo_ += eDep;}
  void accountLongitudinal(G4double eDep) {longitudinalLeakage_ += eDep;}
  void registerPositron(G4double impactX, G4double impactY, G4double energy);

private:
  TTree* t_;
  TH2D* crystalHist_;
  TH2D* radialHist_;
  TH2D* radialLongitudinalHist_;
  TH2D* particleProcessHist_;
  TFile* file_;

  G4double crystalDeps_[54];
  G4double totalDep_;
  G4double lateralLeakage_;
  G4double albedo_;
  G4double longitudinalLeakage_;

  //generator info
  G4int nPositrons_;
  G4double positions_[4];
  G4double energies_[4];

  std::shared_ptr<SimConfiguration> simConf_;
};

class RunAction : public G4UserRunAction
{
public:
  
  RunAction(std::shared_ptr<SimConfiguration> simConf);
  virtual ~RunAction();

  virtual G4Run* GenerateRun();  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  Run* theRun_;
  std::shared_ptr<SimConfiguration> simConf_;
};



  
  



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

