
Bool_t compare(Int_t a, Int_t b);

//_________________
void test11()
{
  Int_t a = 10;
  Int_t b = 5;

  if (compare(a, b)) {
      cout<<"a > b !"<<endl;
  } else {
      cout<<"a < b !"<<endl;
  }

  return 0;
}

//______________________________
Bool_t compare(Int_t a, Int_t b)
{
  if (a > b) {
      return true;
  } else {
      return false; 
  }
}
