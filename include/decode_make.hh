#include <string.h>
#include <iomanip>      // std::setfill, std::setw
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <getopt.h>

#include <sys/stat.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TTree.h"
#include "TGraph.h"
#include "TString.h"
#include "TriggerDelay.hh"
#include "TableUpdater.hh"
#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TStopwatch.h"
#include "MyUtilities.hh"
#include "TMath.h"

using namespace std;


//void usage();
//void decode(TString);
#define WF_LENGHT 1024

TCanvas *c1;
bool verbose;
HeaderStart_t header_start;
Header_t header;
Header5_t header5;
//Waveform_t waveform;
SingleWaveform_t waveform[4];
TTree *rec;
FILE *f;
TFile *outfile;
Int_t year, month, day;
Double_t  time_stamp;
Double_t timestamp;
Double_t t[WF_LENGHT], chn1[WF_LENGHT], chn2[WF_LENGHT], chn3[WF_LENGHT], chn4[WF_LENGHT];
Int_t n_channels = 4;
Int_t n;
Int_t k =0;
Int_t delay_data = -2e3;
Int_t delay_cali = -2e3;
Double_t Integrals[] = {-1,-1,-1,-1};
Double_t *pIntegrals = &Integrals[0];
Int_t nTrigs;
Int_t nHits;
Int_t trigger_point;
bool saturated;
vector<Int_t> vTrigTimes;
vector<Int_t> vHitTimes;
Float_t avrg_chn1,avrg_chn2,avrg_chn3,avrg_chn4;

Float_t avrg_first_chn1;
Float_t avrg_first_chn2;
Float_t avrg_first_chn3;
Float_t avrg_first_chn4;
Float_t avrg_last_chn1;
Float_t avrg_last_chn2;
Float_t avrg_last_chn3;
Float_t avrg_last_chn4;


Int_t n_delay_cali = 0;
Int_t n_delay_data = 0;
Int_t fixed_delay_data = -1;
Int_t fixed_delay_cali = -1;
Int_t fixed_readout = -1;
Int_t n_trig = 0;
Int_t trig_mean;
Int_t trig_mean2;

int n_wf;
int calibflag;

time_t timer;
struct tm tm_time;
float trigger_time;
Int_t sampling_point;
Float_t sampling_time;
int drs_version;
int n_events;
char ch_header[] = {'C','0','0'};
bool check_header_start();
bool is_time_header();
bool get_next_event();
bool read_header();
bool read_header4();
bool read_header5();
bool read_waveforms();
bool check_waveform_headers();
void update_averages();
void get_trigger_times();

TimeBins_t timebins[4];
