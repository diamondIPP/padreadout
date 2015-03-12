#include "decode_make.hh"
#define fopen64 fopen
#define DEFAULT_TRIGTIME 245

vector<Int_t> Get1stTriggerTime(Double_t trig_level,unsigned short chn_trg[],Int_t &nTrigs) {
	unsigned short trigger_level = (trig_level/1000+.5)*65535;

	vector<Int_t> vecTrigTimes;
	nTrigs = 0;
	Int_t trigger_on = 0;
	Int_t i_on = -1;
	//vector<Int_t> trigger_time  = -1;
	Int_t min_trigger_length = 7; // 7  = 10 ns at 0.7 GSamples/s
	Int_t first_trigger_time  = -1;
	for (Int_t i=0;i<WF_LENGHT;i++){
		//Turn the trigger status ON if signal goes below trigger level (Usually -200 mV)
		if (chn_trg[i]<trigger_level && trigger_on == 0) {
			trigger_on = 1;
			i_on=i;
		}
		//Turn trigger status to 0 (OFF) if the singal goes back above the trigger level
		if (chn_trg[i]>trigger_level and trigger_on == 1) {
			trigger_on = 0;
			// If the trigger was longer than the minimum trigger length count the trigger
			if(i_on+min_trigger_length<i) {
				//trigger_time.push_back(i_on);
				nTrigs++;
				vecTrigTimes.push_back(i_on);
			}
			if(nTrigs == 1) first_trigger_time = i_on;
		}
	}
	if (verbose){
		cout<<"\nGet1stTriggerTime"<<endl;
		cout<<first_trigger_time<<" "<<nTrigs<<endl;
	}
	return vecTrigTimes;
}


void GetIntegrals(Int_t k,unsigned short sig[],Int_t trigTime,Int_t delay, Double_t *Integrals){
	if(verbose)
		cout<<k <<"Get Integral "<< trigTime<<" "<<delay<<endl;
	for(Int_t i = 0; i<4; i++) Integrals[i] = -1;
	if(trigTime<0 ||trigTime>=WF_LENGHT)
		return;
	if (delay == -2e3) delay = 0;
	Int_t pos = trigTime+delay;
	if (fixed_readout >= 0){
		if(verbose)
			cout<<"Setting fixed readout position: "<<fixed_readout<<endl;
		pos = fixed_readout;
	}
	sampling_point = pos;
	sampling_time = header.time[pos];
	//return;
	Int_t integral = 0;
	if (pos<0 || WF_LENGHT <= pos)
		return;

	integral += sig[pos];
	int n = 0;
	for (Int_t i = 1; i<= 200; i++){
		if (pos+i>=WF_LENGHT)
			continue;
		if (pos-i<0)
			continue;
		n +=2;
		integral += sig[pos+i];
		integral += sig[pos-i];
		switch (i){
		case 25:
			Integrals[0] = integral/(Double_t)(n+1);
			Integrals[0] = convertToVoltage(Integrals[0]);
			break;
		case 50:
			Integrals[1] = integral/(Double_t)(n+1);
			Integrals[1] = convertToVoltage(Integrals[1]);
			break;
		case 100:
			Integrals[2] = integral/(Double_t)(n+1);
			Integrals[2] = convertToVoltage(Integrals[2]);
			break;
		case 200:
			Integrals[3] = integral/(Double_t)(n+1);
			Integrals[3] = convertToVoltage(Integrals[3]);
			break;
		}
	}
}


