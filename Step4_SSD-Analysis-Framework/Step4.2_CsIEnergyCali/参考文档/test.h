#ifndef TEST_H
#define TEST_H


class Test
{
public:
  Test();
  virtual  ~Test();

  Int_t* GetE1();
  Int_t* GetE2(); 

private:
  Int_t   fMulti;
  Int_t*  fE1;

};

#endif
