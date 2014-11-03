/*
 * MyUtilities.hh
 *
 *  Created on: Nov 3, 2014
 *      Author: bachmair
 */

#ifndef MYUTILITIES_CPP_
#define MYUTILITIES_CPP_

#include "../include/MyUtilities.hh"

using namespace std;
int GetNrecordedWaveforms(FILE* f){
	cout<<"Get N Waveforms: "<<endl;
	char patternArray[] = {'E','H','D','R'};
	char Buffer [4];
	char *tempBuffer = &Buffer[0];
	long pos =0;
	long old_pos = 1;
	int n_wf = -1;
	while (!feof(f)) {
		pos++; // increase the character counter
		// Check the next byte in the file to see if it matches the
		//  file header, fgetc takes the file pointer and returns
		//  the byte that was read from the file.
		if (fgetc(f) == patternArray[0]){
			// When it finds the first byte in patternArray. Set the first
			//  byte in tempBuffer to the first byte in patternArray.
			tempBuffer[0] = patternArray[0];
			// read the next 3 bytes into the rest of tempBuffer
			fread(tempBuffer + 1, sizeof(char), 3, f);
			//cout<<pos <<" \t\t\""<<tempBuffer<<"\""<<endl;
			if (memcmp(tempBuffer,patternArray,4)==0){
				long delta = pos-old_pos;
				long wf_diff = delta - sizeof(Header_t);
				n_wf =  wf_diff/sizeof(SingleWaveform_t);
				//cout<<delta<<" tempBuffer \""<<"\" "<<" "<<wf_diff<<" "<<sizeof(SingleWaveform_t)<<" "<<n_wf<<endl;
				old_pos = pos;
			}
			pos+=3;
		}
		if(n_wf >= 0 && n_wf <=4)
			break;
	}
	rewind(f);	// find number of active channels
	cout<<"\rFound "<<n_wf<<" Waveforms in the file"<<endl;

	return n_wf;
}
#endif /* MYUTILITIES_CPP_ */