void SetBranches(){
	rec->SetAutoSave(10000000000); // prevent the tree from writing itself multiple times into the file. currently set to 10 GB
	rec->Branch("year", &year ,"year/I");
	rec->Branch("month", &month ,"month/I");
	rec->Branch("day", &day ,"day/I");
	rec->Branch("time_stamp", &time_stamp ,"time_stamp/D");
	rec->Branch("timestamp",&timestamp, "timestamp/D");
	//rec->Branch("t", &t   ,"t[WF_LENGHT]/D");
	rec->Branch("n", &k   ,"n/I");
	// Create branches for the channels
	//rec->Branch("chn1", &chn1 ,"chn1[WF_LENGHT]/D");
	//rec->Branch("chn2", &chn2 ,"chn2[WF_LENGHT]/D");
	rec->Branch("calibflag", &calibflag ,"calibflag/I");
	rec->Branch("delay_data",&delay_data ,"delay_data/I");
	rec->Branch("delay_cali",&delay_cali ,"delay_cali/I");
	rec->Branch("Integral50",&Integrals[0],"Integral50/D");
	rec->Branch("Integral100",&Integrals[1],"Integral100/D");
	rec->Branch("Integral200",&Integrals[2],"Integral200/D");
	rec->Branch("Integral400",&Integrals[3],"Integral400/D");
	rec->Branch("saturated",&saturated,"saturated/O");
	rec->Branch("nTrigs",&nTrigs,"nTrigs/I");
	rec->Branch("trigger_point",&trigger_point,"trigger_point/I");
	rec->Branch("vTrigTimes",&vTrigTimes);
	rec->Branch("vHitTimes",&vHitTimes);
	rec->Branch("nHits",&nHits,"nHits/I");
	rec->Branch("avrg_chn1", &avrg_chn1,"avrg_chn1/F");
	rec->Branch("avrg_chn2", &avrg_chn2,"avrg_chn2/F");
	rec->Branch("avrg_chn3", &avrg_chn3,"avrg_chn3/F");
	rec->Branch("avrg_chn4", &avrg_chn4,"avrg_chn4/F");
	rec->Branch("avrg_first_chn1", &avrg_first_chn1,"avrg_first_chn1/F");
	rec->Branch("avrg_first_chn2", &avrg_first_chn2,"avrg_first_chn2/F");
	rec->Branch("avrg_first_chn3", &avrg_first_chn3,"avrg_first_chn3/F");
	rec->Branch("avrg_first_chn4", &avrg_first_chn4,"avrg_first_chn4/F");
	rec->Branch("avrg_last_chn1", &avrg_last_chn1,"avrg_last_chn1/F");
	rec->Branch("avrg_last_chn2", &avrg_last_chn2,"avrg_last_chn2/F");
	rec->Branch("avrg_last_chn3", &avrg_last_chn3,"avrg_last_chn3/F");
	rec->Branch("avrg_last_chn4", &avrg_last_chn4,"avrg_last_chn4/F");
	rec->Branch("n_wf",&n_wf,"n_wf/I");
	rec->Branch("trigger_time",&trigger_time,"trigger_time/F");
	rec->Branch("sampling_time",&sampling_time,"sampling_time/F");
	rec->Branch("sampling_point",&sampling_point,"sampling_point/I");
    rec->Branch("fixed_readout",&fixed_readout,"fixed_readout/I");

	rec->Branch("fixed_delay_cali",&fixed_delay_cali,"fixed_delay_cali/I");
	rec->Branch("fixed_delay_data",&fixed_delay_data,"fixed_delay_data/I");
}

void find_trigger_delay(){
	TFile* rootfile = outfile;
	cout <<"Find Trigger delays"<<endl;
	TriggerDelay dt_data(128,"DATA");
	TriggerDelay dt_cali(16,"CALI");
	TriggerDelay dt_trig(128,"TRIG");
	Int_t delay_data_old = delay_data;
	Int_t delay_cali_old = delay_cali;
	for (n=-1 ;true && n <10; n++) {
		if (!get_next_event())
			break;
		if (n%100==0)
			cout<<"\r "<<n<<" "<<dt_data.GetEvents()<<"\t"<<dt_cali.GetEvents()<<" "<<n_delay_cali<<" "<<n_delay_data<<" "<<" "<<avrg_chn2<<" "<<flush;
		get_trigger_times();
		update_averages();
		if (calibflag)
			dt_cali.AddWaveform(waveform[0].chn,WF_LENGHT);
		else{
			dt_data.AddWaveform(waveform[0].chn,WF_LENGHT);
			dt_trig.AddWaveform(waveform[1].chn,WF_LENGHT);
		}
		if (dt_data.GetEvents()<dt_data.GetNAvrg() && dt_cali.GetEvents()<dt_cali.GetNAvrg())
			continue;

		Float_t mean = (Float_t)trig_mean/(Float_t)n_trig;
		Float_t mean2 = (Float_t)trig_mean2/(Float_t)n_trig-mean*mean;
		Float_t sigma = mean2>=0?sqrt(mean2):-1;
		if (calibflag){
			if (delay_cali!=-2e3){
			}
			else
				delay_cali = dt_cali.check_delay(trig_mean/n_trig);
		}
		else{
			if (delay_data!=-2e3){
			}
			else
				delay_data = dt_data.check_delay(trig_mean/n_trig);
		}
		if (delay_data != -2e3 && delay_cali != -2e3){
			break;
		}
	}
	cout<<"Found Trigger delays: "<<endl;
	cout<<"\tData: "<<delay_data<<endl;
	cout<<"\tCali: "<<delay_cali<<endl;

	if (delay_data_old != -2e3){
		fixed_delay_data = delay_data;
		delay_data = delay_data_old;
		cout<<"USING fixed delay data: "<<delay_data<<endl;
	}
	if (delay_cali_old != -2e3){
		fixed_delay_cali = delay_cali;
		delay_cali = delay_cali_old;
		cout<<"USING fixed delay cali: "<<delay_cali<<endl;

	}
	rootfile->cd();
	TGraph *g_data =dt_data.GetAverageGraph();
	TGraph *g_trig =dt_trig.GetAverageGraph();
	TGraph *g_cali =dt_cali.GetAverageGraph();
	g_data->Write();
	g_cali->Write();
	g_trig->Write();
	TMultiGraph *mg = new TMultiGraph("mgAverageSignals","mgAverageSignals");
	mg->Add(g_data);
	mg->Add(g_cali);
	mg->Add(g_trig);
	mg->Write();
	delete mg;
	rewind(f);
}

