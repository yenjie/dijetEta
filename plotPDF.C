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

TGraphAsymmErrors *plotPDF(int ptMin=25,int ptMax=55, bool isPPb=1, int pdfSet=2, int isCT=0, bool isEmbed = 0)
{
   // isPPb: true: pPb PDF, false: pp PDF
   // pdfSet: only useful when isPPb
   //         1: DSSZ        25 Eigenvalues
   //         2: EPS09       15 Eigenvalues
   //         3: nCT15       16 Eigenvalues
   // isCT: 1 = CT as the pp basedline, 0 = MM as the pp baseline
   static int count = 1;

   
   // 00 is the central
   
   vector <TH1D*> hList;
   
   string fileName;
   string title;
   int nEigenvalues=0;
   
   if (isPPb && pdfSet==1) {
      // DSSZ 
      cout <<"DSSZ"<<endl;
      title = "DSSZ";
      if (isCT) {
            fileName = "parsed_pPbNLO/PPBCMSNEWCTD";
      } else {
            fileName = "parsed_pPbNLO/PPBCMSNEWMMD";
      }
      nEigenvalues = 25;
   } else if (isPPb && pdfSet==2) {
      // EPS09
      cout <<"EPS09"<<endl;
      title = "EPS09";
      if (isCT) {
            fileName = "parsed_pPbNLO/PPBCMSNEWCTE";
      } else {
            fileName = "parsed_pPbNLO/PPBCMSNEWMME";
      }
      nEigenvalues = 15;
   } else if (isPPb && pdfSet==3) {
      // nCT15
      cout <<"nCTEQ15"<<endl;
      title = "nCTEQ15";
      if (isCT) {
            fileName = "parsed_pPbNLO/PPBCMSNEWQCT";
            nEigenvalues = 56;
      } else {
            fileName = "parsed_pPbNLO/PPBCMSNEWQMM";
            nEigenvalues = 50;
      } 
   } else if (!isPPb) {
      if (isCT) {
            title = "PPCT";
            fileName = "parsed_ppNLO/PPCMSNEWCT";
            nEigenvalues = 28;
      } else {
            title = "PPMM";
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
      if (hData==0) hData = (TH1D*)infData->Get(Form("hist_eta_%d",idx));

   } else {
      hData = (TH1D*)infData->Get(Form("hist_eta_pp_%d",idx));
   }
   hDataPP = (TH1D*)infData->Get(Form("hist_eta_pp_%d",idx))->Clone("hPPRef");

   TFile *infMCPP = new TFile(Form("parsed_ppNLO/PPCMSNEWCT00.out.root"));
   TH1D *hMCPP = (TH1D*)infMCPP->Get(Form("hist_ptave%d_%d",ptMin,ptMax))->Clone("hMCPP");
   

   TFile *outf = new TFile(Form("plot/output-isPPb%d-%s-isCT%d-%d-%d.root",isPPb,title.c_str(),isCT,ptMin,ptMax),"recreate");


   TCanvas *c;
   if (!isEmbed) c = new TCanvas(Form("c%d",count),"",600,600);
   
   normalize(hMCPP);
   normalize(hData);
   normalize(hDataPP);

//   hData->Divide(hDataPP);
   hData->Draw();  
   // read files

   
   for (int i=0;i<=2*nEigenvalues;i++)
   {
      TFile *inf;

      if (i<10) inf = new TFile(Form("%s0%d.out.root",fileName.c_str(),i));
      else inf = new TFile(Form("%s%d.out.root",fileName.c_str(),i));

      if (inf->IsZombie()) continue;
      outf->cd();
      TH1D *h = (TH1D*)inf->Get(Form("hist_ptave%d_%d",ptMin,ptMax))->Clone();
      h->SetName(Form("hist_ptave25_55_%2d",i));
      normalize(h);
      //h->Divide(hData);
      //cout <<"Data Area"<<hData->Integral(1,1000)<<" "<<h->Integral(1,1000)<<endl;;
      
        h->SetAxisRange(0.,0.5,"Y");
//      h->SetAxisRange(0.5,1.5,"Y");
      h->SetLineColor(kGray);
      if(i!=0) h->Draw("hist same"); else h->Draw("hist");      
      hList.push_back(h);
      inf->Close();
   }
   //hData->Divide(hData);
     
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
      l->SetFillStyle(3002);
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
   if (!isEmbed) {
      c->Update();
      c->SaveAs(Form("plot/output-isPPb%d-%s-isCT%d-%d-%d.png",isPPb,title.c_str(),isCT,ptMin,ptMax));
   }
   hData->Draw("same");
   g->SetName("g");
   g->Write();
   //outf->Close();
   return g;
}

void plot6Panel(bool isPPb, int pdfSet, int isCT)
{
   TCanvas *c = new TCanvas("c","",1500,1000);
   c->Divide(3,2);
   c->cd(1);
   plotPDF(25,55,isPPb,pdfSet,isCT,1);
   c->cd(2);
   plotPDF(55,75,isPPb,pdfSet,isCT,1);
   c->cd(3);
   plotPDF(75,95,isPPb,pdfSet,isCT,1);
   c->cd(4);
   plotPDF(95,115,isPPb,pdfSet,isCT,1);
   c->cd(5);
   plotPDF(115,150,isPPb,pdfSet,isCT,1);
   c->cd(6);
   plotPDF(150,400,isPPb,pdfSet,isCT,1);
   

   
}

void doAll()
{
   // pp CT
   plotPDF(25,55,0,1,1);
   plotPDF(55,75,0,1,1);
   plotPDF(75,95,0,1,1);
   plotPDF(95,115,0,1,1);
   plotPDF(115,150,0,1,1);
   plotPDF(150,400,0,1,1);

   // pp MM
   plotPDF(25,55,0,1,0);
   plotPDF(55,75,0,1,0);
   plotPDF(75,95,0,1,0);
   plotPDF(95,115,0,1,0);
   plotPDF(115,150,0,1,0);
   plotPDF(150,400,0,1,0);

   // pPb DSSZ+MM
   plotPDF(25,55,1,1,0);
   plotPDF(55,75,1,1,0);
   plotPDF(75,95,1,1,0);
   plotPDF(95,115,1,1,0);
   plotPDF(115,150,1,1,0);
   plotPDF(150,400,1,1,0);

   // pPb DSSZ+CT
   plotPDF(25,55,1,1,1);
   plotPDF(55,75,1,1,1);
   plotPDF(75,95,1,1,1);
   plotPDF(95,115,1,1,1);
   plotPDF(115,150,1,1,1);
   plotPDF(150,400,1,1,1);

   // pPb EPS+MM
   plotPDF(25,55,1,2,0);
   plotPDF(55,75,1,2,0);
   plotPDF(75,95,1,2,0);
   plotPDF(95,115,1,2,0);
   plotPDF(115,150,1,2,0);
   plotPDF(150,400,1,2,0);

   // pPb EPS+CT
   plotPDF(25,55,1,2,1);
   plotPDF(55,75,1,2,1);
   plotPDF(75,95,1,2,1);
   plotPDF(95,115,1,2,1);
   plotPDF(115,150,1,2,1);
   plotPDF(150,400,1,2,1);

   // pPb nCT15
   plotPDF(25,55,1,3,1);
   plotPDF(55,75,1,3,1);
   plotPDF(75,95,1,3,1);
   plotPDF(95,115,1,3,1);
   plotPDF(115,150,1,3,1);
   plotPDF(150,400,1,3,1);

}
