#include <iostream>
#include "stdio.h"
#include "stdlib.h"

#include "TStyle.h"
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TString.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TDatime.h"
#include "TLegend.h"
#include "TFile.h"
#include "TMultiGraph.h"
#include "TCanvas.h"

using namespace std;

// dates expected as mm-dd-yyyy


Int_t runData(Char_t *start_datetime, Char_t *end_datetime )
{
  char date_field[128];
  char sdate[32];
  char edate[32];

  Int_t i, mon, day, yr;

  TSQLRow *row;

  // convert date to yyyy-mm-dd format
  sscanf(start_datetime, "%d-%d-%d", &mon, &day, &yr);
  sprintf(sdate, "%d-%2.2d-%2.2d 00:00:00", yr, mon, day);

  sscanf(end_datetime, "%d-%d-%d", &mon, &day, &yr);
  sprintf(edate, "%d-%2.2d-%2.2d 00:00:00", yr, mon, day);

  TDatime start_time(sdate);
  ULong_t ustart_time = start_time.Convert();

  gStyle->SetTimeOffset(ustart_time + 6. * 3600.);
  TSQLServer *db = TSQLServer::Connect("pgsql://phnxdb1.phenix.bnl.gov/daq", "", "");

  TString query = "SELECT runtype as x, brtimestamp as y , ertimestamp as z  from run ";
  query += "where brtimestamp > \'";
  query += sdate;
  query += "\ '";
  query += "and ertimestamp < \'";
  query += edate;
  query += "\'";

  cout << "query: " << query << endl;

  TSQLResult *res1 = db->Query( query ); 
  Int_t nrows = res1->GetRowCount();
  Int_t nfields = res1->GetFieldCount();
  
  cout << "number of rows: " << nrows << endl;
  cout << "number of fields: " << nfields << endl;
  
  for ( i = 0; i < nrows; i++) {

    row = res1->Next();

    if (!strncmp(row->GetField(0), "PHYSICS", 7) || 
	!strncmp(row->GetField(0), "JUNK", 4) )
	cout << row->GetField(0) << endl;
  }

 
  delete db;

  
  printf("done!\n");

  return 0;

}