void get_trigger_times(){

	vector<Int_t> trigTimes = Get1stTriggerTime(-100,waveform[1].chn,nTrigs);
	vector<Int_t>  hitTimes = Get1stTriggerTime(-100,waveform[3].chn,nHits);

	if (n == 1 && verbose)
		cout<<"GetTrigTimes"<<flush;
	vTrigTimes.clear();
	for (UInt_t i =0;i < trigTimes.size();i++)
		vTrigTimes.push_back(trigTimes.at(i));

	vHitTimes.clear();
	if(n_wf > 3)
		for (UInt_t i =0;i < hitTimes.size();i++)
			vHitTimes.push_back(hitTimes.at(i));

	if (nTrigs==0)
		trigger_point = -1;
	else{
		trigger_point = trigTimes.at(0);
		trig_mean += trigger_point;
		trig_mean2+= trigger_point*trigger_point;
		n_trig++;
	}

	if(trigTimes.size()==0){
		if(verbose) cout<<"Cannot Find TrigTime" << endl;
		trigTimes.push_back(DEFAULT_TRIGTIME);
		trigger_time  = -1;
	}
	else{
		trigger_time =header.time[trigger_point];
		//		cout<<" "<<trigTime<<" "<<tigger_time<<"s "<< flush;
	}
}

void update_averages(){
	saturated = false;
	int n_min = 0;
	int n_avrg = 50;
	avrg_first_chn1 = 0;
	avrg_first_chn2 = 0;
	avrg_first_chn3 = 0;
	avrg_first_chn4 = 0;
	avrg_last_chn1 = 0;
	avrg_last_chn2 = 0;
	avrg_last_chn3 = 0;
	avrg_last_chn4 = 0;
	avrg_chn1 = 0;
	avrg_chn2 = 0;
	avrg_chn3 = 0;
	avrg_chn4 = 0;
	for (Int_t i=0; i<WF_LENGHT; i++) {
		if (!saturated && (waveform[0].chn[i] >= 65535 || waveform[0].chn[i]==0))
			saturated = true;
		if (waveform[2].chn[i]<30)
			n_min++;
		avrg_chn1 += waveform[0].chn[i];
		avrg_chn2 += waveform[1].chn[i];
		avrg_chn3 += waveform[2].chn[i];
		avrg_chn4 += waveform[3].chn[i];
		if (i <n_avrg){
			avrg_first_chn1 += waveform[0].chn[i];
			avrg_first_chn2 += waveform[1].chn[i];
			avrg_first_chn3 += waveform[2].chn[i];
			avrg_first_chn4 += waveform[3].chn[i];
			avrg_last_chn1 += waveform[0].chn[WF_LENGHT - i];
			avrg_last_chn2 += waveform[1].chn[WF_LENGHT - i];
			avrg_last_chn3 += waveform[2].chn[WF_LENGHT - i];
			avrg_last_chn4 += waveform[3].chn[WF_LENGHT - i];
		}
		/*if (n_min > 30 && saturated && i > n_avrg)
		  break;
		 */
	}
	avrg_chn1 = convertToVoltage(avrg_chn1/WF_LENGHT);
	avrg_chn2 = convertToVoltage(avrg_chn2/WF_LENGHT);
	avrg_chn3 = convertToVoltage(avrg_chn3/WF_LENGHT);
	avrg_chn4 = convertToVoltage(avrg_chn4/WF_LENGHT);

	avrg_first_chn1 = convertToVoltage(avrg_first_chn1/n_avrg);
	avrg_first_chn2 = convertToVoltage(avrg_first_chn2/n_avrg);
	avrg_first_chn3 = convertToVoltage(avrg_first_chn3/n_avrg);
	avrg_first_chn4 = convertToVoltage(avrg_first_chn4/n_avrg);

	avrg_last_chn1 = convertToVoltage(avrg_last_chn1/n_avrg);
	avrg_last_chn2 = convertToVoltage(avrg_last_chn2/n_avrg);
	avrg_last_chn3 = convertToVoltage(avrg_last_chn3/n_avrg);
	avrg_last_chn4 = convertToVoltage(avrg_last_chn4/n_avrg);

	calibflag = (n_min>10 && n_wf > 2);
}

