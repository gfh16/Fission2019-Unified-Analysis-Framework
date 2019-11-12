////////////////////////////////////////////////////////////////////////////////
//          MERGE ALPHA CALIBRATION DATA FOR HIGHER STATISTICS                //
//                                                                            //
//     Use TChain to merge alpha calibtation data                             //
//                                                                            //
//     Author gfh                                                             //
//     Date Nov 11, 2019                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
const char L1  = "L1";
const char L2 = "L2";

//______________________________________________________________________________
void AlphaCali_MergeFile()
{
  const int FirstRun = 0;
  const int LastRun  = 5;
  std::string LayerTag("L1");
  std::string FileTag("../../../Fission2019_Data/MapRoot/MapSS");
  TFile *FileOut = new TFile(Form("data/SSD"));
  auto * myData = new TChain("KrPb");

  for(int i=FirstRun; i<LastRun; i++)
  {
    myData->Add(Form("../../../Fission2019_Data/MapRoot/%s_%s_AlphaCali%04d.root",FileTag.c_str(),i));
  //  myData->Add(Form("%s_L2_AlphaCali%04d.root",FileTag.c_str(),i));
  }

  // I disable all the branches
  myData->SetBranchStatus("*",false);
  if(strcmp(L1,LayerTag.c_str())==0)
  {
    myData->SetBranchStatus("SSD1_L1S_E",true);
    myData->SetBranchStatus("SSD2_L1S_E",true);
    myData->SetBranchStatus("SSD3_L1S_E",true);
    myData->SetBranchStatus("SSD4_L1S_E",true);
  }
  if(strcmp(L2,LayerTag.c_str())==0)
  {
    myData->SetBranchStatus("SSD1_L2F_E",true);
    myData->SetBranchStatus("SSD1_L2B_E",true);
    myData->SetBranchStatus("SSD2_L2F_E",true);
    myData->SetBranchStatus("SSD2_L2B_E",true);
    myData->SetBranchStatus("SSD3_L2F_E",true);
    myData->SetBranchStatus("SSD3_L2B_E",true);
    myData->SetBranchStatus("SSD4_L2F_E",true);
    myData->SetBranchStatus("SSD4_L2B_E",true);
  }


}

//______________________________________________________________________________
void MergeFile(TChain *myData, const char *LayerTag)
{

}

//______________________________________________________________________________
void SetPoints(Int_t event, Int_t x, Int_t y, TObject *selected)
{
/////////////////////////////////////////////////////////////////////////////
//    This function is to set the fit range by hand on GUI                 //
//    Click the Central button of the mouse to get the range               //
//                                                                         //
//    Function parameters:                                                 //
//       event :   number of click points                                  //
//           x :   x value of the point                                    //
//           y :   y value of the point                                    //
//    selected :   a pointer to the curent point                           //
/////////////////////////////////////////////////////////////////////////////
  if(event == 2)
  {
    float px = gPad->AbsPixeltoX(x); // Conversion of absolute pixel to X
    float py = gPad->AbsPixeltoX(y); // CoYversion of absolute pixel to Y
    py = gPad->PadtoY(py);           // Convert y from pad to Y
    float Uymin = gPad->GetUymin();  // Returns the minimum/maximum y-coordinate
    float Uymax = gPad->GetUymax();  // value visible on the pad

    //  save the clicks as a marker
    if(px>=gPad->GetUxmin() && px<=gPad->GetUxmax())
    {
      m[NPoints] = new TMarker(px,py,3);          // marker style = 3, means “*”
      l[NPoints] = new TLine(px,Uymin,px,Uymax);
      l[NPoints] -> SetLineColor(kYellow);
      l[NPoints] -> SetLineWidth(2);
      l[NPoints] -> Draw();
      for(int i=0; i<NPoints; i++)
      {
        l[i] -> Draw();
      }
      gPad->Modified();    // Tell the canvas that an object it is displaying have changed
      gPad->Update();      //  Force the canvas to refresh
      printf("x=%f\n", px);
      NPoints++;

      //  I get 6 points and then I return
      if(NPoints==6)
      {
        Index = 1;   // Index
        return;
      }
      cout<< "Click next point"<< endl;
    } // ==== close  if(px>=gPad->GetUxmin() && px<=gPad->GetUxmax())
  }   // ==== close if(event==2)
  return;
}
