

#include "TriggerDelay.hh"

TriggerDelay::TriggerDelay(Int_t n_avrg,TString name){
  //cout<<"Init TriggerDelay::TriggerDelay "<<n_avrg<<endl;
  this->n_avrg = n_avrg;
  nSigs = 0;
  savedSigs.resize(n_avrg);
  mean = 0;
  sigma = 0;
  n = 0;
  this->name = name;
  h_max_signal_time_vs_ampitude = new TH2D((TString)"h_max_signal_time_vs_ampitude_"+name,(TString)"Max signal time vs ampitude "+ name,2048,-1024,1024,1200,-600,600);
  First_trigger_time = 0;
  final_delay = -2e3;
}


TriggerDelay::~TriggerDelay(){};


void TriggerDelay::AddWaveform(unsigned short wf[],UInt_t size){
    bool remove = nSigs>=n_avrg;
    unsigned short max = 0;
    unsigned short min = 65535;
    //short min_t = -1;
    //short max_t = -1;
    Int_t pos = nSigs%n_avrg;
    if (savedSigs[pos].size() < size)
      savedSigs[nSigs%n_avrg].resize(size,0);
    if (avrgSig.size() < size)
      avrgSig.resize(size,0);
    for (UInt_t i = 0; i<size;i++){
      //if (wf[i]>max){
      //    max = wf[i];
      //    max_t = i;
      //}
      //if (wf[i]<min){
      //    min = wf[i];
      //    min_t = i;
      //}
      avrgSig[i] += wf[i];
      if (remove) 
        avrgSig[i] -= savedSigs[pos][i];
      savedSigs[pos][i] = wf[i];
    }
    if (savedSigs[pos].size() > size)
      savedSigs[nSigs%n_avrg].resize(size,0);
    nSigs++;
    //cout<<"AddedWaveform "<<nSigs<<endl;
    //cout<<"Max: "<<max_t<<"/"<<max<<"\tMin: "<<min_t<<"/"<<min<<endl;
    if (nSigs>=n_avrg && nSigs%n_avrg==0)
      AnalyzeWaveform();
};

Int_t TriggerDelay::check_delay(Float_t mean) {
	if (GetEvents()<GetNAvrg())
		return -2e3;
	Int_t delay = (Int_t)GetMean()-mean;
//	cout<<" delay: "<<delay<<" "<<final_delay<<flush;
	if (delay == final_delay){
		n_delay++;
		n_delay_off = 0;
	}
	else if(fabs(delay-final_delay)>5 || n_delay_off>30){
		if (final_delay !=-2e3)
			cout<<"\nLost delay_"<<name<<":"<<final_delay<<" after "<<n_delay<<endl;
		n_delay = 0;
		final_delay = delay;
		n_delay_off = 0;
	}
	else{
		n_delay_off +=1;
	}
	if (n_delay  == 100){
		cout<<"\nFound final delay_"<<name<<": "<<final_delay<<"   "<<endl;
		return final_delay;
	}
	return -2e3;
}

void TriggerDelay::AnalyzeWaveform(){
  //cout<<name<<"::AnalyzeWaveform()"<<endl;

  Int_t max_signal_time;
 
  Double_t Max_signal = FindSignalMax(1,1022,&avrgSig[0],n_avrg,max_signal_time);
  if (First_trigger_time)
    h_max_signal_time_vs_ampitude->Fill(max_signal_time-*First_trigger_time, Max_signal);
  else
    h_max_signal_time_vs_ampitude->Fill(max_signal_time, Max_signal);
  mean+= max_signal_time;
  sigma+= max_signal_time*max_signal_time;
  n++;

}
//int feld[]
Double_t TriggerDelay::FindSignalMax(Int_t first, Int_t last, unsigned long sig[], Int_t nAvrg, Int_t & max_signal_time) {
  if(!(first>=0 && first<last && last<=1024))
    return -9999999;
  if (name == "CAL") cout<<"\nFind Max: ";
  Float_t max = sig[first]/n_avrg;
  if (name == "CAL") cout<<max;;
  max = fabs(max - 32768.);
  max_signal_time = first;
  if (name == "CAL") cout<<" "<<max<<"\n";;
  for(Int_t i = first; i<last;i++){
    Float_t val = sig[i]/n_avrg;
    if (name == "CAL") cout<<i<<"/"<<val;
    val = fabs(val-32768);
    if (name == "CAL") cout<<"/"<<val<<"/"<<max<<" ";;
    if(val>fabs(max)) {
      max=val;
      max_signal_time = i;
      if (name == "CAL") cout<<" new max"<<max<<" "<<max_signal_time<<" ";;
    }
  }
  if (name == "CAL") cout<<flush;;
  max/=nAvrg;
  //cout<<name<<"::get max signal "<<first<<" "<<last<<" "<<max<<" "<<max_signal_time<<endl;
  return max;
}

Int_t TriggerDelay::GetMean() {return n==0?-1:(Int_t)(mean/n+.5);};

Double_t TriggerDelay::GetRMS() {Double_t retVal = n==0?-1:sigma-mean/n*mean/n; return retVal>=0?sqrt(retVal):retVal;}

TGraph* TriggerDelay::GetAverageGraph(){
	Int_t n = avrgSig.size();
	Double_t x[n];
	Double_t y[n];
	for (UInt_t i = 0; i < n; i++){
		x[i] = i;
		y[i] = ((Float_t)avrgSig[i]/n_avrg / 65535. - 0.5) * 1000;
	}
	TGraph* g = new TGraph(n,x,y);
	TString gName = "gAvrgSignal_"+name;
	g->SetName(gName);
	g->SetTitle(gName);
	g->Draw("goff");
	g->GetXaxis()->SetTitle("waveform point");
	g->GetYaxis()->SetTitle("avrg Signal "+ name);
	g->SetLineWidth(3);
	if (name.Contains("CAL") || name.Contains("cal"))
		g->SetLineColor(kBlue);
	else if(name.Contains("TRIG") || name.Contains("trig"))
		g->SetLineColor(kRed);
	else
		g->SetLineColor(kYellow);
	return g;

}
    
