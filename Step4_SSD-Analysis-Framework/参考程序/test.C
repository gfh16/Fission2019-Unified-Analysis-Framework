#include "test.h"

//_______________________
Test::Test() :
fMulti(10)
{
  fE1 = new Int_t  [fMulti];
}

//________________________
Test::~Test()
{
  delete []  fE1;
}


//________________________
Int_t* Test::GetE1()
{
  fMulti = 5;
  for (Int_t i=0; i<fMulti; i++)
  {
    fE1[i] = i+10;
    cout<<i<<setw(10)<<fE1[i]<<endl;
  }
  return fE1;
}


//__________________________
Int_t* Test::GetE2()
{
  fMulti = 5;
  for (Int_t j=0; j<fMulti; j++)
  {
    fE1[j] = j;
    cout<<j<<setw(10)<<fE1[j]<<endl;
  }
  return fE1;
}
