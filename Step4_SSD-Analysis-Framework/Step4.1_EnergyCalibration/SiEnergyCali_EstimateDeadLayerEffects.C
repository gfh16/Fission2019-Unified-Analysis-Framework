////////////////////////////////////////////////////////////////////////////////
//
//  1. 能量刻度的准确性主要由三大因素决定:
//            (1) 脉冲刻度的线性 (2)alpha 能量的准确性 (3)alpha 拟合道址的准确性
//  2. 从结果看, 脉冲刻度的线性性很好. 因此，现在来考察alpha能量的准确性对刻度的影响!
//
//  3. alpha 能量的准确性来源于两部分:
//     (1) alpha 标准能量的准确性. 需要查表, 目前采用的是 刘运祚老师的
//        《常用放射性核素衰变纲图（1982）》. 年代较久远, 数据可能不准确, 不过与其他数据对比,
//         差异仅出现在小数点后第3位. 应该比较准确. 这里将不对此因素进行分析！
//
//     (2)即使 alpha 标准能量是准确的, 但需要考虑硅条的死层的影响, 这是影响 alpha 能量的最
//        主要因素. 这里将重点考虑死层对 alpha 能量修正对刻度的影响
//
//  4. 考察思路:
//     (a) 调整死层的厚度, 使用 LiseModule 计算 alpha 的能损, 从而确定 alpha 入射能量;
//     (b) 计算不同 alpha 能量下, peak1 与 peak2 刻度系数的误差;
//     (c) 当误差小于某一设定值时, 认为当前死层厚度为真实的死层厚度
//     (d) 记录当前的死层厚度, 以及 alpha 能损等信息
//
//  5. 在正式分析死层效应之前, 先回过头看看 pulser 刻度的线性性, 即考察每个 pulser 点相对
//     pulser 刻度曲线的偏离情况 (PulserCali_CheckLinearity.C)
//
//  gfh, 2020-06-20
////////////////////////////////////////////////////////////////////////////////

# include "../../include/ReadFileModule.h"
# include "../../include/EnergyLossModule.h"

Int_t SSDNum = 4;
Int_t CHNum  = 16;

//___________________________________
// 进过镀铝 Mylar 膜之后的 alpha 能量
// 数据直接从 SSD_AlphaEnergies.dat 中复制过来
Double_t E1 = 4.90407; // MeV
Double_t E2 = 5.24768; // MeV

Int_t Z_Alpha = 2;
Int_t A_Alpha = 4;
Int_t Deadlayer_num       = 500;
Double_t Sideadlayer_step = 0.01; // um

Double_t RelativeErrCut = 0.005;


//______________________________________________________________________________
void EstimateDeadLayerEffects(const Char_t* layertag);
void GetSiEnergyCaliPars(const Char_t* layertag, Double_t& k, Double_t& h,
  Double_t& c, Int_t ssdindex, Int_t chindex, Double_t E_MeV, Int_t peakindex);


//______________________________________________________________________________
void SiEnergyCali_EstimateDeadLayerEffects()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  EstimateDeadLayerEffects(L1STag.c_str());
  EstimateDeadLayerEffects(L2FTag.c_str());
  EstimateDeadLayerEffects(L2BTag.c_str());
}



