#include "SimConfiguration.hh"
#include <string>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

//default values
const CalorimeterConfiguration SimConfiguration::defaultCalo = { 
  6,     //nrows
  9,     //ncolumns
  25,    //width
  25,    //height
  140,   //length
  true}; //field 
const GeneratorConfiguration SimConfiguration::defaultGen = { 
  false, //randomize
  "e+", //particle type
  3000, //energy 				     
  0,    //impact x
  0,    //impact y
  0,    //impactAngle
  0 }; //impactPhi
const PhysicsConfiguration SimConfiguration::defaultPhys = { 
  false //em extra physics
};
const WallConfiguration SimConfiguration::defaultWall = {
  false, //on
  12,   //distance
  6.35  //thickness
};

SimConfiguration::SimConfiguration():
  calo(defaultCalo),
  wall(defaultWall),
  phys(defaultPhys),
  genVector(0)
{
  genVector.push_back(defaultGen);
}

template<typename T>
void updateFromConfig(boost::optional<T> opt, T& oldValue){
  if(opt){
    oldValue = *opt;
  }
}

SimConfiguration::SimConfiguration(std::string conf):
  SimConfiguration()
{
  confFile_ = conf;
  readConfig();
}
 
void SimConfiguration::update(){
  if(confFile_.size() != 0){
    readConfig();
  }
}

void SimConfiguration::readConfig(){
  boost::property_tree::ptree fullTree;
  read_json(confFile_, fullTree);
  
  //read calo tree
  auto calTreeOpt = fullTree.get_child_optional("calorimeter");
  if(calTreeOpt){
    auto calTree = *calTreeOpt;

    updateFromConfig(calTree.get_optional<int>("nRows"), calo.nRows);
    
    updateFromConfig(calTree.get_optional<int>("nColumns"), calo.nCols);
    
    updateFromConfig(calTree.get_optional<double>("width"), calo.width);
    
    updateFromConfig(calTree.get_optional<double>("height"), calo.height);
    
    updateFromConfig(calTree.get_optional<double>("length"), calo.length);
    
    updateFromConfig(calTree.get_optional<bool>("field"), calo.field);
  }

  //read generator tree
  auto genTreeOpt = fullTree.get_child_optional("generatorList");
  if(genTreeOpt){
    genVector.resize(0);
    auto genTree = *genTreeOpt;
    
    for(const auto& tree : genTree){
      genVector.push_back(defaultGen);
      auto& thisPrimary = genVector[genVector.size() - 1];
      
      auto thisGenTree = tree.second;
      
      updateFromConfig(thisGenTree.get_optional<bool>("randomize"), thisPrimary.randomize);
      
      updateFromConfig(thisGenTree.get_optional<std::string>("particleType"), 
		       thisPrimary.particleType);
      
      updateFromConfig(thisGenTree.get_optional<double>("energy"), thisPrimary.energy);
      
      updateFromConfig(thisGenTree.get_optional<double>("impactAngle"), thisPrimary.impactAngle);
      
      updateFromConfig(thisGenTree.get_optional<double>("impactPhi"), thisPrimary.impactPhi);

      //impact position needs to be treated specially because it is an array (x, y)
      auto impactPositionOpt = thisGenTree.get_child_optional("impactPosition");
      if(impactPositionOpt){
	auto impactPosition = *impactPositionOpt;
	int component = 0;
	for(auto& componentTree : impactPosition){
	  if(component == 0){
	    thisPrimary.posX = componentTree.second.get<double>("");
	  }
	  else if(component == 1){
	    thisPrimary.posY = componentTree.second.get<double>("");
	  }
	  else{
	    break;
	  }
	  ++component;
	}	
      }
    }
  }

  //read physics tree
  auto physTreeOpt = fullTree.get_child_optional("physics");
  if(physTreeOpt){
    auto physTree = *physTreeOpt;
    
    updateFromConfig(physTree.get_optional<bool>("emExtra"), phys.emExtra);
  }

  //read wall tree
  auto wallTreeOpt = fullTree.get_child_optional("wall");
  if(wallTreeOpt){
    auto wallTree = *wallTreeOpt;
        
    updateFromConfig(wallTree.get_optional<bool>("on"), wall.on);
    
    updateFromConfig(wallTree.get_optional<double>("distance"), wall.distance);
        
    updateFromConfig(wallTree.get_optional<double>("thickness"), wall.thickness);
  }    
}
