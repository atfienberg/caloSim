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
// $Id: DetectorConstruction.cc 75117 2013-10-28 09:38:37Z gcosmo $
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Element.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4String.hh"
#include <iostream>
#include <cstdlib>

#define CRYSTALLENGTH 140*mm
#define CRYSTALWIDTH 25*mm
#define CRYSTALHEIGHT 25*mm
#define NROWS 6
#define NCOLS 9

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  // build pbf2 material
  G4NistManager* nist = G4NistManager::Instance();
  G4Element* Pb = nist->FindOrBuildElement("Pb"); 
  G4Element* F = nist->FindOrBuildElement("F");
  G4Material* mPbF2 = new G4Material("PbF2", 7.77*g/cm3, 2);
  G4int nPb = 1;
  G4int nF = 2;
  mPbF2->AddElement(Pb, nPb);
  mPbF2->AddElement(F, nF);
   
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double worldHeight = 2 * NROWS * CRYSTALHEIGHT;
  G4double worldWidth = 2 * NCOLS * CRYSTALWIDTH;
  G4double worldLength = 2 * CRYSTALLENGTH;
  G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*worldWidth, 0.5*worldHeight, 0.5*worldLength);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        worldMat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
            

  //build the crystals
  G4Box* solidCrystal =
    new G4Box("solidCrystal",
	      0.5*CRYSTALWIDTH, 0.5*CRYSTALHEIGHT, 0.5*CRYSTALLENGTH);

  G4LogicalVolume* logicalCrystal = new G4LogicalVolume(solidCrystal,
							mPbF2,
							"logicalCrystal");
  
  for(int row = 0; row < NROWS; ++row){
    for(int col = 0; col < NCOLS; ++col){
      double xPos = (-NCOLS/2 + col) * CRYSTALWIDTH;
      double yPos = (NROWS/2 - row) * CRYSTALHEIGHT;

      new G4PVPlacement(0,
			G4ThreeVector(xPos, yPos, 0),
			logicalCrystal,
			"PhysicalCrystal",
			logicWorld,
			false,
			NCOLS*row + col,
			checkOverlaps);
   }
  }

  
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
