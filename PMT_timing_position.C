

#include <stdio.h>
#include <stdlib.h>

void PMT_all_positions(char *filename=NULL) {
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

  TTree  *wcsimGeoT = (TTree*) f->Get("wcsimGeoT");

  WCSimRootGeom *wcsimrootgeom = 0;
  wcsimGeoT->SetBranchAddress("wcsimrootgeom",&wcsimrootgeom);
  // cout << "wcsimrootgeom value: " << wcsimrootgeom << endl;
  // cout << "getentry: " << wcsimGeoT->GetEntries() << endl;
  wcsimGeoT->GetEntry(0);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);


  //--------------------------
  // As you can see, there are lots of ways to get the number of hits.
  cout << "Nb of entries " << wcsimT->GetEntries() << endl;

  //-----------------------




  TH2D *YvsQ = new TH2D("QvsY","Y coordinate vs. charge", 100, -0.5, 15.5, 1000, -4000, 4000);
  YvsQ->SetYTitle("Y coordinate of PMT");
  // graph.GetYaxis()->SetTitleOffset(1.4);
  YvsQ->SetXTitle("charge");

  TH2D *XvsQ = new TH2D("QvsX","X coordinate vs. charge", 100, -0.5, 15.5, 1000, -4000, 4000);
  XvsQ->SetYTitle("X coordinate of PMT");
  XvsQ->SetXTitle("charge");

  TH2D *ZvsQ = new TH2D("QvsZ","Z coordinate vs. charge", 100, -0.5, 15.5, 1000, -4000, 4000);
  ZvsQ->SetYTitle("Z coordinate of PMT");
  ZvsQ->SetXTitle("charge");

  TH2D *YvsX = new TH2D("YvsX","Y coordinate vs. X coordinate", 1000, -4000, 4000, 1000, -4000, 4000);
  YvsX->SetYTitle("Y coordinate of PMT");
  YvsX->SetXTitle("X coordinate of PMT");

  TH2D *QvsT = new TH2D("QvsT","charge vs. time", 40, 0, 2000, 40, -0.5, 15.5);
  QvsT->SetXTitle("time");
  QvsT->SetYTitle("charge");


//ben added

TFile mihofile("test.root","RECREATE");
TTree mihotree("data","data");

float x,y,z,q,t;
mihotree.Branch("x",&x,"x/F");
mihotree.Branch("y",&y,"y/F");
mihotree.Branch("z",&z,"z/F");
mihotree.Branch("q",&q,"q/F");
mihotree.Branch("t",&t,"t/F");

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
          WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)
          (wcsimrootevent->GetCherenkovDigiHits()->At(i));
      //WCSimRootChernkovDigiHit has methods GetTubeId(), GetT(), GetQ()
            QvsT->Fill(hit->GetT(), hit->GetQ());
            // WCSimRootCherenkovHitTime *cHitTime = wcsimrootevent->GetCherenkovHitTimes()->At(i);
            //WCSimRootCherenkovHitTime has methods GetTubeId(), GetTruetime()
          }


          double charge = hit->GetQ();
          int tubeId = hit -> GetTubeId();
          double timing = hit->GetT();
          // cout << "Tube ID: " << tubeId << endl;
          WCSimRootPMT pmt = wcsimrootgeom->GetPMT(tubeId);
          double pmtX = pmt.GetPosition(0);
          double pmtY = pmt.GetPosition(1);
          double pmtZ = pmt.GetPosition(2);

          x=pmtX;
          y=pmtY;
          z=pmtZ;
          q=charge;
          t=timing;

          mihotree.Fill();

          // cout << "Y value: " << pmtY << endl;

          XvsQ->Fill(charge, pmtX);
          YvsQ->Fill(charge, pmtY);
          ZvsQ->Fill(charge, pmtZ);
          YvsX->Fill(pmtX, pmtY);

          } // END FOR RAW HITS

    } // END FOR iTRIG

  } // END FOR iENTRY

mihotree.Write();
mihofile.Close();

  TH1 *temp;
    float win_scale=0.75;
    int n_wide=2;
    int n_high=3;
    TCanvas *c1 = new TCanvas("c1","c1",800*n_wide*win_scale,800*n_high*win_scale);
    c1->Divide(n_wide,n_high);
    c1->cd(1);
    YvsQ->Draw("colz");

    c1->cd(2);
    XvsQ->Draw("colz");

    c1->cd(3);
    ZvsQ->Draw("colz");

    c1->cd(4);
    YvsX->Draw("colz");

    c1->cd(5);
    temp=QvsT->ProfileX();
    temp->SetTitle("average charge vs time");
    temp->Draw();



  }