bool is_time_header(){
	char hdr[] = {'T','I','M','E'};
	if (memcmp(header_start.event_header,hdr,4)!=0){
//		cout << "time header  " << header_start.event_header << std::endl;
		return false;
	}
	return true;
}
bool check_header_start(){
	char hdr[] = {'E','H','D','R'};
	if (memcmp(header_start.event_header,hdr,4)!=0){
		cout << "event header  " << header_start.event_header << std::endl;
		return false;
	}
	return true;
}

bool read_header(){
	if (drs_version == 5)
		return read_header5();
	else
		return read_header4();
}
bool read_header5(){
		year = header5.year;
		month = header5.month;
		day = header5.day;
		time_stamp = header5.hour*3600+header5.minute*60+header5.second+header5.millisecond/1000.;
		tm_time.tm_hour = header5.hour;
		tm_time.tm_min  = header5.minute;
		tm_time.tm_sec  = header5.second;
		tm_time.tm_year = header5.year;
		tm_time.tm_mon  = header5.month;
		tm_time.tm_mday = header5.day;
		timestamp = int(mktime(&tm_time))+header5.millisecond/1000.;
		if (n == 0 && verbose) cout << "millisecond " << header5.millisecond << std::endl;
		if (n == 0 && verbose) cout << "waveform size : " <<    sizeof(waveform)/4*n_channels << endl;
		if (n == 0 && verbose){
			cout <<"\nserial: "<<header5.serial_number;
			cout <<"\nreserv: "<<header5.reserved1;
		}
		// print this info for every 10kth event
		if (n%10000 == 0) cout << "\t year "<< year << ": month " << month << ": day " << day <<
				" : hour " << std::setfill(' ')<<std::setw(2)<<header5.hour <<
				" : minute " <<std::setfill(' ')<<std::setw(2)<< header5.minute <<
				" \t "<<timestamp<< " "<<std::flush;;
		return true;
}


bool read_header4(){	// get the year/month/date
	year = header.year;
	month = header.month;
	day = header.day;
	time_stamp = header.hour*3600+header.minute*60+header.second+header.millisecond/1000.;
	tm_time.tm_hour = header.hour;
	tm_time.tm_min  = header.minute;
	tm_time.tm_sec  = header.second;
	tm_time.tm_year = header.year;
	tm_time.tm_mon  = header.month;
	tm_time.tm_mday = header.day;
	timestamp = int(mktime(&tm_time))+header.millisecond/1000.;
	if (n == 0 && verbose) cout << "millisecond " << header.millisecond << std::endl;
	if (n == 0 && verbose) cout << "waveform size : " <<    sizeof(waveform)/4*n_channels << endl;
	if (n == 0 && verbose){
		cout <<"\nserial: "<<header.serial_number;
		cout <<"\nreserv: "<<header.reserved1;
	}
	// print this info for every 10kth event
	if (n%10000 == 0) cout << "\t year "<< year << ": month " << month << ": day " << day <<
			" : hour " << std::setfill(' ')<<std::setw(2)<<header.hour <<
			" : minute " <<std::setfill(' ')<<std::setw(2)<< header.minute <<
			" \t "<<timestamp<< " "<<std::flush;;
	return true;
}

