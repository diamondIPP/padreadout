#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <getopt.h>

#include "TFile.h"
#include "TF1.h"
#include "TTree.h"
#include "TGraph.h"
#include "TString.h"
#include "TriggerDelay.hh"
#include "TableUpdater.hh"
#include "TCanvas.h"

#include "TStopwatch.h"


using namespace std;


//void usage();
//void decode(TString);
struct Header_t {
  char           event_header[4];
  unsigned int   serial_number;
  unsigned short year;
  unsigned short month;
  unsigned short day;
  unsigned short hour;
  unsigned short minute;
  unsigned short second;
  unsigned short millisecond;
  unsigned short reserved1;
  float time[1024];
};

struct SingleWaveform_t{
  char           header[4];
  unsigned short chn[1024];
};
struct Waveform_t {
  char           chn1_header[4];
  unsigned short chn1[1024];
  char           chn2_header[4];
  unsigned short chn2[1024];
  char           chn3_header[4];
  unsigned short chn3[1024];
  char           chn4_header[4];
  unsigned short chn4[1024];
};


Header_t header;
Waveform_t waveform;
TTree *rec;
Int_t year, month, day;
Double_t  time_stamp;
Double_t t[1024], chn1[1024], chn2[1024], chn3[1024], chn4[1024];
Int_t n_channels = 4;
Int_t n;
Int_t k =0;
Int_t delay_data = -2e3;
Int_t delay_cali = -2e3;
Double_t Integrals[] = {-1,-1,-1,-1};
Double_t *pIntegrals = &Integrals[0];
Int_t nTrigs;
Int_t trigTime;
bool saturated;
vector<Int_t> vTrigTimes;
int calibflag;


