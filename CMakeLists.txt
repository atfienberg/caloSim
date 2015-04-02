cmake_minimum_required (VERSION 2.6)

project (ToyCaloSimulation)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

execute_process(COMMAND root-config --cflags OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE ROOTCFLAGS)
execute_process(COMMAND root-config --libs OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE ROOT_LIBRARIES)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++11 ${ROOTCFLAGS}")

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*.hh)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${Geant4_INCLUDE_DIR})

add_executable(runSim ${PROJECT_SOURCE_DIR}/runSim.cc ${sources} ${headers})

target_link_libraries (runSim ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})

set(TXTFILES
  vis.mac
  do10k.mac
  do500.mac
  simConfiguration.json
  simConfigurationMultipleParticles.json
  )

foreach(_file ${TXTFILES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
    )
endforeach()
