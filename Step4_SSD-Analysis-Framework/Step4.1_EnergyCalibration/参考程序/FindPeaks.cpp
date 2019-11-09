// A script for peak finding using TSpectrum class
//

Int_t npeaks = 30;
//===========================================
// definite a function to generate the npeaks
Double_t fpeaks(Double_t *x, Double_t *par)
{
  Double_t result = par[0] + par[1]*x[0];
  for(Int_t p=0; p<npeaks; p++)
  {
    Double_t norm  = par[3*p+2];  // "height"
    Double_t mean  = par[3*p+3];
    Double_t sigma = par[3*p+4];

    result += norm*TMath::Gaus(x[0],mean,sigma); // 拟合函数是一个线性函数与高斯函数的叠加
  }
  return result;
}

//====================================
//  住程序开始
void FindPeaks()
{
  Int_t np = 10;
  npeaks = TMath::Abs(np);

  //===============================
  //  generate n peaks at random

  // set parameters
  Double_t par[300];
  par[0] = 0.8;
  par[1] = -0.6/1000;
  for(Int_t p=0; p<npeaks; p++)
  {
    par[3*p+2] = 1;   // "height"
    par[3*p+3] = 10+gRandom->Rndm()*980; //"mean"
    par[3*p+4] = 3+2*gRandom->Rndm();    //"sigma"
  }

  // definite fitting function and function setting
  TF1 *f = new TF1("f",fpeaks,0,1000,2+3*npeaks);
  f->SetNpx(1000);
  f->SetParameters(par);

  // histogram and draw
  TCanvas * c1 = new TCanvas("c1","c1",10,10,1000,900);
  c1->Divide(1,2);
  c1->cd(1);

  TH1F *h = new TH1F("h","test",500,0,1000);
  h->FillRandom("f",20000);
  h->Draw();

  // find  peak candidates
  TSpectrum *s = new  TSpectrum(2*npeaks);
  Int_t nfound = s->Search(h,2,"",0.10);
  printf("Found %d candidate peaks to fit\n",nfound);

  //==================================================
  // estimate background using TSpectrum::Background
  TH1 *hb = s->Background(h,20,"same"); // h-histogram, 20-迭代次数, "sanme"-draw option
  if (hb) c1->Update();
  if (np<0) return;

  //==============================================
  // estimate background using a fiting method
  // 1.定义拟合函数
  c1->cd(2);
  TF1 *fline = new TF1("fline","pol1",0,1000);
  h->Fit("fline","qn");

  // Loop on all found peaks: estimate peaks at the background level
  par[0] = fline->GetParameter(0);
  par[1] = fline->GetParameter(1);
  npeaks = 0;
  Double_t *xpeaks = s->GetPositionX();
  for(Int_t p=0; p<nfound; p++)
  {
    Double_t xp = xpeaks[p];
    Int_t bin   = h->GetXaxis()->FindBin(xp);
    Double_t yp = h->GetBinContent(bin);
    if(yp - TMath::Sqrt(yp) < fline->Eval(xp)) continue; // 判选条件
    par[3*npeaks+2] = yp;  //"height"
    par[3*npeaks+3] = xp;  //"mean"
    par[3*npeaks+4] = 3;   // "sigma"

    printf("找到第 %d 个peak: %.1f\n",npeaks,xp);

    npeaks++;
  }
  cout<<"有效的peak数目为:"<< npeaks << endl;
  cout<<"Now fitting: Be patient\n" <<endl;

  TH1F *h2 = (TH1F*)h->Clone("h2");
  TF1 *fit = new TF1("fit",fpeaks,0,1000,2+3*npeaks);
  TVirtualFitter::Fitter(h2,10+3*npeaks);
  fit->SetParameters(par);
  fit->SetNpx(1000);
  h2->Fit("fit");

  return;
}
