#include "../include/CSHINEPedestals.h"
#include <iostream>

using namespace std;


int main()
{
  std::string pathfile("Makefile");
  std::string layertag("L1S");
  std::string filetag("Pedestal0000");

  CSHINEPedestals pedestal;
  pedestal.ClickToFindPedestals(layertag.c_str(), filetag.c_str());

}
