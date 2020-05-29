////////////////////////////////////////////////////////////////////////////////
//
//   脉冲刻度时, 我们使用了 "拨档法 (Switch)" 与 "旋钮法 (Height)"
//   从拟合数据看，两者的差别很小，< 1%
//   现在来定量考察一下两种方法的拟合参数的差异, 来决定后续的数据处理采取哪种刻度方式
//
//   by gfh, 2020-05-29
////////////////////////////////////////////////////////////////////////////////

Int_t SSDNum = 4;
Int_t CHNum  = 16;
Int_t ParsNum = 3;



//______________________________________________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum);
void DeleteData(Double_t*** p, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum);


//___________________________________________________
void PulserCali_EstimateErrorOfHightAndSwitch()
{
  std::string LayerTag("L1S");
  std::string GainTag("Gain20");
  std::string CaliTag("PulserReCali");  // "PulserCali" or "PulserReCali"
  std::string HeightTag("Height");
  std::string SwitchTag("Switch");
  std::string FileOutTag("ErrorOfHightAndSwitch");
/*
  std::string pathPNGOutput(Form("figures/SSD_%s_PulserCali_%s.png",  LayerTag.c_str(),FileOutTag.c_str()));
  std::string pathHeightFileIn(Form("output/SSD_%s_PulserCali_%s.dat",LayerTag.c_str(),HeightTag.c_str()));
  std::string pathSwitchFileIn(Form("output/SSD_%s_PulserCali_%s.dat",LayerTag.c_str(),SwitchTag.c_str()));
*/

  std::string pathPNGOutput(Form("figures/SSD_%s_%s_%s_%s.png",  LayerTag.c_str(),CaliTag.c_str(),GainTag.c_str(),FileOutTag.c_str()));
  std::string pathHeightFileIn(Form("output/SSD_%s_%s_%s_%s.dat",LayerTag.c_str(),CaliTag.c_str(),GainTag.c_str(),HeightTag.c_str()));
  std::string pathSwitchFileIn(Form("output/SSD_%s_%s_%s_%s.dat",LayerTag.c_str(),CaliTag.c_str(),GainTag.c_str(),SwitchTag.c_str()));

  Int_t numpar_HeightIn = 3;
  Int_t numpar_SwitchIn = 3;

  Double_t*** HeightIn = ReadData(pathHeightFileIn.c_str(), SSDNum, CHNum, numpar_HeightIn);
  Double_t*** SwitchIn = ReadData(pathSwitchFileIn.c_str(), SSDNum, CHNum, numpar_SwitchIn);

  Double_t SSDCHNum[SSDNum][CHNum];
  Double_t a_ErrofHightOverSwitch[SSDNum][CHNum];
  Double_t b_ErrofHightOverSwitch[SSDNum][CHNum];

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      SSDCHNum[i][j]=j+1;

      a_ErrofHightOverSwitch[i][j]=100*abs((HeightIn[i][j][0]-SwitchIn[i][j][0])/SwitchIn[i][j][0]); // 结果是百分比
      b_ErrofHightOverSwitch[i][j]=100*abs((HeightIn[i][j][2]-SwitchIn[i][j][2])/SwitchIn[i][j][2]); // 结果是百分比

      cout.setf(ios::fixed);
      cout<<setw(5)<<std::right<<i<<setw(5)<<std::right<<j<<setw(15)<<std::right
          <<"a_err = "<<setw(10)<<std::right<<setprecision(11)<<a_ErrofHightOverSwitch[i][j]<<setw(15)<<std::right
          <<"b_err = "<<setw(10)<<std::right<<setprecision(11)<<b_ErrofHightOverSwitch[i][j]<<endl;
    }
  }

  TCanvas* cans = new TCanvas("cansa","Error of Hight and Switch",1200,1000);
  cans->Divide(2,2);
  for (Int_t i=0; i<2; i++)
  {
    cans->cd(i+1);
    gPad->SetGridx();
    gPad->SetLeftMargin(0.13);

    TGraph* a_Error_grap = new TGraph(CHNum,SSDCHNum[i],a_ErrofHightOverSwitch[i]);
    TGraph* b_Error_grap_scale = new TGraph(CHNum,SSDCHNum[i],b_ErrofHightOverSwitch[i]);

    auto hista = a_Error_grap->GetHistogram();
    //a_Error_grap->SetTitle(Form("SSD%d_%s_PulserCali_%s",i+1,LayerTag.c_str(),FileOutTag.c_str()));
    a_Error_grap->SetTitle(Form("SSD%d_%s_%s_%s",i+1,LayerTag.c_str(),CaliTag.c_str(),FileOutTag.c_str()));
    a_Error_grap->GetYaxis()->SetRangeUser(0.5*hista->GetMinimum(),1.2*hista->GetMaximum());
    hista->Draw();
    gPad->Update();

    auto histb = b_Error_grap_scale->GetHistogram();
    Double_t rightmax = histb->GetMaximum();
    Double_t scale = gPad->GetUymax()/rightmax;

    for (Int_t j=0; j<CHNum; j++)
    {
      b_ErrofHightOverSwitch[i][j] = b_ErrofHightOverSwitch[i][j]*scale;
    }
    TGraph* b_Error_grap = new TGraph(CHNum,SSDCHNum[i],b_ErrofHightOverSwitch[i]);

    a_Error_grap->SetMarkerStyle(20);
    a_Error_grap->SetMarkerColor(kBlue);
    a_Error_grap->GetXaxis()->SetTitle("Channel");
    a_Error_grap->GetXaxis()->CenterTitle(1);
    a_Error_grap->GetXaxis()->SetTitleSize(0.04);
    a_Error_grap->GetYaxis()->SetTitle("Relative Errors (%)");
    a_Error_grap->GetYaxis()->CenterTitle(1);
    a_Error_grap->GetYaxis()->SetTitleSize(0.04);

    b_Error_grap->SetMarkerStyle(20);
    b_Error_grap->SetMarkerColor(kRed);
    b_Error_grap->GetXaxis()->SetTitle("Channel");
    b_Error_grap->GetXaxis()->CenterTitle(1);
    b_Error_grap->GetXaxis()->SetTitleSize(0.04);
    b_Error_grap->GetYaxis()->SetTitle("Relative Errors (%)");
    b_Error_grap->GetYaxis()->CenterTitle(1);
    b_Error_grap->GetYaxis()->SetTitleSize(0.04);

    TMultiGraph* mg = new TMultiGraph();
    mg->Add(a_Error_grap);
    mg->Add(b_Error_grap);

    TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),
                              gPad->GetUxmax(),gPad->GetUymax(),
                              0,rightmax,510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);

    TLegend* legend = new TLegend(0.70,0.80,0.9,0.9);
    legend->AddEntry(a_Error_grap,"a_Err(Slope)","lp");
    legend->AddEntry(b_Error_grap,"b_Err(Intercept)","lp");

    mg->Draw("PL");
    axis->Draw();
    legend->Draw("SAME");

  }
  cans->Print(pathPNGOutput.c_str());

}


