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
  for (Int_t i=0;i<1024;i++){
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
    if(trigTime<0 ||trigTime>=1024)
        return;
    if (delay == -2e3) delay = 0;
    Int_t pos = trigTime+delay;
//return;
    Int_t integral = 0;
    if (pos<0 || 1024 <= pos)
        return;

    integral += sig[pos];
    int n = 0;
    for (Int_t i = 1; i<= 200; i++){
        if (pos+i>=1024)
            continue;
        if (pos-i<0)
            continue;
        n +=2;
        integral += sig[pos+i];
        integral += sig[pos-i];
        switch (i){
            case 25:
                Integrals[0] = integral/(Double_t)(n+1);
                Integrals[0] = (Double_t) (Integrals[0] / 65535. - 0.5) * 1000;
                break;
            case 50:
                Integrals[1] = integral/(Double_t)(n+1);
                Integrals[1] = (Double_t) (Integrals[1] / 65535. - 0.5) * 1000;
                break;
            case 100:
                Integrals[2] = integral/(Double_t)(n+1);
                Integrals[2] = (Double_t) (Integrals[2] / 65535. - 0.5) * 1000;
                break;
            case 200:
                Integrals[3] = integral/(Double_t)(n+1);
                Integrals[3] = (Double_t) (Integrals[3] / 65535. - 0.5) * 1000;
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
  //rec->Branch("t", &t   ,"t[1024]/D");  
  rec->Branch("n", &k   ,"n/I");  
  // Create branches for the channels
  //rec->Branch("chn1", &chn1 ,"chn1[1024]/D");
  //rec->Branch("chn2", &chn2 ,"chn2[1024]/D");

  rec->Branch("calibflag", &calibflag ,"calibflag/I");
  rec->Branch("delay_data",&delay_data ,"delay_data/I");
  rec->Branch("delay_cali",&delay_cali ,"delay_cali/I");
  rec->Branch("Integral50",&Integrals[0],"Integral50/D");
  rec->Branch("Integral100",&Integrals[1],"Integral100/D");
  rec->Branch("Integral200",&Integrals[2],"Integral200/D");
  rec->Branch("Integral400",&Integrals[3],"Integral400/D");
  rec->Branch("saturated",&saturated,"saturated/O");
  rec->Branch("nTrigs",&nTrigs,"nTrigs/I");
  rec->Branch("trigTime",&trigTime,"trigTime/I");
  rec->Branch("vTrigTimes",&vTrigTimes);
  rec->Branch("vHitTimes",&vHitTimes);
  rec->Branch("nHits",&nHits,"nHits/I");
  rec->Branch("avrg_chn1",&avrg_chn1,"avrg_chn1/F");
  rec->Branch("avrg_chn2",&avrg_chn2,"avrg_chn2/F");
  rec->Branch("avrg_chn3",&avrg_chn3,"avrg_chn3/F");
  rec->Branch("avrg_chn4",&avrg_chn4,"avrg_chn4/F");
  rec->Branch("n_wf",&n_wf,"n_wf/I");
}

int GetNrecordedWaveforms(FILE* f){
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
  rewind(f);
  return n_wf;
}

void decode(TString filename) {

  //cout << "this is the stopwatch at start\n===============" << std::endl;
  TStopwatch  stopwatch;

  // open the binary waveform file
  FILE *f;
  if (filename.EndsWith(".dat"))
    filename.ReplaceAll(".dat","");
  cout <<"Filename: "<<filename<<endl;
  f = fopen64(filename+".dat", "r");
  if (!f){
    cout<<"Cannot read file \""<<filename<<"\". Please take a valid file."<<endl;
    return;
  }
  
  //open the root file
  TFile *outfile = new TFile(filename+".root", "RECREATE");
  map<string,string> results;
  
  rec= new TTree("rec","rec");
  SetBranches();
//  rec->Branch("chn3", &chn3 ,"chn3[1024]/D");
//  rec->Branch("chn4", &chn4 ,"chn4[1024]/D");
  //  cout << "21" << endl;
//  TriggerDelay dt_data(16,"DATA");
//  TriggerDelay dt_cali(1,"CALI");
  TriggerDelay dt_data(128,"DATA");
  TriggerDelay dt_cali(16,"CALI");
  Int_t trig_mean = 0;
  Int_t trig_mean2 = 0;
  Int_t n_trig = 0;
  Int_t n_delay_cali = 0;
  Int_t n_delay_data = 0;
  if(delay_data != -2e3)
    n_delay_data = 100;
  if(delay_cali != -2e3)
    n_delay_cali = 100;
  Int_t n_saved_data = 0;
  Int_t n_saved_cali = 0;
  TGraph* g_data = 0;//new TGraph(1024);
  TGraph* g_cali = 0;//new TGraph(1024);
  TGraph* g_trig = 0;
  TGraph* g_Flag = 0;
  TGraph* g_hits = 0;
  TCanvas* c1= new TCanvas();
  TH1F* h_data = new TH1F("h_data","Integral50_{data}",1001,-500.5,500.5);
  TH1F* h_cali = new TH1F("h_cali","Integral50_{cali}",1001,-500.5,500.5);

  n_wf = GetNrecordedWaveforms(f);

  // find number of active channels
  cout<<"\rFound "<<n_wf<<" Waveforms in the file"<<endl;
  //cout<<"loop"<<endl;
  // loop over all events in data file
  char hdr[] = {'E','H','D','R'};
  outfile->mkdir("Graphs");
  outfile->cd("Graphs");
  for (n=0 ; fread(&header, sizeof(header), 1, f) > 0; n++) {
    //  cout << "30" << endl;
    // decode time     
    if( n == 0 && verbose) 
        cout << "event header  " << header.event_header << std::endl;
    if (memcmp(header.event_header,hdr,4)!=0)
        cout << "event header  " << header.event_header << std::endl;
    if (n%1000 == 0) cout << "\r"<<n << std::flush;

    // get the year/month/date
    year = header.year;
    month = header.month;
    day = header.day;
    time_stamp = header.hour*3600+header.minute*60+header.second+header.millisecond/1000.; 
    if (n == 0 && verbose) cout << "millisecond " << header.millisecond << std::endl;

    if (n == 0 && verbose) cout << "waveform size : " <<    sizeof(waveform)/4*n_channels << endl;
    if (n == 0 && verbose){
        cout <<  "header: "<<header.event_header;
        cout <<"\nserial: "<<header.serial_number;
        cout <<"\nreserv: "<<header.reserved1;
    }

    // print this info for every 10kth event
    if (n%10000 == 0) cout << "\t year "<< year << ": month " << month << ": day " << day << " : hour " << std::setfill(' ')<<std::setw(2)<<header.hour << " : minute " <<std::setfill(' ')<<std::setw(2)<< header.minute << std::flush;;

    // read the waveform
    Int_t s = sizeof(SingleWaveform_t);
    if (verbose && n == 1)
        cout<<n <<" "<<s<<" "<<n_wf<<" "<<s*n_wf<<endl;
    float sizeofwaveform = fread(&waveform, n_wf*s, 1, f);
    if (verbose && n == 1)
        cout << "sizeofwaveform: " << sizeofwaveform << std::endl;
    vector<Int_t> trigTimes = Get1stTriggerTime(-100,waveform.chn2,nTrigs);
    vector<Int_t>  hitTimes = Get1stTriggerTime(-100,waveform.chn4,nHits);
    
    // decode amplitudes in mV
    saturated = false;
    int n_min = 0;
    int n_avrg = 30;
    avrg_chn1 = 0;
    avrg_chn2 = 0;
    avrg_chn3 = 0;
    avrg_chn4 = 0;
    for (Int_t i=0; i<1024; i++) {
      //chn3_int += (Double_t) ((waveform.chn3[i]) / 65535. - 0.5) * 1000;
      if (!saturated && (waveform.chn1[i] >= 65535 || waveform.chn1[i]==0))
          saturated = true;
      if (waveform.chn3[i]<30)
          n_min++;
      if (i <n_avrg){
          avrg_chn1 += waveform.chn1[i];
          avrg_chn2 += waveform.chn2[i];
          avrg_chn3 += waveform.chn3[i];
          avrg_chn4 += waveform.chn4[i];
      }
      
      if (n_min > 30 && saturated && i > n_avrg) 
          break;
    }
    avrg_chn1 /= (Double_t)(n_avrg);
    avrg_chn2 /= (Double_t)(n_avrg);
    avrg_chn3 /= (Double_t)(n_avrg);
    avrg_chn4 /= (Double_t)(n_avrg);
    avrg_chn1 = (Double_t) (avrg_chn1 / 65535. - 0.5) * 1000;
    avrg_chn2 = (Double_t) (avrg_chn2 / 65535. - 0.5) * 1000;
    avrg_chn3 = (Double_t) (avrg_chn3 / 65535. - 0.5) * 1000;
    avrg_chn4 = (Double_t) (avrg_chn4 / 65535. - 0.5) * 1000;


    calibflag = (n_min>10);

    if (n == 1 && verbose)
        cout<<"GetTrigTimes"<<flush;
    vTrigTimes.clear();
    for (int i =0;i < trigTimes.size();i++) vTrigTimes.push_back(trigTimes.at(i));

    vHitTimes.clear();
    for (int i =0;i < hitTimes.size();i++) vHitTimes.push_back(hitTimes.at(i));

    if (nTrigs==0)
      trigTime = -1;
    else{
      trigTime = trigTimes.at(0);
      trig_mean += trigTime;
      trig_mean2+= trigTime*trigTime;
      n_trig++;
    }
      
    if (n == 1 && verbose)
        cout<<"Add Waveforms"<<endl;
    if(calibflag){
        if (n_delay_cali <100){
          dt_cali.AddWaveform(waveform.chn1,1024);
        }
    }
    else{
      if (n_delay_data <100){
            dt_data.AddWaveform(waveform.chn1,1024);
      }
    }

    k++;
    if (n == 0 && verbose)
        cout<<"Find Trig delay"<<endl;
    //find Trigger Delay
    if(n_trig>50&&(n_delay_data<100||n_delay_cali<100)) {
        if (n == 0 && verbose)
            cout<<"Find Trig delay"<<endl;
        Float_t mean = (Float_t)trig_mean/(Float_t)n_trig;
        Float_t mean2 = (Float_t)trig_mean2/(Float_t)n_trig-mean*mean;
        Float_t sigma = mean2>=0?sqrt(mean2):-1;
        if(n_delay_data<100){
            //cout<<mean/sigma<<" Found 1st Trigger: "<<mean<<endl;;
            if (dt_data.GetEvents()>dt_data.GetNAvrg()){
                Int_t delay = (Int_t)dt_data.GetMean()-trig_mean/n_trig;
                //cout<<" "<<dt_data.GetMean()<<" delay: "<<delay<<endl;
                if (delay == delay_data)
                    n_delay_data++;
                else if (fabs(delay-delay_data)>5){
                    if (delay_data != -2e3) cout<<"Lost delay_data "<<delay_data<<" after "<<n_delay_data<<endl;
                    n_delay_data = 0;
                    delay_data = delay;
                }
                if (n_delay_data  == 100){
                    cout<<"\rFound final delay_data: "<<delay_data<<endl;
                    if (delay_data<-50 || delay_data > 200){
                        delay_data = -32;
                        cout<<"Invalid delay_data--> Fix to: "<<delay_data<<endl;
                    }
                        
                }
            }
        }
        if (n_delay_cali<100){
            if (dt_cali.GetEvents()>dt_cali.GetNAvrg()){
                Int_t delay = (Int_t)dt_cali.GetMean()-trig_mean/n_trig;
                if (delay == delay_cali)
                    n_delay_cali++;
                else if(fabs(delay-delay_cali)>5){
                    if (delay_cali !=-2e3) cout<<"Lost delay_cali "<<delay_cali<<" after "<<n_delay_cali<<endl;
                    n_delay_cali = 0;
                    delay_cali = delay;
                }
                if (n_delay_cali  == 100)
                    cout<<"\rFound final delay_cali: "<<delay_cali<<endl;
            }
        }
    }
    if(trigTimes.size()==0){
        if(verbose) cout<<"Cannot Find TrigTime" << endl;
        trigTimes.push_back(DEFAULT_TRIGTIME);
    }
    //Calculate Integrals
    if (n == 0 && verbose)
        cout<<"Find Integral"<<endl;
    if (calibflag){
        GetIntegrals(k,waveform.chn1,trigTimes[0],delay_cali,&Integrals[0]);
        h_cali->Fill(Integrals[0]);
    }
    else{
        GetIntegrals(k,waveform.chn1,trigTimes[0],delay_data,&Integrals[0]);
        h_data->Fill(Integrals[0]);
    }

    //Save waveforms
    if (n == 0 && verbose)
        cout<<"Save Graphs"<<endl;
    if ((n_saved_cali < 20 && calibflag && n_delay_cali==100) || (n_saved_data <= 20 && !calibflag && n_delay_data == 100)){
        for (Int_t i=0; i<1024; i++) {
            chn1[i] = (Double_t) ((waveform.chn1[i]) / 65535. - 0.5) * 1000;
            chn2[i] = (Double_t) ((waveform.chn2[i]) / 65535. - 0.5) * 1000;
            chn3[i] = (Double_t) ((waveform.chn3[i]) / 65535. - 0.5) * 1000;
            chn4[i] = (Double_t) ((waveform.chn4[i]) / 65535. - 0.5) * 1000;
            t[i] = (Double_t) header.time[i];
        }
        g_trig = new TGraph(1023,t,chn2);
        g_hits = new TGraph(1023,t,chn4);

        if (calibflag){
            g_cali = new TGraph(1023,t,chn1);
            g_cali->SetName(TString::Format("g_cali_%d03",n_saved_cali));
            g_cali->SetTitle(TString::Format("Calibration Event %06d",k));
            g_cali->Draw("goffAPL");
            g_cali->GetXaxis()->SetTitle("time");
            g_cali->GetYaxis()->SetTitle("signal / mV");
            g_cali->Write();
            g_trig->SetName(TString::Format("g_cali_trig_%d03",n_saved_cali));
            g_hits->SetName(TString::Format("g_cali_hits_%d03",n_saved_cali));
            //cout<<"Saved Cali event "<<n_saved_cali<<"/"<<k<<" "<<g_cali->GetName()<<endl;
            n_saved_cali++;
            delete g_cali;
        }
        else{
            g_data = new TGraph(1023,t,chn1);
            g_data->SetName(TString::Format("g_data_%03d",n_saved_data));
            g_data->SetTitle(TString::Format("Data Event %06d",k));
            g_data->Draw("goffAPL");
            g_data->GetXaxis()->SetTitle("time");
            g_data->GetYaxis()->SetTitle("signal_{ch1} / mV");
            g_data->Write();
            g_trig->SetName(TString::Format("g_data_trig_%03d",n_saved_data));
            g_hits->SetName(TString::Format("g_data_hits_%03d",n_saved_data));
            //cout<<"Saved data event "<<n_saved_data<<"/"<<k<<" "<<g_data->GetName()<<endl;
            n_saved_data++;
            delete g_data;
        }
        g_trig->SetTitle(TString::Format("Trig Event %06d",k));
        g_trig->Draw("goffAPL");
        g_trig->GetXaxis()->SetTitle("time");
        g_trig->GetYaxis()->SetTitle("signal_{ch2} / mV");
        g_trig->Write();
        delete g_trig;

        g_hits->SetTitle(TString::Format("Hits Event %06d",k));
        g_hits->Draw("goffAPL");
        g_hits->GetXaxis()->SetTitle("time");
        g_hits->GetYaxis()->SetTitle("signal_{ch4} / mV");
        g_hits->Write();
        delete g_hits;
    }

    if (verbose)
      cout<<n<<" Fill"<<endl;
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
  h_data->Fit(f_data,"Q");
  TF1* f_data2= new TF1("f_data2","gaus",-500,500);
  f_data2->SetNpx(1000);
  f_data2->SetLineColor(kGreen);
  Float_t mp = h_data->GetBinCenter(h_data->GetMaximumBin());
  if ( abs((mean - mp)/mean - 1) > .3)
      mp = mean;
  h_data->Fit(f_data2,"Q+","",mp-sigma/2.,mp+sigma/2.);
  cout<<"RUN "<<filename<<":\n\tDATA: "<<f_data->GetParameter(1)<<"\n\tCALI: "<<f_cali->GetParameter(1)<<endl;
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
  cout<< "usage:\n ./decode -i <filename>" << std::endl;
}

int main(int argc, char* argv[]){
   verbose = false;

  TString infile;
  TString outfile;
  delay_data = -2e3;
  delay_cali = -2e3;
  // Parse options
  char ch;
  while ((ch = getopt(argc, argv, "i:h")) != -1 ) {
    switch (ch) {
      case 'i': infile  = TString(optarg);  break;
      case 'h': usage(); break;
      case 'd': delay_data = atoi(optarg);cout<<"Set delay_data to "<<delay_data<<endl;break;
      case 'c': delay_cali = atoi(optarg);cout<<"Set delay_cali t0 "<<delay_cali<<endl;
      default:
      cerr << "*** Error: unknown option " << optarg << std::endl;
      usage();
    }
  }
  
  // Check arguments
  if( argc<1 ) {
    usage();
  }

  //cout<<"calling decode on file " << infile << ".dat" << std::endl;
  decode(infile);
  //cout<<"done. exiting..."<<std::endl;
}
