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
/// \file electromagnetic/TestEm18/TestEm18.cc
/// \brief Main program of the electromagnetic/TestEm18 example
//
//
// $Id: TestEm18.cc 66241 2012-12-13 18:34:42Z gunter $
//

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"
#include "SteppingVerbose.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "SimConfiguration.hh"

#include <iostream>
#include <ctime>
#include <memory>

using namespace std;

int main(int argc,char** argv) {

  //choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  long seeds[2];
  seeds[0] = (long) time(NULL);
  seeds[1] = (long) (seeds[0] * G4UniformRand());
  CLHEP::HepRandom::setTheSeeds(seeds);

  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);

  // Construct the default run manager
  unique_ptr<G4RunManager> runManager(new G4RunManager);
  
  auto simConf = std::make_shared<SimConfiguration>("simConfiguration.json");

  //initialization classes
  runManager->SetUserInitialization(new DetectorConstruction(simConf));
  runManager->SetUserInitialization(new PhysicsList(simConf));
  runManager->SetUserInitialization(new ActionInitialization(simConf));

   
  // get the pointer to the User Interface manager 
  G4UImanager* UI = G4UImanager::GetUIpointer();  

  if (argc!=1)   // batch mode  
    {
     G4String command = "/control/execute ";
     G4String fileName = argv[1];
     UI->ApplyCommand(command+fileName);
    }
    
  else           //define visualization and UI terminal for interactive mode
    {       
      unique_ptr<G4VisManager> visManager(new G4VisExecutive);
      visManager->Initialize();
      cout << "vis initializing" << endl;
      
      
      unique_ptr<G4UIExecutive> ui(new G4UIExecutive(argc,argv));      
      ui->SessionStart();
    }   

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

