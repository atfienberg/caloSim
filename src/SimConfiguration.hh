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
  double pPhiHat;
} GeneratorConfiguration;

class SimConfiguration{
public:
  SimConfiguration();
  SimConfiguration(std::string);
  
  void update();
  
  CalorimeterConfiguration calo;
  std::vector<GeneratorConfiguration> genVector;

  static const CalorimeterConfiguration defaultCalo;
  
  static const GeneratorConfiguration defaultGen;

private:
  void readConfig();
  std::string confFile_;		  
};
  

#endif