//_______________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum)
{
///////////////////////////////////////////////////////
//     构建一个函数，返回类型是 三维数组(三级指针)          //
//     注意多级指针的使用！！！                          //
///////////////////////////////////////////////////////
   Double_t*** readpar = NULL;
   readpar = new Double_t** [SSDNum];
   for(Int_t i=0; i<SSDNum; i++)
   {
     readpar[i] = new Double_t* [CHNum];
     for(Int_t j=0; j<CHNum; j++)
     {
       readpar[i][j]= new Double_t[ParNum];
     }
   }
   ifstream in;
   in.open(datapath);
   if(!in.is_open())
   {
     printf("Error: file %s not found\n",datapath);
     return NULL;
   }
   while(in.good())
   {
     // 按行读取数据
     std::string LineRead;
     std::getline(in, LineRead);
     LineRead.assign(LineRead.substr(0, LineRead.find('*')));
     if(LineRead.empty()) continue;
     if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
     std::istringstream LineStream(LineRead);

     Int_t ssdnum;
     Int_t chnum;

     LineStream >> ssdnum >> chnum;
     for(Int_t i=0; i<ParNum; i++)
     {
       LineStream >>readpar[ssdnum][chnum][i];
     }
   }
   in.close();
   return readpar;
}

//______________________________________________________________________________
void DeleteData(Double_t*** p, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum)
{
/////////////////////////////////////////////////////////////
//    释放前面用 new方法给 Double_t*** p 动态分配的内存         //
//    多级指针需要逐层释放内存！！！                            //
/////////////////////////////////////////////////////////////
  for(Int_t i=0; i<SSDNum; i++)
  {
    for(Int_t j=0; j<CHNum; j++)
    {
      delete [] p[i][j];
    }
  }
  for(Int_t i=0; i<SSDNum; i++)
  {
    delete [] p[i];
  }
  delete [] p;
}
