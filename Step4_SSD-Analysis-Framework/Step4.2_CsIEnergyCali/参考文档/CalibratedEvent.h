#ifndef CALIBRATEDEVENT_H
#define CALIBRATEDEVENT_H

//_______________________________
class CalibratedEvent
{
public:
  Int_t      fMulti;
  Int_t      *fStripb;  // [fMulti]
  Int_t      *fStripf;  // [fMulti]
  Double_t   *fESib;    // [fMulti]
  Double_t   *fESif;    // [fMulti]

  CalibratedEvent();
  virtual ~CalibratedEvent();

  ClassDef(CalibratedEvent,1);
};

#endif
