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
#include "G4SubtractionSolid.hh"
#include "G4Element.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4String.hh"
#include "G4GlobalMagFieldMessenger.hh"


#include <cstdlib>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DetectorConstruction::DetectorConstruction(std::shared_ptr<SimConfiguration> simConf)
  : G4VUserDetectorConstruction(),
    simConf_(simConf)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  double crystalLength = simConf_->calo.length;
  double crystalWidth = simConf_->calo.width;
  double crystalHeight = simConf_->calo.height;
  int nRows = simConf_->calo.nRows;
  int nCols = simConf_->calo.nCols;

  // build pbf2 material
  G4NistManager* nist = G4NistManager::Instance();
  G4Element* Pb = nist->FindOrBuildElement("Pb"); 
  G4Element* F = nist->FindOrBuildElement("F");
  if (!mPbF2_) {
    mPbF2_.reset(new G4Material("PbF2", 7.77 * g / cm3, 2));
  }
  G4int nPb = 1;
  G4int nF = 2;
  mPbF2_->AddElement(Pb, nPb);
  mPbF2_->AddElement(F, nF);
   
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double worldHeight = 2 * nRows * crystalHeight;
  G4double worldWidth = 2 * nCols * crystalWidth;
  G4double worldLength = 2 * crystalLength;
  G4Material* worldMat = nist->FindOrBuildMaterial("G4_Galactic");
  
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
            

  G4VisAttributes* worldAttr = new G4VisAttributes();
  worldAttr->SetVisibility(false);
  logicWorld->SetVisAttributes(worldAttr);

  //build lateral leakage catcher
  
  G4ThreeVector globalCaloCenter( (nCols % 2 - 1) * crystalWidth/2.0,
            -1*(nRows % 2 - 1) * crystalHeight/2.0,
            0);

  G4Box* caloPlusLateral = new G4Box("caloPlusLateral", 
             0.5*crystalWidth*nCols + 0.5,
             0.5*crystalHeight*nRows + 0.5,
             0.5*crystalLength);
  G4Box* fullCaloSize = new G4Box("fullCalo",
          0.5*crystalWidth*nCols, 
          0.5*crystalHeight*nRows,
          0.5*crystalLength);
  G4SubtractionSolid* lateralLeakageSolid = new G4SubtractionSolid("lateral",
                   caloPlusLateral,
                   fullCaloSize);
    
  G4LogicalVolume* logicLateralLeakage =
    new G4LogicalVolume(lateralLeakageSolid,
      worldMat,
      "lateral");
  new G4PVPlacement(0,
        globalCaloCenter,
        logicLateralLeakage,
        "lateral",
        logicWorld,
        false,
        0,
        checkOverlaps);
  G4VisAttributes* latAttr = new G4VisAttributes(G4Colour::Magenta());
  latAttr->SetForceWireframe(true);
  latAttr->SetVisibility(true);
  logicLateralLeakage->SetVisAttributes(latAttr);
  
  //build albedo and longitudunal leakage catchers
  G4Box* albedoLongSolid = new G4Box("albedoLong",
         0.5*crystalWidth*nCols + 0.5,
         0.5*crystalHeight*nRows + 0.5,
         0.5);
  G4LogicalVolume* logicAlbedoLong =
    new G4LogicalVolume(albedoLongSolid,
      worldMat,
      "albedoLong");
  new G4PVPlacement(0,
        globalCaloCenter - G4ThreeVector(0, 0, 0.5*crystalLength + 0.5),
        logicAlbedoLong,
        "albedo",
        logicWorld,
        false,
        0, 
        checkOverlaps);
  new G4PVPlacement(0,
        globalCaloCenter + G4ThreeVector(0, 0, 0.5*crystalLength + 0.5),
        logicAlbedoLong,
        "longitudinal",
        logicWorld,
        false,
        0, 
        checkOverlaps);

  G4VisAttributes* albLongAtr = new G4VisAttributes(G4Colour::Green());
  albLongAtr->SetForceWireframe(true);
  albLongAtr->SetVisibility(true);
  logicAlbedoLong->SetVisAttributes(albLongAtr);
     
  //build the crystals
  G4Box* solidCrystal =
    new G4Box("solidCrystal",
        0.5*crystalWidth, 0.5*crystalHeight, 0.5*crystalLength);

  G4LogicalVolume* logicalCrystal = new G4LogicalVolume(solidCrystal,
              mPbF2_.get(),
              "logicalCrystal");
  
  for(int row = 0; row < nRows; ++row){
    for(int col = 0; col < nCols; ++col){
      double xPos = (-nCols/2 + col) * crystalWidth;
      double yPos = (nRows/2 - row) * crystalHeight;

      new G4PVPlacement(0,
      G4ThreeVector(xPos, yPos, 0),
      logicalCrystal,
      "crystal",
      logicWorld,
      false,
      nCols*row + col,
      checkOverlaps);
   }
  }

  G4VisAttributes* crystalAttr = new G4VisAttributes(G4Colour::Black());
  crystalAttr->SetForceWireframe(true);
  crystalAttr->SetVisibility(true);
  logicalCrystal->SetVisAttributes(crystalAttr);

  //turn on a field
  if(simConf_->calo.field){
    G4ThreeVector field(0.0, 1.0 * tesla, 0.0);
    G4GlobalMagFieldMessenger* fMes = new G4GlobalMagFieldMessenger(field);
    fMes->SetVerboseLevel(1);
  }
  
  //build the wall
  if(simConf_->wall.on){
    G4Box* solidWall =
      new G4Box("solidWall",
    0.5*worldWidth, 0.5*worldHeight, 0.5*simConf_->wall.thickness);
    
    G4Material* Al = nist->FindOrBuildMaterial("G4_Al");
    G4LogicalVolume* logicalWall = new G4LogicalVolume(solidWall,
                  Al,
                  "logicalWall");
    new G4PVPlacement(0,
          G4ThreeVector(0, 0, -1*(0.5*crystalLength + simConf_->wall.distance)),
          logicalWall,
          "wall",
          logicWorld,
          false,
          0,
          checkOverlaps);

    G4VisAttributes* wallAttr = new G4VisAttributes(G4Colour::Green());
    wallAttr->SetForceWireframe(true);
    wallAttr->SetVisibility(true);
    logicalWall->SetVisAttributes(wallAttr);
  }
          
  
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
