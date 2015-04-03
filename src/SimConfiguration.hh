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
} CalorimeterConfiguration;

typedef struct{
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
  std::vector<GeneratorConfiguration> genVector;
  PhysicsConfiguration phys;

  static const CalorimeterConfiguration defaultCalo;
  
  static const GeneratorConfiguration defaultGen;

  static const PhysicsConfiguration defaultPhys;

private:
  void readConfig();
  std::string confFile_;		  
};
  

#endif