//______________________________________________________________________________
void EstimateDeadLayerEffects(const Char_t* layertag)
{
  std::string PulserTag("Height");

  std::string pathPNDOutput(Form("figures/SSD_%s_DeadLayerEffectsOntheParErrs.png",layertag));
  std::string pathFitFuncPDFOutput(Form("figures/SSD_%s_DeadLayerEffectsOnFitFunc.pdf",layertag));
  std::string pathFitFuncPDFOutputBegin(Form("figures/SSD_%s_DeadLayerEffectsOnFitFunc.pdf[",layertag));
  std::string pathFitFuncPDFOutputEnd(Form("figures/SSD_%s_DeadLayerEffectsOnFitFunc.pdf]",layertag));

  std::string pathDeadLayerParsOut(Form("output/SSD_%s_DeadLayerFitPars.dat",layertag));

  ofstream SiEnergyCaliPars(pathDeadLayerParsOut.c_str());
  SiEnergyCaliPars<<"* Consider the deadlayer effects on the fit parameters of peak1 and peak2 \n";
  SiEnergyCaliPars<<"* 1,2 represent using alpha peak1,peak2, respectively. \n";
  SiEnergyCaliPars<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(10)<<"k1"<<setw(15)<<"h1"<<setw(15)<<"k2"<<setw(13)
                  <<"h2"<<setw(25)<<"<deadlayer>(um)"<<setw(18)<<"deadlayer(um)"<<setw(15)
                  <<"E1_ELoss"<<setw(20)<<"E2_ELoss\n";

  Double_t YRangeLow = 0.;
  Double_t YRangeUp  = 45.;
  Double_t YScaleRangeUp_Err         = 20.;
  Double_t YScaleRangeLow_DeadLayer  = 25.;

  Double_t DeadLayer_RangeLow  = 0.;
  Double_t DeadLayer_RangeUp   = 4.;
  Double_t RelativeErr_RangeLow = 0.;
  Double_t RelativeErr_RangeUp  = 1.5*RelativeErrCut*100;
  Double_t RelativeErr_RangeLow_L2F = 0.005*100;
  Double_t RelativeErr_RangeUp_L2F  = 0.015*100;

  Double_t ScaleFactor_Err       = (YScaleRangeUp_Err-YRangeLow)/(RelativeErr_RangeUp-RelativeErr_RangeLow);
  Double_t ScaleFactor_DeadLayer = (YRangeUp-YScaleRangeLow_DeadLayer)/(DeadLayer_RangeUp-DeadLayer_RangeLow);
  Double_t ScaleFactor_Err_L2F   = (YScaleRangeUp_Err-YRangeLow)/(RelativeErr_RangeUp_L2F-RelativeErr_RangeLow_L2F);

  Double_t k1[SSDNum][CHNum];
  Double_t k2[SSDNum][CHNum];
  Double_t h1[SSDNum][CHNum];
  Double_t h2[SSDNum][CHNum];
  Double_t c1[SSDNum][CHNum];
  Double_t c2[SSDNum][CHNum];

  Double_t SSDCHNum[SSDNum][CHNum];

  Double_t k_RelativeErr[SSDNum][CHNum];
  Double_t k_RelativeErr_Sum[SSDNum];
  Double_t k_RelativeErr_Ave[SSDNum];
  Double_t k_RelativeErr_scale[SSDNum][CHNum];
  Double_t k_RelativeErr_Ave_scale[SSDNum];

  Double_t deadlayer[SSDNum][CHNum];
  Double_t deadlayer_Sum[SSDNum];
  Double_t deadlayer_Ave[SSDNum];
  Double_t deadlayer_scale[SSDNum][CHNum];
  Double_t deadlayer_Ave_scale[SSDNum];

  Double_t energyloss1[SSDNum][CHNum];
  Double_t energyloss2[SSDNum][CHNum];

  Double_t E1_ELoss[Deadlayer_num];
  Double_t E2_ELoss[Deadlayer_num];
  Double_t E1_Residual[Deadlayer_num];
  Double_t E2_Residual[Deadlayer_num];

  Double_t RelativeErr[SSDNum][CHNum][Deadlayer_num];


  //_____________________________________
  // 计算不同死层厚度下的能损与剩余能量,
  EnergyLossModule LISEModule;
  for (Int_t i=0; i<Deadlayer_num; i++)
  {
    E1_ELoss[i] = LISEModule.GetEnergyLoss(Z_Alpha,A_Alpha,E1,"Si",i*Sideadlayer_step);
    E2_ELoss[i] = LISEModule.GetEnergyLoss(Z_Alpha,A_Alpha,E2,"Si",i*Sideadlayer_step);

    E1_Residual[i] = E1 - E1_ELoss[i];
    E2_Residual[i] = E2 - E2_ELoss[i];
  }


  //__________________________________________
  // 计算不同能量下, peak1 与 peak2 刻度参数的差异
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      k1[i][j] = 0.; h1[i][j] = 0.;
      k2[i][j] = 0.; h2[i][j] = 0.;
      c1[i][j] = 0.; c2[i][j] = 0.;

      deadlayer[i][j] = 0.;
      energyloss1[i][j] = 0.;
      energyloss2[i][j] = 0.;

      if (((i<2) && strcmp(layertag,"L2F")==0) ||
      ((i==1) && (j==15) && strcmp(layertag,"L2B")==0)) {                  // SSD1_L2F,SSD2_L2F peak2 刻度的斜率
        GetSiEnergyCaliPars(layertag,k1[i][j],h1[i][j],c1[i][j],i,j,E1,1); // 比 peak1 的大;
        GetSiEnergyCaliPars(layertag,k2[i][j],h2[i][j],c2[i][j],i,j,E2,2); // 死层修正无效, 因此不考虑死层的修正
        deadlayer[i][j]   = Sideadlayer_step * 0;                          // SSD2_L2B_CH15 同样无法考虑死层的修正
        cout<<setw(5)<<i<<setw(5)<<j<<setw(5)<<0<<setw(10)<<deadlayer[i][j]<<endl;

      } else {
        for (Int_t k=0; k<Deadlayer_num; k++)
        {
          GetSiEnergyCaliPars(layertag,k1[i][j],h1[i][j],c1[i][j],i,j,E1_Residual[k],1);
          GetSiEnergyCaliPars(layertag,k2[i][j],h2[i][j],c2[i][j],i,j,E2_Residual[k],2);

          RelativeErr[i][j][k]  = abs((c1[i][j]-c2[i][j])/c1[i][j]);

          if (RelativeErr[i][j][k]<RelativeErrCut) {
            deadlayer[i][j]   = Sideadlayer_step * k;
            energyloss1[i][j] = E1_ELoss[k];
            energyloss2[i][j] = E2_ELoss[k];
            break;
          }
        }
      } // close "else"
    }  // close CHNum
  }  //  close SSDNum

  // 计算系数误差与平均deadlayer厚度
  for (Int_t i=0; i<SSDNum; i++)
  {
    deadlayer_Sum[i]     = 0.;
    k_RelativeErr_Sum[i] = 0.;

    for (Int_t j=0; j<CHNum; j++)
    {
      SSDCHNum[i][j] = j+1;
      k_RelativeErr[i][j] = 100*abs((c1[i][j]-c2[i][j])/c1[i][j]); // 以 % 为单位

      k_RelativeErr_Sum[i] += k_RelativeErr[i][j];
      deadlayer_Sum[i]     += deadlayer[i][j];

      // 计算用于 scale 画图所用的数据
      if  ((i<2) && strcmp(layertag,"L2F")==0) {
        k_RelativeErr_scale[i][j]  = (k_RelativeErr[i][j]-RelativeErr_RangeLow_L2F) *ScaleFactor_Err_L2F;

      } else {
        k_RelativeErr_scale[i][j]  = (k_RelativeErr[i][j]-RelativeErr_RangeLow) *ScaleFactor_Err;
      }
      deadlayer_scale[i][j] = (deadlayer[i][j]-DeadLayer_RangeLow)*ScaleFactor_DeadLayer + YScaleRangeLow_DeadLayer;

      cout<<setw(5)<<i<<setw(5)<<j<<setw(15)<<deadlayer[i][j]
          <<setw(15)<<abs((c1[i][j]-c2[i][j])/c1[i][j])
          <<setw(15)<<k1[i][j]<<setw(15)<<k2[i][j]
          <<setw(15)<<h1[i][j]<<setw(15)<<h2[i][j]<<endl;
    }

    // 计算每层硅条刻度参数相对误差与死层厚度的平均值
    if (i==3 && strcmp(layertag,"L1S")==0) { //SSD4_L1S_CH00为空
      k_RelativeErr_Ave[i] = (k_RelativeErr_Sum[i]-k_RelativeErr[i][0])/(CHNum-1);
      deadlayer_Ave[i]     = (deadlayer_Sum[i]-deadlayer[i][0])/(CHNum-1);
    } else {
      k_RelativeErr_Ave[i] = k_RelativeErr_Sum[i]/CHNum;
      deadlayer_Ave[i]     = deadlayer_Sum[i]/CHNum;
    }
    // 计算 scale 后的值
    if ((i<2) && strcmp(layertag,"L2F")==0) { // SSD1_L2F,SSD2_L2F 不考虑死层, 单独画
      k_RelativeErr_Ave_scale[i] = (k_RelativeErr_Ave[i]-RelativeErr_RangeLow_L2F)*ScaleFactor_Err_L2F;
    } else {
      k_RelativeErr_Ave_scale[i] = (k_RelativeErr_Ave[i]-RelativeErr_RangeLow)*ScaleFactor_Err;
    }
    deadlayer_Ave_scale[i] = (deadlayer_Ave[i]-DeadLayer_RangeLow)*ScaleFactor_DeadLayer + YScaleRangeLow_DeadLayer;

    // 输出经死层修正后的刻度参数
    for (Int_t j=0; j<CHNum; j++)
    {
      SiEnergyCaliPars<<setw(5) <<i<<setw(7)<<j<<setw(15)<<k1[i][j]<<setw(15)<<h1[i][j]
                      <<setw(15)<<k2[i][j]<<setw(15)<<h2[i][j]<<setw(15)<<deadlayer_Ave[i]
                      <<setw(18)<<deadlayer[i][j]<<setw(20)<<energyloss1[i][j]
                      <<setw(20)<<energyloss2[i][j]<<endl;
    }
  }


  //_______________________________________________________
  //  画图
  TMultiGraph* mg_scale[SSDNum];
  TGraph* graph_k_Err_scale[SSDNum];
  TGraph* graph_deadlayer_scale[SSDNum];

  TGaxis* k_Err_Yaxis[SSDNum];
  TGaxis* deadlayer_Yaxis[SSDNum];

  TLine*  line_Err_average[SSDNum];
  TLine*  line_deadlayer_average[SSDNum];
  TLine*  line0_Err[SSDNum];
  TLine*  line0_deadlayer[SSDNum];

  TLatex* latex_Err[SSDNum];
  TLatex* latex_deadlayer[SSDNum];

  TCanvas* cans = new TCanvas(Form("cans_%s",layertag),Form("cans_%s",layertag),1200,1000);
  cans->Divide(2,2);

  for (Int_t i=0; i<SSDNum; i++)
  {
    mg_scale[i] = new TMultiGraph();
    graph_k_Err_scale[i]     = new TGraph(CHNum, SSDCHNum[i], k_RelativeErr_scale[i]);
    graph_deadlayer_scale[i] = new TGraph(CHNum, SSDCHNum[i], deadlayer_scale[i]);

    graph_k_Err_scale[i]->SetMarkerStyle(20);
    graph_k_Err_scale[i]->SetMarkerColor(kBlue);
    graph_deadlayer_scale[i]->SetMarkerStyle(20);
    graph_deadlayer_scale[i]->SetMarkerColor(kRed);

    mg_scale[i]->Add(graph_k_Err_scale[i]);
    mg_scale[i]->Add(graph_deadlayer_scale[i]);

    cans->cd(i+1);
    gPad->SetGridx();
    mg_scale[i]->SetTitle(Form("SSD%d_%s_Peak1AndPeak2ParErrs_DeadLayer",i+1,layertag));
    mg_scale[i]->GetXaxis()->SetRangeUser(0.,17.);
    mg_scale[i]->GetXaxis()->SetNdivisions(117);

    auto hist_scale = graph_k_Err_scale[i]->GetHistogram();
    hist_scale->SetTitle(Form("SSD%d_%s_Peak1Peak2ParErrsWithDeadLayer",i+1,layertag));
    hist_scale->GetXaxis()->SetRangeUser(0.5,16.5);
    hist_scale->GetXaxis()->SetNdivisions(117);
    hist_scale->GetXaxis()->SetTitle("Channel");
    hist_scale->GetXaxis()->CenterTitle(1);
    hist_scale->GetXaxis()->SetTitleSize(0.04);
    hist_scale->GetYaxis()->SetRangeUser(YRangeLow,YRangeUp);
    hist_scale->Draw("Y+ H");
    gPad->Update();

    if  ((i<2) && strcmp(layertag,"L2F")==0) {
      k_Err_Yaxis[i]     = new TGaxis(gPad->GetUxmin(),YRangeLow,gPad->GetUxmin(),YScaleRangeUp_Err,
                                      RelativeErr_RangeLow_L2F,RelativeErr_RangeUp_L2F,1002,"-R");
    } else {
      k_Err_Yaxis[i]     = new TGaxis(gPad->GetUxmin(),YRangeLow,gPad->GetUxmin(),YScaleRangeUp_Err,
                                      RelativeErr_RangeLow,RelativeErr_RangeUp,1003,"-R");
    }
    deadlayer_Yaxis[i] = new TGaxis(gPad->GetUxmin(),YScaleRangeLow_DeadLayer,gPad->GetUxmin(),YRangeUp,
                                    DeadLayer_RangeLow,DeadLayer_RangeUp,1003,"-R");

    k_Err_Yaxis[i]->SetTitle("k_{RelativeErr} (%%)");
    k_Err_Yaxis[i]->CenterTitle(1);
    k_Err_Yaxis[i]->SetTitleSize(0.05);
    k_Err_Yaxis[i]->SetTitleColor(kBlue);
    k_Err_Yaxis[i]->SetLineColor (kBlue);
    k_Err_Yaxis[i]->SetLabelColor(kBlue);
    deadlayer_Yaxis[i]->SetTitle("Thickness(#mum)");
    deadlayer_Yaxis[i]->CenterTitle(1);
    deadlayer_Yaxis[i]->SetTitleSize(0.05);
    deadlayer_Yaxis[i]->SetTitleColor(kRed);
    deadlayer_Yaxis[i]->SetLineColor (kRed);
    deadlayer_Yaxis[i]->SetLabelColor(kRed);

    line_Err_average[i] = new TLine(gPad->GetUxmin(),k_RelativeErr_Ave_scale[i],
                                    gPad->GetUxmax(),k_RelativeErr_Ave_scale[i]);
    line_deadlayer_average[i] = new TLine(gPad->GetUxmin(),deadlayer_Ave_scale[i],
                                    gPad->GetUxmax(),deadlayer_Ave_scale[i]);
    line0_Err[i] = new TLine(gPad->GetUxmin(),YScaleRangeUp_Err,
                             gPad->GetUxmax(),YScaleRangeUp_Err);
    line0_deadlayer[i] = new TLine(gPad->GetUxmin(),YScaleRangeLow_DeadLayer,
                             gPad->GetUxmax(),YScaleRangeLow_DeadLayer);
    line_Err_average[i]->SetLineColor(kGreen);
    line_Err_average[i]->SetLineStyle(7);
    line_Err_average[i]->SetLineWidth(3);
    line_deadlayer_average[i]->SetLineColor(kGreen);
    line_deadlayer_average[i]->SetLineStyle(7);
    line_deadlayer_average[i]->SetLineWidth(3);
    line0_Err[i]->SetLineColor(kBlue);
    line0_Err[i]->SetLineStyle(2);
    line0_deadlayer[i]->SetLineColor(kRed);
    line0_deadlayer[i]->SetLineStyle(2);

    latex_Err[i]       = new TLatex(3,15.,Form("<k_{RelativeErr}> = %.3f (%%)<%.2f (%%)",k_RelativeErr_Ave[i],RelativeErrCut*100));
    latex_deadlayer[i] = new TLatex(3,40.,Form("<deadlayer> = %.5f (#mum)",deadlayer_Ave[i]));
    latex_Err[i]->SetTextColor(kBlue);
    latex_deadlayer[i]->SetTextColor(kRed);

    mg_scale[i]->Draw("PL");
    k_Err_Yaxis[i]->Draw();
    deadlayer_Yaxis[i]->Draw();
    line_Err_average[i]->Draw();
    line_deadlayer_average[i]->Draw();
    line0_Err[i]->Draw();
    line0_deadlayer[i]->Draw();
    latex_Err[i]->Draw();
    latex_deadlayer[i]->Draw();
  }
  cans->Print(pathPNDOutput.c_str());

  //______________________________________
  // 画出死层修正后的能量刻度曲线
  TF1* fPeak1_deadlayer[SSDNum][CHNum];
  TF1* fPeak2_deadlayer[SSDNum][CHNum];

  TLegend* legend[SSDNum][CHNum];
  TLatex*  latex [SSDNum][CHNum];

  TCanvas* cans_func = new TCanvas("cans_func","cans_func",800,600);
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  cans_begin->Close();
  cans_end->Close();

  cans_begin->Print(pathFitFuncPDFOutputBegin.c_str());
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      fPeak1_deadlayer[i][j] = new TF1(Form("fPeak1_DeadLayer_SSD%d_CH%d",i+1,j),
                                       Form("%.5f*x+%.5f",k1[i][j],h1[i][j]),0,4096);
      fPeak2_deadlayer[i][j] = new TF1(Form("fPeak2_DeadLayer_SSD%d_CH%d",i+1,j),
                                       Form("%.5f*x+%.5f",k2[i][j],h2[i][j]),0,4096);
      fPeak1_deadlayer[i][j]->SetTitle(Form("SSD%d_%s_CH%02d_FitFunctionDeadLayer",i+1,layertag,j));
      fPeak1_deadlayer[i][j]->GetXaxis()->SetTitle("ADC CH");
      fPeak1_deadlayer[i][j]->GetXaxis()->CenterTitle(1);
      fPeak1_deadlayer[i][j]->GetYaxis()->SetTitle("Energy (MeV)");
      fPeak1_deadlayer[i][j]->GetYaxis()->CenterTitle(1);

      fPeak1_deadlayer[i][j]->SetLineColor(kRed);
      fPeak2_deadlayer[i][j]->SetLineColor(kGreen);

      legend[i][j] = new TLegend(0.15,0.7,0.55,0.85);
      legend[i][j]->AddEntry(fPeak1_deadlayer[i][j],"Pulser+AlphaPeak1","l");
      legend[i][j]->AddEntry(fPeak2_deadlayer[i][j],"Pulser+AlphaPeak2","l");

      latex[i][j] = new TLatex(2000.,0.2*fPeak1_deadlayer[i][j]->GetMaximum(),Form("deadlayer=%.5f(#mum)",deadlayer[i][j] ));
      latex[i][j]->SetTextColor(kMagenta);

      cans_func->cd();
      fPeak1_deadlayer[i][j]->Draw();
      fPeak2_deadlayer[i][j]->Draw("SAME");
      legend[i][j]->Draw("SAME");
      latex[i][j] ->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans_func->Print(pathFitFuncPDFOutput.c_str());
    }
  }
  cans_end->Print(pathFitFuncPDFOutputEnd.c_str());
}


