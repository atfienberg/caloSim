#include "G4UserStackingAction.hh"
#include "G4Track.hh"

class StackingAction : public G4UserStackingAction
{
public:
  StackingAction();
  virtual ~StackingAction();
  
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
};
    