bool check_single_waveform_header(char *waveform_header){
	bool retval = memcmp(ch_header,waveform_header,3) == 0;
	if (verbose && false){
		cout<<"wf_header \"";
		for (int i=0;i<4;i++)
			cout<<waveform_header[i];
		cout<<"\" "<<retval<<endl;
	}
	return retval;
}


bool check_waveform_headers(){
	bool retval = true;
	for (int ch = 0; ch<n_wf;ch++)
		retval = retval && check_single_waveform_header(waveform[ch].header);
	if (verbose)
		cout<<"waveform_check: "<<n_wf<<" Wfs with  => "<<retval<<endl;
	return retval;
}

bool read_timebins(){
	cout <<"read time header "<<sizeof(TimeHeader_t)<<endl;
	if (fread(&header5.reserved2, sizeof(TimeHeader_t), 1, f) <= 0){
		cout<<"BREAK time header"<<endl;
		return false;
	}
	Int_t s = sizeof(SingleTimeBins_t);
	long length = s*n_wf;
	cout<<"read timebins"<<length<<endl;
	if (fread(&timebins, length, 1, f) <= 0){
		cout<<"BREAK time bins"<<endl;
		return false;
	}
	return true;
}
bool read_waveforms(){
	if (n%1000 == 0) cout << "\r"<<n<<"/"<<n_events << std::flush;
	// read the waveform
	Int_t s = sizeof(SingleWaveform_t);
	//	if (verbose && n == 1)
	long length = s*n_wf;
	if (verbose) cout<<"read waveform: "<<length<<endl;
	float sizeofwaveform = fread(&waveform, length, 1, f);
	if (verbose && n == 1)
		cout << "sizeofwaveform: " << sizeofwaveform << std::endl;
	return check_waveform_headers();
}

bool get_next_event(){
	if (verbose) cout<<"\nGet Next Event"<<endl;
	int header_length;
	if (drs_version == 5)
		header_length = sizeof(header5);
	else
		header_length = sizeof(header);
	while (true){
//		cout<<"continue search"<<endl;
		if (verbose) cout << "read header start  "<<sizeof(header_start)<<endl;
		if (fread(&header_start, sizeof(header_start), 1, f) <= 0)
			return false;
		bool time_header = is_time_header();
		if (time_header)
			if (verbose) cout<<"FOUND TIME HEADER"<<endl;
		if (!check_header_start()&& !time_header){
			cerr<<"Didn't find header"<<endl;
			continue;
		}
		if (verbose) cout<<"Found header"<<endl;
		if (time_header){
			if (!read_timebins())
				return false;
		}
		else if (drs_version == 5){
			if (verbose) cout<<"read header5 "<<header_length<<endl;
			if (fread(&header5, header_length, 1, f) <= 0){
				cout<<"BREAK5"<<endl;
				return false;
			}
		}
		else{
			if (verbose) cout<<"read header4 "<<header_length<<endl;
			if (fread(&header, header_length, 1, f) <= 0){
				cout<<"BREAK4"<<endl;
				return false;
			}
		}
		if (!read_header()){
			cerr<<"Could not analyse header"<<endl;
			continue;
		}
		if (!time_header){
			if (!read_waveforms()){
				cerr<<"Cannot read event"<<endl;
				continue;
			}
			if (verbose)cout<<"Found event"<<endl;
			return true;
		}
		else if (verbose) cout<<"Time header -> search for event header"<<endl;
	}
}