//______________________________________________________________________________
void GetSiEnergyCaliPars(const Char_t* layertag, Double_t& k, Double_t& h,
  Double_t& c, Int_t ssdindex, Int_t chindex, Double_t E_MeV, Int_t peakindex)
{
  std::string PulserTag("Height");   // 使用 "Height" 方式的脉冲刻度
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPulserIn(Form("output/SSD_%s_PulserCali_%s.dat",layertag,PulserTag.c_str()));
  std::string pathPulserInGain20;
  std::string pathAlphaCali00_08;
  std::string pathAlphaCali05_08;
  std::string pathAlphaCali00_48;

  Int_t numpar_PulserIn       = 3;  // a, err_a, b
  Int_t numpar_PulserInGain20 = 3;  // a, err_a, b
  Int_t numpar_AlphaCali00_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali05_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali00_48 = 3;  // peak1, peak2, peak3

  Double_t E_CH_00_08;
  Double_t E_CH_05_08;
  Double_t E_CH_00_48;

  ReadFileModule readfile;
  Double_t*** PulserIn = readfile.ReadData(pathPulserIn.c_str(), SSDNum, CHNum, numpar_PulserIn);
  Double_t a = PulserIn[ssdindex][chindex][0];
  Double_t b = PulserIn[ssdindex][chindex][2];

  //______________________________
  //  对 L1S 进行刻度
  if (strcmp(layertag,"L1S")==0) {

    pathPulserInGain20 = (Form("output/SSD_%s_PulserReCali_Gain20_%s.dat",layertag,PulserTag.c_str()));
    pathAlphaCali00_08 = (Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali00_08.c_str()));
    pathAlphaCali05_08 = (Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali05_08.c_str()));

    Double_t*** PulserInGain20 = readfile.ReadData(pathPulserInGain20.c_str(), SSDNum, CHNum, numpar_PulserInGain20);
    Double_t*** AlphaCHIn00_08 = readfile.ReadData(pathAlphaCali00_08.c_str(), SSDNum, CHNum, numpar_AlphaCali00_08);
    Double_t*** AlphaCHIn05_08 = readfile.ReadData(pathAlphaCali05_08.c_str(), SSDNum, CHNum, numpar_AlphaCali05_08);

    Double_t a_gain20 = PulserInGain20[ssdindex][chindex][0];
    Double_t b_gain20 = PulserInGain20[ssdindex][chindex][2];

    switch(peakindex) {
      case 1:   // 使用 peak1 进行刻度
          E_CH_00_08 = AlphaCHIn00_08[ssdindex][chindex][0];
          E_CH_05_08 = AlphaCHIn05_08[ssdindex][chindex][0];
          break;
      case 2:  // 使用 peak2 进行刻度
          E_CH_00_08 = AlphaCHIn00_08[ssdindex][chindex][1];
          E_CH_05_08 = AlphaCHIn05_08[ssdindex][chindex][1];
          break;
      default:
          cout<<Form("SSD%d_%s_%02d peak%d invalid!",ssdindex+1,layertag,chindex,peakindex);
    }

    // SSD1,SSD2,SSD3,SSD4 alpha 刻度文件使用不一样
    switch(ssdindex) {
      case 0:  // for SSD1,SSD2, 改变了gain
      case 1:
          c = E_MeV / (a_gain20 * E_CH_00_08 + b_gain20);
          break;
      case 2:  // SSD1,SSD2,SSD3 使用 AlphaCali00_08
          c = E_MeV / (a * E_CH_00_08 + b);
          break;
      case 3:   // SSD1,SSD2,SSD3 使用 AlphaCali00_08
          c = E_MeV / (a * E_CH_05_08 + b);
          break;
    }
    k = c * a;
    h = c * b;
  }

  //_____________________________________________________________
  //  对 L2F, L2B 进行刻度
  if (strcmp(layertag,"L2F")==0 || strcmp(layertag,"L2B")==0) {

    pathAlphaCali00_48 = (Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali00_48.c_str()));
    Double_t*** AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(), SSDNum, CHNum, numpar_AlphaCali00_48);

    switch(peakindex) {
      case 1:   // 使用 peak1 进行刻度
          E_CH_00_48 = AlphaCHIn00_48[ssdindex][chindex][0];
          break;
      case 2:  // 使用 peak2 进行刻度
          E_CH_00_48 = AlphaCHIn00_48[ssdindex][chindex][1];
          break;
      default:
          cout<<Form("SSD%d_%s_%02d peak%d invalid!",ssdindex+1,layertag,chindex,peakindex);
    }
    c = E_MeV / (a * E_CH_00_48 + b);
    k = c * a;
    h = c * b;
  }
}
