#include "include/ReadFileModule.h"

void test()
{
  std::string pathfile("Makefile");

  ReadFileModule data;
  if (!data.IsFileExists(pathfile.c_str()))
  {
	  cout<<Form("File %s exists!", pathfile.c_str());
  }
}
