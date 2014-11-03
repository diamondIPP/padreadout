/*
 * MyUtilities.hh
 *
 *  Created on: Nov 3, 2014
 *      Author: bachmair
 */

#ifndef MYUTILITIES_HH_
#define MYUTILITIES_HH_
#include <string.h>
#include <iomanip>      // std::setfill, std::setw
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "TROOT.h"

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
Double_t convertToVoltage(unsigned short input);

int GetNrecordedWaveforms(FILE* f);

#endif /* MYUTILITIES_HH_ */
