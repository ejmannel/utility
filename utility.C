#include "../inc/std_headers.h"
#include "../inc/root.h"
#include "../inc/utility.h"


#test


Double_t fluence(Double_t voltage) {

  Double_t c;

  c = 1./(1.6 * 100000000.);

  cout << endl << "Fluence function: " << endl;
  cout << "Input voltage: " << voltage << endl;
  cout << "Conversion Factor: " << c << " " << 1./c << endl << endl;   
  return (voltage/c*1000);
}

Int_t get_biasdata2(Char_t *start_datetime, Char_t *end_datetime,
		   Double_t **time, Double_t **bias) {

  TDatime start_time(start_datetime);
  ULong_t ustart_time = start_time.Convert();
  
  // First get the rad monitoring data

  TString qname, query;

  TSQLServer *db = 
    TSQLServer::Connect("pgsql://phnxdb0.phenix.bnl.gov/daq", "", ""); 
  query = "SELECT read_datetime,current FROM sipmcon";
  query += " WHERE read_datetime <=\'";
  query += start_datetime; 
  query += "\' AND read_datetime <= \'";
  query += end_datetime; 
  query += "\' ORDER by read_datetime ASC";
  
  cout << "SQL query: " << query << endl;

  TSQLResult *res = db->Query( query );
  Int_t nrows = res->GetRowCount();
  *time = new Double_t[nrows];
  *bias = new Double_t[nrows*24];
  
  TSQLRow *row;
  TString values;

  Int_t ipt = 0;
  TObjArray *tx;
  TObjArray ejm(24);

  for (Int_t i = 0; i < nrows; i++) {
    row = res->Next();
    TDatime read_time(row->GetField(0));
    *(*time+ipt) = (Double_t) (read_time.Convert() - ustart_time);
    cout << row->GetField(1) << endl;
    values = Strip(row->GetField(1),'{');
    values.Remove(TString::kTrailing, '}');
    tx = values.Tokenize(",");
    cout << values << endl;
    for (Int_t i = 0; i <= tx->GetLast(); i++) { 
      ejm[i] = *(tx->At(i));
      cout << ejm[i] << endl;
    }

    cout << endl;
    ipt++;
  }

  cout << " counts: " << ipt << endl;
  
  delete db;
  return ipt;
}


Int_t get_raddata(Char_t *start_datetime, Char_t *end_datetime, 
		  Int_t channel, Double_t **time, Double_t **radfet, 
		  Double_t **si, Double_t **temp) {


  TDatime start_time(start_datetime);
  ULong_t ustart_time = start_time.Convert();
  // TDatime stop_time(end_datetime);
  // ULong_t ustop_time = stop_time.Convert();
  
  // First get the rad monitoring data

  TString qname, query;

  TSQLServer *db = TSQLServer::Connect("pgsql://phnxdb0.phenix.bnl.gov/daq", 
                                       "", ""); 
  query = "SELECT * FROM radmon WHERE read_datetime > \'";
  query += start_datetime; 
  query += "\' AND read_datetime < \'";
  query += end_datetime; 
  query += "\' AND channel =  ";
  query += channel;
  query += " ORDER by read_datetime ASC";
  
  cout << "SQL query: " << query << endl;

  TSQLResult *res = db->Query( query );
  Int_t nrows = res->GetRowCount();

//   cout << *time << " " << *radfet << " " << *si << " " << *temp << endl;
  *time = new Double_t[nrows];
  *radfet = new Double_t[nrows];
  *si = new Double_t[nrows];
  *temp = new Double_t[nrows];

  TSQLRow *row;

  Int_t ipt = 0;
  for (Int_t i = 0; i < nrows; i++) {
    row = res->Next();
    
    if (strtod(row->GetField(5), NULL) < 5. &&
	strtod(row->GetField(8), NULL) < 20. &&
	strtod(row->GetField(11), NULL) < 20. &&
	strtod(row->GetField(14), NULL) < 20. &&
	TMath::Abs(strtod(row->GetField(13), NULL)/strtod(row->GetField(12), NULL) - 1.) < 0.01 &&
	TMath::Abs(strtod(row->GetField(4), NULL)/strtod(row->GetField(3), NULL) - 1.) < 0.01) {

      TDatime read_time(row->GetField(1));

      *(*time+ipt) = (Double_t) (read_time.Convert() - ustart_time);
      *(*temp+ipt) = strtod(row->GetField(5), NULL);
      *(*radfet+ipt) = strtod(row->GetField(14), NULL);
      *(*si+ipt) = strtod(row->GetField(11), NULL);

//       cout << "database " << ipt << ": " << row->GetField(1) << "  " <<
// 	*(*time+ipt) << endl;  
      ipt++;

    }
  }

  cout << "Channel " << channel << " counts: " << ipt << endl;
  
  delete db;
  return ipt;
}