void decode(TString filename) {

	TStopwatch  stopwatch;

	// open the binary waveform file
	if (filename.EndsWith(".dat"))
		filename.ReplaceAll(".dat","");
	cout <<"Filename: "<<filename<<endl;
	filename = filename+".dat";
	filename.ReplaceAll("//","/");
	f = fopen64(filename, "r");
	if (!f){
		cout<<"Cannot read file \""<<filename<<"\". Please take a valid file."<<endl;
		return;
	}
	struct stat st;
	stat(filename, &st);

	filename = filename.ReplaceAll(".dat","");
	unsigned long size = st.st_size;
	//open the root file
	outfile = new TFile(filename+".root", "RECREATE");
	map<string,string> results;


	rec= new TTree("rec","rec");
	SetBranches();
	TriggerDelay dt_data(128,"DATA");
	TriggerDelay dt_cali(16,"CALI");
	Int_t n_saved_data = 0;
	Int_t n_saved_cali = 0;
	TGraph* g_data = 0;
	TGraph* g_cali = 0;
	TGraph* g_trig = 0;
	TGraph* g_flag = 0;
	TGraph* g_hits = 0;
	TH1F* h_data = new TH1F("h_data","Integral50_{data}",1001,-500.5,500.5);
	TH1F* h_cali = new TH1F("h_cali","Integral50_{cali}",1001,-500.5,500.5);
	drs_version = GetDRS_revision(f);
	cout<<"DRS4 EvaluationBoard "<<drs_version<<endl;
	n_wf = GetNrecordedWaveforms(f,drs_version);
	long  event_size;
	if (drs_version == 5)
		event_size =  sizeof(header5) + n_wf * sizeof(SingleWaveform_t);
	else
		event_size =  sizeof(header) + n_wf * sizeof(SingleWaveform_t);
	n_events = size/event_size;

	cout<<"Total Size: "<<size<<" Bytes"<<endl;
	cout<<"Event Size: "<<event_size<<" Bytes"<<endl;
	cout<<"N Events:   "<<n_events<<" "<<(float)size/event_size<<endl;

	find_trigger_delay();//f,outfile);
	// loop over all events in data file

	outfile->mkdir("Graphs");
	outfile->cd("Graphs");
	rewind(f);
	int header_length;
	if (drs_version == 5)
		header_length = sizeof(header5);
	else
		header_length = sizeof(header);
	cout<<"START"<<" "<<header_length<<endl;
	for (n=-1 ; true; n++) {
		if (!get_next_event())
			break;
		// decode time

		if (get_trigger_times >=0)
			get_trigger_times();
		// decode amplitudes in mV
		update_averages();

		//Calculate Integrals
		if (n == 0 && verbose)
			cout<<"Find Integral"<<endl;
		if (calibflag){
			GetIntegrals(k,waveform[0].chn,vTrigTimes[0],delay_cali,&Integrals[0]);
			h_cali->Fill(Integrals[0]);
		}
		else{
			GetIntegrals(k,waveform[0].chn,vTrigTimes[0],delay_data,&Integrals[0]);
			h_data->Fill(Integrals[0]);
		}

		//Save waveforms
		if (n == 0 && verbose)
			cout<<"Save Graphs"<<endl;
		if ((n_saved_cali < 20 && calibflag) || (n_saved_data <= 20 && !calibflag)){
			for (Int_t i=0; i<WF_LENGHT; i++) {
				chn1[i] = (Double_t) ((waveform[0].chn[i]) / 65535. - 0.5) * 1000;
				chn2[i] = (Double_t) ((waveform[1].chn[i]) / 65535. - 0.5) * 1000;
				chn3[i] = (Double_t) ((waveform[2].chn[i]) / 65535. - 0.5) * 1000;
				chn4[i] = (Double_t) ((waveform[3].chn[i]) / 65535. - 0.5) * 1000;
				t[i] = (Double_t) header.time[i];
			}
			if (n_wf>2)
				g_trig = new TGraph(1023,t,chn2);
			if (n_wf>3)
				g_hits = new TGraph(1023,t,chn4);

			if (calibflag){
				g_cali = new TGraph(1023,t,chn1);
				g_cali->SetName(TString::Format("g_cali_%d03",n_saved_cali));
				g_cali->SetTitle(TString::Format("Calibration Event %06d",k));
				if (g_cali && !g_cali->IsZombie())
					g_cali->Draw("goffAPL");
				g_cali->SetLineColor(kYellow);
				g_cali->SetLineWidth(3);
				if (g_cali->GetXaxis())
					g_cali->GetXaxis()->SetTitle("time");
				if (g_cali->GetYaxis())
					g_cali->GetYaxis()->SetTitle("signal / mV");
				g_cali->Write();
				if (g_trig)
					g_trig->SetName(TString::Format("g_cali_trig_%d03",n_saved_cali));
				if (g_hits)
					g_hits->SetName(TString::Format("g_cali_hits_%d03",n_saved_cali));
				//cout<<"Saved Cali event "<<n_saved_cali<<"/"<<k<<" "<<g_cali->GetName()<<endl;
				n_saved_cali++;
				c1->Clear();
			}
			else{
				g_data = new TGraph(1023,t,chn1);
				g_data->SetName(TString::Format("g_data_%03d",n_saved_data));
				g_data->SetTitle(TString::Format("Data Event %06d",k));
				if (g_data && !g_data->IsZombie())
					g_data->Draw("goffAPL");
				g_data->SetLineColor(kYellow);
				g_data->SetLineWidth(3);
				if (g_data->GetXaxis())
					g_data->GetXaxis()->SetTitle("time");
				if (g_data->GetYaxis())
					g_data->GetYaxis()->SetTitle("signal_{ch1} / mV");
				g_data->Write();
                if (g_trig)
				g_trig->SetName(TString::Format("g_data_trig_%03d",n_saved_data));
				if (g_hits) g_hits->SetName(TString::Format("g_data_hits_%03d",n_saved_data));
				//cout<<"Saved data event "<<n_saved_data<<"/"<<k<<" "<<g_data->GetName()<<endl;
				n_saved_data++;
				c1->Clear();
			}
			if(g_trig){
				g_trig->SetTitle(TString::Format("Trig Event %06d",k));
				g_trig->Draw("goffAPL");
				g_trig->SetLineColor(kBlue);
				g_trig->SetLineWidth(3);
				g_trig->GetXaxis()->SetTitle("time");
				g_trig->GetYaxis()->SetTitle("signal_{ch2} / mV");
				g_trig->Write();
				c1->Clear();
			}

			if (g_hits) {
				g_hits->SetTitle(TString::Format("Hits Event %06d",k));
				g_hits->Draw("goffAPL");
				g_hits->SetLineColor(kGreen);
				g_hits->SetLineWidth(3);
				g_hits->GetXaxis()->SetTitle("time");
				g_hits->GetYaxis()->SetTitle("signal_{ch4} / mV");
				g_hits->Write();
				c1->Clear();
			}

            if (fixed_readout>=0){
                TString title;
                if (calibflag)
                    title = TString::Format("mgOverview_calib_%06d",n_saved_cali);
                else
                    title = TString::Format("mgOverview_data_%06d",n_saved_data);

                TMultiGraph* mg = new TMultiGraph(title,title);
                if (g_data)
                    mg->Add(g_data);
                if (g_cali)
                    mg->Add(g_cali);
                if (g_trig)
                    mg->Add(g_trig);
                if (g_hits)
                    mg->Add(g_hits);
                mg->Write();
                c1->Clear();
                delete mg;
            }
			g_hits=0;
			g_trig =0;
			g_data = 0;
			g_cali = 0;
		}

		if (verbose)
			cout<<n<<" Fill: "<<rec->GetEntries()<<endl;
		rec->Fill();
	}
	outfile->cd();

	// print number of events
	//  cout<<TString::Format("\r%4d/%4d \t%5.1f",ientry,nentries,(Float_t)ientry/(Float_t)(nentries)*100)<<flush;
	cout<<"\r"<<n<<" events processed"<<endl;
	//cout<<"\""<< filename+".root" <<"\" written"<<endl;

	// save and close root file
	outfile->cd();
	rec->Write();
	TF1* f_cali = new TF1("f_cali","gaus",-500,500);
	h_cali->Fit(f_cali,"Q");

	TF1* f_data;
	Float_t mean = h_data->GetMean();
	Float_t sigma= h_data->GetRMS();
	if (mean > 0)
		f_data = new TF1("f_data","landau",0,500);
	else{
		f_data = new TF1("f_data","[0]*TMath::Landau(-x,[1],[2])",-500,0);
		f_data->SetParLimits(1,-500,500);
		f_data->SetParameter(1,-1*mean);
		f_data->SetParLimits(2,0,500);
		f_data->SetParameter(2,sigma);
		f_data->SetParLimits(0,0,1e6);
		f_data->SetParameter(0,h_data->GetEntries());

	}
	f_data->SetNpx(1000);
	f_data->SetLineWidth(1);
	h_data->Fit(f_data,"Q");
	TF1* f_data2= new TF1("f_data2","gaus",-500,500);
	f_data2->SetLineWidth(1);
	f_data2->SetNpx(1000);
	f_data2->SetLineColor(kGreen);
	Float_t mp = h_data->GetBinCenter(h_data->GetMaximumBin());
	double value = (mean - mp)/mean - 1;
	if ( TMath::Abs(value) > .3)
		mp = mean;
	h_data->Fit(f_data2,"Q+","",mp-sigma/2.,mp+sigma/2.);
	cout<<"RUN "<<filename<<":\n\tDATA: "<<f_data->GetParameter(1)<<"\n\tCALI: "<<f_cali->GetParameter(1)<<endl;

	TString pic_name = filename(filename.Last('/')+1,filename.Length()-filename.Last('/'));
	TCanvas c2("c_"+pic_name);
	if (h_data && !h_data->IsZombie() && h_data->GetEntries()>0)
		h_data->Draw();
	c2.SaveAs("input/h_data_"+pic_name+".png");
	h_data->Write();
	h_cali->Write();
	dt_cali.GetHisto()->Write();
	dt_data.GetHisto()->Write();
	outfile->Close();
	ReadTableFile("results.txt",&results);
	results[(string)filename] = (string)TString::Format("%7d | %3d, %3d | DATA:+%7.2f | %7.2f +/- %7.2f \tCALI: %7.2f +/- %7.2f", n,
			delay_data,
			delay_cali,
			f_data->GetParameter(1),
			f_data2->GetParameter(1),
			f_data2->GetParameter(2),
			f_cali->GetParameter(1),
			f_cali->GetParameter(2));
	WriteTableFile("results.txt",&results);

	//cout << "this is the stopwatch at end\n===============" << std::endl;
	stopwatch.Stop();
	cout << "cpu time:  " << stopwatch.CpuTime() << std::endl;
	//cout << "real time: " << stopwatch.RealTime() << std::endl;
	stopwatch.Reset();
}

