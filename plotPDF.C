#include <vector>
#include <iostream>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TString.h>
#include <TLine.h>
#include <TBox.h>
#include <TGraphAsymmErrors.h>

void normalize(TH1D *h)
{
//   double intergal = h->Integral(1,h->GetNbinsX());
   double integral=0;
   for (int i=1;i<=h->GetNbinsX();i++)
   {
      integral+= h->GetBinContent(i);
   }
   for (int i=1;i<=h->GetNbinsX();i++)
   {
      double val = h->GetBinContent(i)/integral/h->GetBinWidth(i);
      //double valErr = h->GetBinError(i)/integral/h->GetBinWidth(i);
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

TGraphAsymmErrors *plotPDF(int ptMin=25,int ptMax=55, bool isPPb=1, int pdfSet=2, int isCT=0)
{
   


   TCanvas *c = new TCanvas("c","",600,600);
   
   // 00 is the central
   
   vector <TH1D*> hList;
   
   string fileName;
   int nEigenvalues=0;
   
   if (isPPb && pdfSet==1) {
      // DSSZ 
      cout <<"DSSZ"<<endl;
      if (isCT) {
            fileName = "parsed_pPbNLO/PPBCMSNEWCTD";
      } else {
            fileName = "parsed_pPbNLO/PPBCMSNEWMMD";
      }
      nEigenvalues = 25;
   } else if (isPPb && pdfSet==2) {
      // EPS09
      cout <<"EPS09"<<endl;
      if (isCT) {
            fileName = "parsed_pPbNLO/PPBCMSNEWCTE";
      } else {
            fileName = "parsed_pPbNLO/PPBCMSNEWMME";
      }
      nEigenvalues = 15;
   } else if (!isPPb) {
      if (isCT) {
            fileName = "parsed_ppNLO/PPCMSNEWCT";
            nEigenvalues = 28;
      } else {
            fileName = "parsed_ppNLO/PPCMSNEWMM";
	    nEigenvalues = 25;
      }
   
   }
   
   TFile *infData = new TFile("../outf_pPb_pp.root");
   int idx=0;
   if (ptMin==25) idx=0;
   if (ptMin==55) idx=1;
   if (ptMin==75) idx=2;
   if (ptMin==95) idx=3;
   if (ptMin==115) idx=4;
   if (ptMin==150) idx=5;
   
   TH1D *hData;
   TH1D *hDataPP;
   if (isPPb) {
      hData = (TH1D*)infData->Get(Form("hist_eta_pPb_%d",idx));
   } else {
      hData = (TH1D*)infData->Get(Form("hist_eta_pp_%d",idx));
   }
   hDataPP = (TH1D*)infData->Get(Form("hist_eta_pp_%d",idx));

   hData->Draw();  
   // read files
   
   for (int i=0;i<=2*nEigenvalues;i++)
   {
      TFile *inf;

      if (i<10) inf = new TFile(Form("%s0%d.out.root",fileName.c_str(),i));
      else inf = new TFile(Form("%s%d.out.root",fileName.c_str(),i));

      if (inf->IsZombie()) continue;
      TH1D *h = (TH1D*)inf->Get(Form("hist_ptave%d_%d",ptMin,ptMax));
      h->SetName(Form("hist_ptave25_55_%2d",i));
      normalize(h);
      //h->Add(hData,-1);
      //cout <<"Data Area"<<hData->Integral(1,1000)<<" "<<h->Integral(1,1000)<<endl;;
      
      h->SetAxisRange(0,0.5,"Y");
      h->SetLineColor(kGray);
      if(i!=0) h->Draw("hist same"); else h->Draw("hist");      
      hList.push_back(h);
   }
   
   TGraphAsymmErrors *g = new TGraphAsymmErrors;
      
   for (int j=1;j<=hList[0]->GetNbinsX();j++) 
   {
      double deltaXp=0;
      double deltaXn=0;
      for (int i=1;i<=nEigenvalues;i++)
      {
         double diff1 = hList[i*2-1]->GetBinContent(j)-hList[0]->GetBinContent(j);
	 double diff2 = hList[i*2]->GetBinContent(j)-hList[0]->GetBinContent(j);
	 double diffp=0;
	 double diffn=0;
	 if (diff1>diffp) diffp=diff1;
	 if (diff2>diffp) diffp=diff2;
	 if (-diff1>diffn) diffn=-diff1;
	 if (-diff2>diffn) diffn=-diff2;
	 deltaXp+=diffp*diffp;
	 deltaXn+=diffn*diffn;
      }
      
      deltaXp=sqrt(deltaXp);
      deltaXn=sqrt(deltaXn);
      
      TBox *l = new TBox(hList[0]->GetBinLowEdge(j),hList[0]->GetBinContent(j)+deltaXp,hList[0]->GetBinLowEdge(j+1),hList[0]->GetBinContent(j)-deltaXn);
      l->SetFillStyle(3003);
      l->SetFillColor(4);
      l->SetLineColor(4);
      l->Draw();
      cout <<hList[0]->GetBinCenter(j)<<" "<<hList[0]->GetBinContent(j)<<" "<<deltaXp<<" "<<deltaXn<<endl;
      g->SetPoint(j-1,hList[0]->GetBinCenter(j),hList[0]->GetBinContent(j));
      g->SetPointError(j-1,hList[0]->GetBinCenter(j)-hList[0]->GetBinLowEdge(j),-hList[0]->GetBinCenter(j)+hList[0]->GetBinLowEdge(j+1),deltaXn,deltaXp);
   }
   hList[0]->SetTitle(Form("%s,%d,%d",fileName.c_str(),ptMin,ptMax));
   hList[0]->SetYTitle("Arbitrary Unit");
   hList[0]->SetLineColor(2);
   hList[0]->Draw("hist same");
   //hData->Draw("same");
   return g;
}