Int_t get_zdcdata(Char_t *start_datetime, Char_t *end_datetime, Int_t channel, 
		  Double_t **time, Double_t **zdc) {


  TDatime start_time(start_datetime);
  ULong_t ustart_time = start_time.Convert();
  
  // First get the rad monitoring data

  TString qname, query;

  TSQLServer *db = TSQLServer::Connect("mysql://phnxdb1.phenix.bnl.gov/scalers", 
                                       "phoncs", "phenix7815"); 
  query = "SELECT read_datetime as x, rate1 as y FROM rhicscaler1 WHERE read_datetime > \'";
  query += start_datetime; 
  query += "\' AND read_datetime < \'";
  query += end_datetime; 
  query += "\' ORDER by read_datetime ASC";
  
  cout << "SQL query: " << query << endl;

  TSQLResult *res = db->Query( query );
  Int_t nrows = res->GetRowCount();

  *time = new Double_t[nrows];
  *zdc = new Double_t[nrows];

  TSQLRow *row;

  Int_t ipt = 0;
  for (Int_t i = 0; i < nrows; i++) {
    row = res->Next();
    TDatime read_time(row->GetField(0));
    *(*time+ipt) = (Double_t) (read_time.Convert() - ustart_time);
    *(*zdc+ipt) = strtod(row->GetField(1), NULL);

 //       cout << "database " << ipt << ": " << row->GetField(1) << "  " <<
 // 	*(*time+ipt) << endl;  
    ipt++;
  }

  cout << "Channel " << channel << " counts: " << ipt << endl;
  
  delete db;
  return ipt;
}

Int_t get_biasdata(Char_t *start_datetime, Char_t *end_datetime,
		   Char_t *table, Char_t *channel, Double_t **time, 
		   Double_t **bias, Int_t **sw) {

  TDatime start_time(start_datetime);
  ULong_t ustart_time = start_time.Convert();
  
  // First get the rad monitoring data

  TString qname, query;

  TSQLServer *db = 
    TSQLServer::Connect("pgsql://phnxdb0.phenix.bnl.gov/daq", "", ""); 
  query = "SELECT time,current,switch FROM ";
  query += table;
  query += " WHERE name=\'";
  query += channel;
  query += "\' AND time > \'";
  query += start_datetime; 
  query += "\' AND time < \'";
  query += end_datetime; 
  query += "\' ORDER by time ASC";
  
  cout << "SQL query: " << query << endl;

  TSQLResult *res = db->Query( query );
  Int_t nrows = res->GetRowCount();

  *time = new Double_t[nrows];
  *bias = new Double_t[nrows];
  *sw = new Int_t[nrows];
  
  TSQLRow *row;

  Int_t ipt = 0;
  for (Int_t i = 0; i < nrows; i++) {
    row = res->Next();
    TDatime read_time(row->GetField(0));
    *(*time+ipt) = (Double_t) (read_time.Convert() - ustart_time);
    *(*bias+ipt) = strtod(row->GetField(1), NULL);
    if (!strncmp(row->GetField(2), "On", 2))
      *(*sw+ipt) = 1;
    else
      *(*sw+ipt) = 0;
    //       cout << "database " << ipt << ": " << row->GetField(1) << "  " <<
    // 	*(*time+ipt) << endl;  
    ipt++;
  }

  cout << "Channel " << channel << " counts: " << ipt << endl;
  
  delete db;
  return ipt;
}

