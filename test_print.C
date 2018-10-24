#include <stdio.h>
#include <stdlib.h>

void test_print(char *filename=NULL) {
  /* A simple script to plot aspects of phototube hits
   *
   * I like to run this macro as
   * $ root -l -x 'test_print.C("../wcsim.root")'
   */

  gROOT->Reset();

  // searching for the environment string pointed to by the name
  // and returns the associated value to the string

  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");

  if(wcsimdirenv !=  NULL){
    //to load the shared classes to be run in root
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.rootmap");
    gSystem->Load("${WCSIMDIR}/src/WCSimRootDict_rdict.pcm");
  }else{
    std::cout << "Can't load WCSim ROOT dictionaries" << std::endl;
  }

  gStyle->SetOptStat(1);

  TFile *f;
  char fTest[128];
  if (filename==NULL){
    std::cout << "Please provide filename in option" << std::endl;
    std::cout << "Will load auto wcsim.root in WCSIMDIR ..." << std::endl;
    char* name = "wcsim.root";
    strncpy(fTest, wcsimdirenv, sizeof(fTest));
    strncat(fTest, "/", (sizeof(fTest) - strlen(fTest)) );
    strncat(fTest, name, (sizeof(fTest) - strlen(fTest)) );
    f = new TFile(fTest);
  }else{
    f = new TFile(filename);
  }
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }else{
    if (filename==NULL) cout << "File open bro: " << fTest << endl;
    else cout << "File open bro: " << filename << endl;
  }

  TTree  *wcsimT = (TTree*)f->Get("wcsimT");

  WCSimRootEvent *wcsimroothyperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimroothyperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  //wcsimT->GetBranch("wcsimrootevent_OD")->SetAutoDelete(kTRUE);

  // const long unsigned int nbEntries = wcsimT->GetEntries();
  const long unsigned int nbEntries = 5;
  cout << "Nb of entries " << wcsimT->GetEntries() << endl;


  for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
    // Point to event iEntry inside WCSimTree
    wcsimT->GetEvent(iEntry);

    // Nb of Trigger inside the event
    const unsigned int nbTriggers = wcsimroothyperevent->GetNumberOfEvents();
    const unsigned int nbSubTriggers = wcsimroothyperevent->GetNumberOfSubEvents();

    cout << "iEntry : " << iEntry << endl;
    cout << "nbTrig : " << nbTriggers << endl;
    cout << "nbSubTrig : " << nbSubTriggers << endl;

  }
} // END MACRO
