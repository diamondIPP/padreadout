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
//
Double_t convertToVoltage(unsigned short input){
	return (Double_t) (input / 65535. - 0.5) * 1000;
}

int GetDRS_revision(FILE* f){
	printf("GetDRS_revision\n\n");
	rewind(f);
	printf("GetDRS_revision\n\n");
	char patternArray[] = {'T','I','M','E'};
	int len_array = 4;
	long pos =0;
	printf("GetDRS_revision\n\n");
	bool version_5 = true;
	while (!feof(f) && pos < len_array) {
		int c = fgetc(f);
//		printf("%s != %s",c,patternArray[pos]);
		if (c != patternArray[pos])
			version_5 = false;
		pos++;
	}
	if (version_5)
		return 5;
	else
		return 4;
	return 0;
}

int GetNrecordedWaveforms(FILE* f,int drs_version){
	cout<<"Get N Waveforms: "<<endl;
	char patternArray[] = {'E','H','D','R'};
	char Buffer [4];
	char *tempBuffer = &Buffer[0];
	long pos =0;
	long old_pos = 1;
	int n_wf = -1;
	int header_length;
	if (drs_version == 5)
		header_length = sizeof(Header5_t);
	else
		header_length = sizeof(Header_t);
	int waveform_length = sizeof(SingleWaveform_t);
	cout<<"Header size: "<<header_length<<endl;
	cout<<"Waveform size:"<<waveform_length<<endl;
	bool found_header = false;
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
//			cout<<pos <<" \t\t@\""<<tempBuffer<<"\"@ \""<<patternArray<<"\""<<endl;
			//do a memort compare of 4 bytes
			if (memcmp(tempBuffer,patternArray,4)==0){
				if (found_header){
					long delta = pos-old_pos;
					long wf_diff = delta - header_length - 4;
					n_wf =  wf_diff/waveform_length;
					cout<<old_pos<<"-"<<pos<<" "<<delta<<" tempBuffer \""<<header_length<<"\" "<<" "<<wf_diff<<" "<<waveform_length<<" "<<n_wf<<" "<<(float)wf_diff/waveform_length<<endl;
					old_pos = pos;
				}
				found_header = true;
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
