#include "SimConfiguration.hh"
#include <string>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

//default values
const CalorimeterConfiguration SimConfiguration::defaultCalo = { 
  6, //nrows
  9, //ncolumns
  25, //width
  25, //height
  140 }; //length
const GeneratorConfiguration SimConfiguration::defaultGen = { 
  "e+", //particle type
  3000, //energy 				     
  0,    //impact x
  0,    //impact y
  0,    //angle x
  0 }; //angle y

SimConfiguration::SimConfiguration():
  calo(defaultCalo),
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
  boost::property_tree::ptree fullTree;
  read_json(conf, fullTree);
  
  //read calo tree
  auto calTreeOpt = fullTree.get_child_optional("calorimeter");
  if(calTreeOpt){
    auto calTree = *calTreeOpt;

    auto optNRows = calTree.get_optional<int>("nRows");
    updateFromConfig(optNRows, calo.nRows);
    auto optNCols = calTree.get_optional<int>("nColumns");
    updateFromConfig(optNCols, calo.nCols);
    auto optWidth = calTree.get_optional<double>("width");
    updateFromConfig(optWidth, calo.width);
    auto optHeight = calTree.get_optional<double>("height");
    updateFromConfig(optHeight, calo.height);
    auto optLength = calTree.get_optional<double>("length");
    updateFromConfig(optLength, calo.length);
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
      
      auto optType = thisGenTree.get_optional<std::string>("particleType");
      updateFromConfig(optType, thisPrimary.particleType);
      auto optEnergy = thisGenTree.get_optional<double>("energy");
      updateFromConfig(optEnergy, thisPrimary.energy);
      auto optAngleX = thisGenTree.get_optional<double>("angleX");
      updateFromConfig(optAngleX, thisPrimary.angleX);
      auto optAngleY = thisGenTree.get_optional<double>("angleY");
      updateFromConfig(optAngleY, thisPrimary.angleY);

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
}
  
  