Int_t get_magstatus(Char_t *start_datetime) {

  // start_datetime formate yyyy-mm-dd hr:min:sec

  cout << start_datetime << endl;

  TDatime starttime(start_datetime);
  ULong_t ustarttime = starttime.Convert() - 180;

  cout << ustarttime << endl;
  cout << "ustarttime are string: " <<  starttime.AsString() << endl;

  TTimeStamp *t1 = new TTimeStamp(0, 0, 0, 0);
  
  t1->SetSec(ustarttime);

  TString magDateTime = t1->AsString("s");
  Char_t magDate[64], magTime[64];
  sscanf(t1->AsString("s"), "%s %s", magDate, magTime);

  // First get the rad monitoring data

  TString qname, query;

  TSQLServer *db = TSQLServer::Connect("mysql://phnxdb1.phenix.bnl.gov/opc", 
                                       "phoncs", "phenix7815"); 
  query = "SELECT * FROM  NMCMProbe1Slot0AI4 WHERE read_date > \'";
  query += magDate; 
  query += "\' AND read_time > \'";
  query += magTime; 
  query += "\' limit 1";
  
  cout << "SQL query: " << query << endl;

  TSQLResult *res = db->Query( query );
  Int_t nrows = res->GetRowCount();
  
  cout << "nrows: " << nrows << endl;

  Int_t status = -1;
  Float_t hprobe;
  TSQLRow *row;
  for (Int_t i = 0; i < nrows; i++) {
     row = res->Next();
     cout << row->GetField(0) << " " << row->GetField(1) << " ";
     cout << row->GetField(2) << " " << row->GetField(3) << " ";
     cout << endl;
     hprobe = strtod(row->GetField(1), NULL);

     if ((fabs(hprobe) - 0.1) > 0.0)
       status = 0;
     else if ((fabs(hprobe) - 1.39) < 0.1 && hprobe < 0.0) 
       status = 1;
     else if ((fabs(hprobe) - 1.39) < 0.1 && hprobe > 0.0) 
       status = 2;
     else if ((fabs(hprobe) - 1.64) < 0.1 && hprobe < 0.0) 
       status = 1;
     else if ((fabs(hprobe) - 1.64) < 0.1 && hprobe > 0.0) 
       status = 2;
     else if ((fabs(hprobe) - 1.90) < 0.1 && hprobe < 0.0) 
       status = 4;
     else if ((fabs(hprobe) - 1.90) < 0.1 && hprobe > 0.0) 
       status = 5;
     else if (hprobe > -1.30 && hprobe < 1.3) 
       status = 3;
  }

  cout << "Status " << status << endl;
  
  delete db;
  return status;
}

void setupGraph(TGraph *grt, Int_t index,
		Float_t min_tscale, Float_t max_tscale, 
		ULong_t ustart_time, ULong_t ustop_time,
		TString xtitle, TString ytitle) {

  Int_t color[15] = {1, 3, 4, 2, 6, 
		     7, 8, 9, 1, 3, 
                     4, 2, 6, 7, 9 };
  Int_t marker[15] = {2, 2, 2, 2, 2, 
		      2, 2, 2, 3, 3,
		      3, 3, 3, 3, 3};

  grt->GetXaxis()->SetTitle(xtitle);
  grt->GetXaxis()->SetTitleOffset(0.3);
  grt->GetXaxis()->SetLabelSize(0.025);
  grt->GetXaxis()->SetLabelOffset(0.05);
  grt->GetXaxis()->SetNdivisions( -604 );

  if (ustart_time != ustop_time) { 
    grt->GetXaxis()->SetTimeDisplay(1);
    grt->GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}");
    grt->GetXaxis()->SetLimits(0., ustop_time-ustart_time);
  }

  grt->GetYaxis()->SetTitle(ytitle);
  grt->GetYaxis()->SetLabelSize(0.035);
  grt->GetYaxis()->SetLabelOffset(0.01);
  grt->GetYaxis()->SetTitleOffset(1.3);

  grt->SetMinimum(min_tscale);
  if (max_tscale != 0.) grt->SetMaximum(max_tscale);

  grt->SetMarkerSize(0.5 );
  grt->SetMarkerStyle( marker[index] );
  grt->SetMarkerColor(color[index]);
  grt->SetLineColor(color[index]);

  return;
}

