

#include <stdio.h>
#include <stdlib.h>

void PMT_position_photoelectron(char *filename=NULL) {
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

  WCSimRootEvent *wcsimroothyperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimroothyperevent);

  TTree  *wcsimGeoT = f->Get("wcsimGeoT");

  WCSimRootGeom *wcsimrootgeom = new WCSimRootGeom();
  wcsimGeoT->SetBranchAddress("wcsimrootgeom",&wcsimrootgeom);
  // wcsimrootgeom->GetEntry(0);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);


  //--------------------------
  // As you can see, there are lots of ways to get the number of hits.
  cout << "Nb of entries " << wcsimT->GetEntries() << endl;
  //-----------------------

  TH2D *QvsY = new TH2D("QvsY","charge vs. Y coordinate", 100, -100, 100, 100, -0.5, 15.5);
  QvsY->SetXTitle("Y coordinate of PMT");
  QvsY->SetYTitle("charge");

  const long unsigned int nbEntries = wcsimT->GetEntries();

  for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
    // Point to event iEntry inside WCSimTree
    wcsimT->GetEvent(iEntry);

    // Nb of Trigger inside the event
    const unsigned int nbTriggers = wcsimroothyperevent->GetNumberOfEvents();
    const unsigned int nbSubTriggers = wcsimroothyperevent->GetNumberOfSubEvents();

    for(long unsigned int iTrig = 0; iTrig < nbTriggers; iTrig++){
      WCSimRootTrigger *wcsimrootevent = wcsimroothyperevent->GetTrigger(iTrig);

      // RAW HITS
      int ncherenkovdigihits = wcsimrootevent->GetNcherenkovdigihits();
        for (int i = 0; i < ncherenkovdigihits; i++){
          WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)wcsimrootevent->GetCherenkovDigiHits()->At(i);

          double charge = hit->GetQ();
          int tubeId = hit -> GetTubeId();
          WCSimRootPMT pmt = wcsimrootgeom->GetPMT(tubeId);
          double pmtY = pmt.GetPosition(1);

          QvsY->Fill(pmtY, charge);
          } // END FOR RAW HITS

    } // END FOR iTRIG

  } // END FOR iENTRY


    TH1 *temp;
    TCanvas *c1 = new TCanvas("c1","c1", 600, 600);
    QvsY->Draw("colz");

  }