void usage(){
	cout<< "usage:\n ./decode -i <filename> [-d DELAY_DATA] [-c DELAY_CONFIG]" << std::endl;
	cout<<"\t-f for fixed readout"<<endl;
}

int main(int argc, char* argv[]){
	verbose = false;
	gROOT->SetBatch();
	c1 = new TCanvas();
	TString infile;
	TString outfile;
	delay_data = -2e3;
	delay_cali = -2e3;
	// Parse options
	char ch;
	while ((ch = getopt(argc, argv, "d:c:i:f:h:v")) != -1 ) {
		switch (ch) {
		case 'i': infile  = TString(optarg);  break;
		case 'h': usage(); return 0; break;
		case 'd': cout<<"Analyse: \""<<optarg<<"\""<<endl;delay_data = atoi(optarg);cout<<"Set delay_data to "<<delay_data<<endl;break;
		case 'c': cout<<"Analyse: \""<<optarg<<"\""<<endl;delay_cali = atoi(optarg);cout<<"Set delay_cali to "<<delay_cali<<endl;break;
		case 'f': cout<<"Analyse: \""<<optarg<<"\""<<endl;fixed_readout = atoi(optarg);cout<<"Set fixed_redaout to "<<fixed_readout<<endl;break;
		case 'v': cout<<"SetVerbose"<<endl;verbose = true;break;
		default:
			cerr << "*** Error: unknown option " << optarg << std::endl;
			usage();
		}
	}

	// Check arguments
	if( argc<1 ) {
		usage();
	}
    if (infile.Contains("S129")){
        TString infile3 = infile(infile.Last('/')+1,infile.Length());
        cout <<infile3<<endl;
        TString infile2 = infile3(5,3);
        cout<<"INFILE2: \""<<infile2<<"\""<<endl;
        int fixed = infile2.Atoi();
        cout<<"FIXED: "<<fixed<<endl;
        if (0<fixed && fixed <1024)
            fixed_readout = fixed;
    }



	//cout<<"calling decode on file " << infile << ".dat" << std::endl;
	decode(infile);
	//cout<<"done. exiting..."<<std::endl;
}
