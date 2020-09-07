#include "CalibratedEvent.h"

//_______________________________
CalibratedEvent::CalibratedEvent() :
fMulti(16)
{
  fStripb   =  new Int_t      [fMulti];
  fStripf   =  new Int_t      [fMulti];
  fESib     =  new Double_t   [fMulti];
  fESif     =  new Double_t   [fMulti];
};


CalibratedEvent::~CalibratedEvent()
{
  delete [] fStripb;
  delete [] fStripf;
  delete [] fESib;
  delete [] fESif;
}
