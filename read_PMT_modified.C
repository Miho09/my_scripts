

#include <stdio.h>
#include <stdlib.h>

void read_PMT(char *filename=NULL) {
  /* A simple script to plot aspects of phototube hits
   * This code is rather cavalier; I should be checking return values, etc.
   * First revision 6-24-10 David Webber
   *
   * I like to run this macro as
   * $ root -l -x 'read_PMT.C("../wcsim.root")'
   */

  gROOT->Reset();
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  }else{
    gSystem->Load("../libWCSimRoot.so");
  }
  gStyle->SetOptStat(1);

  TFile *f;
  if (filename==NULL){
    f = new TFile("../wcsim.root");
  }else{
    f = new TFile(filename);
  }
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }

  TTree  *wcsimT = f->Get("wcsimT");

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);


  // Currently only looks at one event.  I suspect you could loop over more events, if they existed.
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  //--------------------------
  // As you can see, there are lots of ways to get the number of hits.
  cout << "Number of tube hits " << wcsimrootevent->GetNumTubesHit() << endl;
  cout << "Number of Cherenkov tube hits " << wcsimrootevent->GetNcherenkovhits() << endl;
  cout << "Number of Cherenkov tube hits " << wcsimrootevent->GetCherenkovHits()->GetEntries() << endl;

  cout << "Number of digitized tube hits " << wcsimrootevent->GetNumDigiTubesHit() << endl;
  cout << "Number of digitized Cherenkov tube hits " << wcsimrootevent->GetNcherenkovdigihits() << endl;
  cout << "Number of digitized Cherenkov tube hits " << wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << endl;

  cout << "Number of photoelectron hit times " << wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << endl;
  cout << "Number of photoelectron hit times " << wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << endl;
  cout << "Nb of entries " << wcsimT->GetEntries() << endl;
  //-----------------------

  TH1D *PE = new TH1D("PEmult","Photoelectron multiplicty", 16,-0.5,100);
  PE->SetXTitle("Photoelectrons");

  TH1D *PMT_hits = new TH1D("PMT_hits","Hits vs PMT detector number", 120000,-0.5,120000-0.5);

  int max=wcsimrootevent->GetNcherenkovhits();
  for (int i = 0; i<max; i++){
    WCSimRootCherenkovHit *chit = wcsimrootevent->GetCherenkovHits()->At(i);
    PMT_hits->Fill(chit->GetTubeID());
    //WCSimRootCherenkovHit has methods GetTubeId(), GetTotalPe(int)
    PE->Fill(chit->GetTotalPe(1));
  }
  //PE->Draw("");

  //----------------------------

  TH2D *QvsT = new TH2D("QvsT","charge vs. time", 40, 0, 2000, 40, -0.5, 15.5);
  QvsT->SetXTitle("time");
  QvsT->SetYTitle("charge");

  int event_num;
  cout << "Please enter a value between 0 to 9: " << endl;
  cin >> event_num;
  wcsimT->GetEvent(event_num);

  int max = wcsimrootevent->GetNcherenkovdigihits();
  for (int i = 0; i<max; i++){
    WCSimRootCherenkovDigiHit *cDigiHit = wcsimrootevent->GetCherenkovDigiHits()->At(i);
    //WCSimRootChernkovDigiHit has methods GetTubeId(), GetT(), GetQ()
    QvsT->Fill(cDigiHit->GetT(), cDigiHit->GetQ());
    WCSimRootCherenkovHitTime *cHitTime = wcsimrootevent->GetCherenkovHitTimes()->At(i);
    //WCSimRootCherenkovHitTime has methods GetTubeId(), GetTruetime()
  }

  TH1 *temp;

    TPaveLabel *title = new TPaveLabel(0.1,0.94,0.9,0.98,
                   "Drawing options for one dimensional histograms");

    float win_scale=0.75;
    int n_wide=2;
    int n_high=2;
    TCanvas *c1 = new TCanvas("c1","c1",700*n_wide*win_scale,500*n_high*win_scale);
    c1->Divide(n_wide,n_high);
    c1->cd(1);
    QvsT->Draw("colz");

    c1->cd(2);
    temp=QvsT->ProjectionY();
    temp->SetTitle("charge");
    temp->Draw();
    // c1->GetPad(2)->SetLogy();

    c1->cd(3);
    temp=QvsT->ProjectionX();
    temp->SetTitle("hits vs time");
    temp->Draw();
    // c1->GetPad(3)->SetLogy();

    c1->cd(4);
    temp=PE;
    temp->Draw();
    // c1->GetPad(5)->SetLogy();

  }