void setupMGraph(TMultiGraph *grt, 
		 Float_t min_tscale, Float_t max_tscale, 
		 ULong_t ustart_time, ULong_t ustop_time,
		 TString xtitle, TString ytitle) {

  grt->GetXaxis()->SetTitle(xtitle);
  grt->GetXaxis()->SetTitleOffset(0.3);
  grt->GetXaxis()->SetLabelSize(0.025);
  grt->GetXaxis()->SetLabelOffset(0.05);
  grt->GetXaxis()->SetNdivisions( -604 );
  grt->GetXaxis()->SetTimeDisplay(1);
  grt->GetXaxis()->SetTimeFormat("#splitline{%m/%d/%y}{%H:%M:%S}");
  grt->GetXaxis()->SetLimits(0., ustop_time-ustart_time);
 
  grt->GetYaxis()->SetTitle(ytitle);
  grt->GetYaxis()->SetLabelSize(0.035);
  grt->GetYaxis()->SetLabelOffset(0.01);
  grt->GetYaxis()->SetTitleOffset(1.5);

  if (min_tscale != 0.) grt->SetMinimum(min_tscale);
  if (max_tscale != 0.) grt->SetMaximum(max_tscale);

  return;
}

Double_t temp_corr (Double_t volt, Double_t current, Double_t Tc) {
  // Function returns the correction in Voltag (Volts) for 
  // Tc is the temperature correction in mV/C
  // 1000 ocnverts mV to Volts
  return Tc/1000. * (22.0 - temperature(volt, current));
}

Double_t temperature( Double_t v, Double_t i) {

  const Double_t beta = 3530;
  const Double_t CtoK = 273.15;
  const Double_t r_ntc = 10000.0;

//T_NTC = 1/(1/(t_ntc + CtoK) +1/BETA*(LN((VOLTAGE/2.5)/10000)))-273.15

  // Float_t  a, b, c;
  // a = 1./(25+CtoK);
  // b = 1.0/beta*(TMath::Log((v/i)/r_ntc));
  // c = 1/(a + b) - CtoK;
  // cout << "a: " << a << " b: " << b <<  " c: " << c << endl;

  return ( 1.0/(1.0/(25. + CtoK) + 1.0/beta*(TMath::Log((v/i)/r_ntc))) - CtoK);

} 

Double_t sigma(Double_t **pnts, Double_t cnt) {

  Int_t i;
  Double_t x, xx;
  x = xx = 0.;       

  if (cnt == 0) return(0.);

  for (i = 0; i < cnt; i++) { 
    x += *(*pnts+i);
    xx += *(*pnts+i) * *(*pnts+i);
  }

  return (sqrt(xx/i - x/i*x/i));

}

Double_t mean(Double_t **pnts, Double_t cnt, Double_t ave, Double_t sig) {

  Double_t x = 0.;
  Double_t icnt = 0;

  if (cnt == 0) {
    cout << endl << "Mean Error: cnt = "<< cnt << endl;
    return(0.);
  }

  for (Int_t i = 0; i < cnt; i++) { 
    if( (fabs(*(*pnts+i) - ave) < 2.5 * sig) || (sig == -1.)) { 
      x += *(*pnts+i);
      icnt++;
    }
  }

  if (icnt == 0) {
    cout << endl << "Mean Error: icnt = "<< icnt << endl;
    return(0.);
  }

  return (x/icnt);
 
}

