#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <vector>

typedef struct{
  int nRows;
  int nCols;
  double width;
  double height;
  double length;
  bool field;
} CalorimeterConfiguration;

typedef struct{
  bool on;
  double distance;
  double thickness;
} WallConfiguration;

typedef struct{  
  bool randomize;
  std::string particleType;
  double energy;
  double posX;
  double posY;
  double impactAngle;
  double impactPhi;
} GeneratorConfiguration;

typedef struct{
  bool emExtra;
} PhysicsConfiguration;

class SimConfiguration{
public:
  SimConfiguration();
  SimConfiguration(std::string);
  
  void update();
  
  CalorimeterConfiguration calo;
  WallConfiguration wall;
  PhysicsConfiguration phys;
  std::vector<GeneratorConfiguration> genVector;

  static const CalorimeterConfiguration defaultCalo;
  
  static const GeneratorConfiguration defaultGen;

  static const PhysicsConfiguration defaultPhys;

  static const WallConfiguration defaultWall;

private:
  void readConfig();
  std::string confFile_;		  
};
  

#endif
