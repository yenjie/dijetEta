#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <vector>

void normalize(TH1D *h)
{
   double intergal = h->Integral(1,h->GetNbinsX());
   for (int i=1;i<=h->GetNbinsX();i++)
   {
      double val = h->GetBinContent(i)/intergal/h->GetBinWidth(i);
      //double valErr = h->GetBinError(i)/intergal/h->GetBinWidth(i);
      h->SetBinContent(i,val);
      //h->SetBinError(i,valErr);
   }
}

/*
  KEY: TH1D	hist_ptave25_55;1	
  KEY: TH1D	histerr_ptave25_55;1	
  KEY: TH1D	hist_ptave55_75;1	
  KEY: TH1D	histerr_ptave55_75;1	
  KEY: TH1D	hist_ptave75_95;1	
  KEY: TH1D	histerr_ptave75_95;1	
  KEY: TH1D	hist_ptave95_115;1	
  KEY: TH1D	histerr_ptave95_115;1	
  KEY: TH1D	hist_ptave115_150;1	
  KEY: TH1D	histerr_ptave115_150;1	
  KEY: TH1D	hist_ptave150_400;1	
  KEY: TH1D	histerr_ptave150_400;1	
*/

void plot(int ptMin=25,int ptMax=55)
{
   TCanvas *c = new TCanvas("c","",600,600);
   TH1D *hCentral;
   
   // 0 is the central
   
   vector <TH1D*> hList;
   
   for (int i=0;i<=50;i++)
   {
      TFile *inf;
      if (i<10) inf = new TFile(Form("PPBCMSNEWCTD0%d.out.root",i));
      else inf = new TFile(Form("PPBCMSNEWCTD%d.out.root",i));
      if (inf->IsZombie()) continue;
      TH1D *h = (TH1D*)inf->Get(Form("hist_ptave%d_%d",ptMin,ptMax));
      h->SetName(Form("hist_ptave25_55_%2d",i));
      normalize(h);
      if (i==0) hCentral=h;
      h->SetLineColor(kGray);
      if(i!=0) h->Draw("same"); else h->Draw();      
   }
   hCentral->SetLineColor(2);
   hCentral->Draw("same");
}
