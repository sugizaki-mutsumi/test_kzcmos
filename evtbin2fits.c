#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fitsio.h"

#include "capreolib.h"



/*--------------------------------------------------------------------------*/
void printerror( int status){
  /*****************************************************/
  /* Print out cfitsio error messages and exit program */
  /*****************************************************/
  if (status) {
    fits_report_error(stderr, status); /* print error report */
    exit( status );    /* terminate the program, returning error status */
  }
  return;
}


int main(int argc, char* argv[]){

  unsigned char bytearr[NBYTES];  // 5-byte data packet container
  unsigned char data_flag, data_mode, module_id;

  int idx; // 
  int rtn; //
  unsigned char buf[45]; // Maxinmum 45 for HK data
  /// HK data
  CMOSHK cmoshk;
  /// Event data
  EVTHEAD evtheader;
  EVTDATA evtdata;
  
  int evthdr_cnt = 0;
  int hkhdr_cnt  = 0;
  int evtftr_cnt = 0;
  int event_cnt  = 0;
  int packet_cnt = 0;



  /// for event fits
  double timedel = 0.1; /// 0.1 sec per frame
  double event_time;
  short xmin=0, xmax=1023;
  short ymin=0, ymax=1023;
  short adumin=0, adumax=4095;
  char *telescop="HIZGUNDAM";
  char *instrume="WXM";


  if(argc<3){
    printf("(Usage) %s (input binary filename) (output fits filename)\n");
    return 0;
  }
  
  
  char *binfname = argv[1];
  char *outfname = argv[2];

  FILE *fp; //, *fout;
  
  fp = fopen(binfname, "rb");
  if(fp==NULL){
    printf("input file = %s could not be opened\n", binfname);
    return 0;
  }

  /// output fits file format
  fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
  int status; //, hdutype;
  long firstrow, firstelem;
  long nrows =0; // default    
  int frameid, frameid_start;
  
  int tfields   = 6;  /* table will have 6 columns */
  /* define the name, datatype, and physical units for the 3 columns */
  char *ttype[] = {"TIME", "X",  "Y",  "ADU",  "FLAG",  "FRAMEID"};
  char *tform[] = {"1D",   "1I", "1I", "1I",   "1B",    "1J"};
  char *tunit[] = {"s",     "pixel", "pixel", "channel;",  "",  ""};
  char *extname = "EVENTS";
  

  status = 0;
  if( fits_create_file(&fptr, outfname, &status) )
    printerror( status );
  
  if ( fits_create_tbl(fptr, BINARY_TBL, nrows, tfields, ttype, tform, tunit, extname, &status) )
    printerror( status );

  
  firstrow  = 1;  /* first row in table to write   */
  firstelem = 1;  /* first element in row  (ignored if it is a scalar column) */
  nrows = 1;
  frameid_start = -1; // default
  
  
  /// process data every 5 bytes
  while( fread(bytearr, sizeof(*bytearr), NBYTES, fp)==NBYTES ) { 
    packet_cnt++;

    data_flag = (bytearr[0] >> 6) & 0x03;
    data_mode = (bytearr[0] >> 4) & 0x03; 
    

#ifdef DEBUG
    printf("%08d: %02x %02x %02x %02x %02x dflag=%d, dmode=%d\n",
	   packet_cnt, bytearr[0], bytearr[1], bytearr[2], bytearr[3], bytearr[4], data_flag, data_mode);
#endif // DEBUG

    if (data_flag == 1 && data_mode==0){ // start HK data process
      strncpy(buf, bytearr, NBYTES);
      for(idx=1; idx<9; idx++){ /// copy 9*NBYTES byetes
	fread(bytearr, sizeof(*bytearr), NBYTES, fp);
	packet_cnt++;
	strncpy(buf+idx*NBYTES, bytearr, NBYTES);
      }
      rtn = read_cmoshk(buf, &cmoshk);
      if(rtn<0) {
	printf("Error in raeding HK data\n");
	break;
      }
      print_cmoshk(&cmoshk);
      hkhdr_cnt++;
      //continue;
    } // End of HK data

    
    if (data_flag==1 && data_mode==1){ // start event data process 
      /// read event header
      strncpy(buf, bytearr, NBYTES);
      for(idx=1; idx<2; idx++){ /// copy 2*NBYTES byetes
	fread(bytearr, sizeof(*bytearr), NBYTES, fp);
	packet_cnt++;
	strncpy(buf+idx*NBYTES, bytearr, NBYTES);
      }
      rtn = read_evtheader(buf, &evtheader);
      if(rtn<0) {
	printf("Error in reading event header!\n");
	break;
      }
      printf("### event data header\n");
      print_evtheader(&evtheader);
      evthdr_cnt++;
      frameid = evtheader.frame_id;
      if(frameid_start <0){// initiallze frameid_start
	frameid_start = frameid;
	module_id = evtheader.module_id;
      }
      event_time = (evtheader.frame_id - frameid_start)*timedel;
      
      /// read event hit
      while( fread(bytearr, sizeof(*bytearr), NBYTES, fp)==NBYTES ) { 
	packet_cnt++;
	data_flag = (bytearr[0] >> 6) & 0x03;
	if( data_flag == 3 ) { // event data
	  rtn = read_evtdata(bytearr, &evtdata);
	  if(rtn<0) break;

	  /// check event data
	  if(event_cnt%10000 == 0) print_evtdata(&evtdata);
	  event_cnt++;

	  if(evthdr_cnt>1){ /// discard 1st event
	    /// output file
	    fits_write_col(fptr, TDOUBLE, 1, firstrow, firstelem, nrows, &event_time, &status);
	    fits_write_col(fptr, TSHORT,  2, firstrow, firstelem, nrows, &evtdata.event_x, &status);
	    fits_write_col(fptr, TSHORT,  3, firstrow, firstelem, nrows, &evtdata.event_y, &status);
	    fits_write_col(fptr, TSHORT,  4, firstrow, firstelem, nrows, &evtdata.adu, &status);
	    fits_write_col(fptr, TBYTE,   5, firstrow, firstelem, nrows, &evtdata.flag, &status);
	    fits_write_col(fptr, TINT,    6, firstrow, firstelem, nrows, &evtheader.frame_id, &status);
	    firstrow++;
	  }

	} else {
	  break;
	}
      } /// End of readinf event hit 
      if(rtn<0){
	printf("Error in reading event data!\n");
	break;
      }
      //// print last event
      print_evtdata(&evtdata);
      
      /// read event footer
      if (data_flag == 2){
	read_evtfooter(buf, &evtheader);
	printf("### event data footer\n");
	print_evtheader(&evtheader);
	evtftr_cnt++;
      } else {
	printf("%08d: Error! data_flag=%d is illegal\n", packet_cnt, data_flag);
	break;
      }
    } /// End of event data 
  }

  printf("packet_cnt = %d\n", packet_cnt);
  printf("evthdr_cnt = %d\n", evthdr_cnt);
  printf("evtftr_cnt = %d\n", evtftr_cnt);
  printf("hkhdr_cnt = %d\n", hkhdr_cnt);
  printf("event_cnt  = %d\n", event_cnt);
  fclose(fp);

  
  //// white header keywords
  fits_write_key(fptr, TSTRING, "TELESCOP", telescop, "Telescope (mission) name", &status);
  fits_write_key(fptr, TSTRING, "INSTRUME", instrume, "Instrument name", &status);
  printf("MODULE_ID = %d\n", module_id);
  fits_write_key(fptr, TBYTE, "MODULEID", &module_id, "MODULE ID", &status);
  fits_write_key(fptr, TDOUBLE, "TIMEDEL", &timedel, "TIMEDEL", &status);

  //
  fits_write_key(fptr, TSHORT, "TLMIN2", &xmin, "Minimum value for X column", &status);
  fits_write_key(fptr, TSHORT, "TLMAX2", &xmax, "Maximum value for X column", &status);
  fits_write_key(fptr, TSHORT, "TLMIN3", &ymin, "Minimum value for Y column", &status);
  fits_write_key(fptr, TSHORT, "TLMAX3", &ymax, "Maximum value for Y column", &status);
  fits_write_key(fptr, TSHORT, "TLMIN4", &adumin, "Minimum value for ADU column", &status);
  fits_write_key(fptr, TSHORT, "TLMAX4", &adumax, "Maximum value for ADU column", &status);
  
  if ( fits_close_file(fptr, &status) )       /* close the FITS file */
    printerror( status );

  return 0;
}