Int_t get_opcdata(Char_t *point_name, Char_t *start_datetime, 
		  Char_t *end_datetime, Double_t **x, Double_t **y) {

  Char_t start_date[32], start_time[32];
  Char_t end_date[32], end_time[32];

  sscanf(start_datetime, "%s %s", start_date, start_time);
  sscanf(end_datetime, "%s %s", end_date, end_time);

  TDatime startTime( start_datetime );
  ULong_t ustartTime = startTime.Convert();

  TSQLServer *db = TSQLServer::Connect("mysql://phnxdb1.phenix.bnl.gov/opc",
				       "phoncs", "phenix7815");
  
  TString query = "SELECT * FROM toc WHERE tagname = \"";
  query += point_name;
  query += "\"";

  cout << "SQL query: " << query << endl;

  TSQLResult *res = db->Query( query );

  Int_t nrows = res->GetRowCount();
  Int_t nfields = res->GetFieldCount();

  // cout << "Query returned " << nrows << " rows" << endl;
  // cout << "Query returned " << nfields << " fields" << endl;

  TSQLRow *row;
  row = res->Next();
 
  Float_t  offset = strtod(row->GetField(3), NULL);
  Float_t  slope = strtod(row->GetField(4), NULL);

  // cout << offset << " " << slope << endl;
		  
  query = "SELECT * FROM  ";
  query += point_name;
  query += " WHERE timestamp(read_date,read_time) >=";
  query += " timestamp(\"";
  query += start_date;
  query += "\",\"";
  query += start_time;
  query += "\") AND timestamp(read_date,read_time) <=";
  query += " timestamp(\"";
  query += end_date;
  query += "\",\"";
  query += end_time; 
  query += "\") order by id asc";

  cout << "SQL query: " << query << endl;

  res = db->Query( query );

  nrows = res->GetRowCount();
  nfields = res->GetFieldCount();
  
  // cout << "Query returned " << nrows << " rows" << endl;
  // cout << "Query returned " << nfields << " fields" << endl;

  *x = new Double_t[nrows];
  *y = new Double_t[nrows];
  
  Int_t ipt = 0;
  Char_t event_datetime[32];

  for (Int_t i = 0; i < nrows; i++) {

    row = res->Next();

    sprintf(event_datetime, "%s %s", row->GetField(3), row->GetField(2));
    TDatime eventTime(event_datetime);

    *(*x+ipt) = (Double_t) ( eventTime.Convert() - ustartTime);
    *(*y+ipt) = offset + strtod( row->GetField(1), NULL ) * slope;
    // cout << "time: " << event_datetime << " " << ustartTime;
    // cout << " " << *(*x+ipt) << endl;
      
    ipt++;
   
  }

  delete db;
  
  return ipt;

}

Int_t get_rundata(Char_t *runType, Char_t *start_datetime, 
		  Char_t *end_datetime, Int_t **x, ULong_t **y,
		  ULong_t **z) {

  Char_t start_date[32], start_time[32];
  // Char_t end_date[32], end_time[32];

  TString query;
  TSQLRow *row;

  sscanf(start_datetime, "%s %s", start_date, start_time);
  // sscanf(end_datetime, "%s %s", end_date, end_time);

  // TDatime startTime( start_datetime );
  // ULong_t ustartTime = startTime.Convert();

  TSQLServer *db = TSQLServer::Connect("pgsql://phnxdb0.phenix.bnl.gov/daq", 
                                       "", ""); 
  query = "SELECT runnumber as x,brtimestamp as y,ertimestamp as z FROM  ";
  query += "run  WHERE brtimestamp >= '";
  query += start_datetime;
  query += "' AND brtimestamp <= '";
  query += end_datetime;
  query += "' and RunType='";
  query += runType;
  query += "' order by runnumber asc";

  cout << "SQL query: " << query << endl;

  TSQLResult *res = db->Query( query );

  Int_t nrows = res->GetRowCount();
  // Int_t nfields = res->GetFieldCount();
  
  *x = new Int_t[nrows];
  *y = new ULong_t[nrows];
  *z = new ULong_t[nrows];
  
  Int_t ipt = 0;

  for (Int_t i = 0; i < nrows; i++) {

    row = res->Next();

    *(*x+ipt) = strtod( row->GetField(0), NULL );      
    TDatime eventBegTime(row->GetField(1));
    *(*y+ipt) = (Double_t) ( eventBegTime.Convert());
    TDatime eventEndTime(row->GetField(2));
    *(*z+ipt) = (Double_t) ( eventEndTime.Convert());

    ipt++;
   
  }

  delete db;
  
  return ipt;

}



