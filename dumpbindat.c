#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "capreolib.h"


int main(int argc, char* argv[]){

  unsigned char bytearr[NBYTES];  // 5-byte data packet container
  unsigned char data_flag, data_mode; //, module_id;

  int idx; // 
  int rtn; //
  unsigned char buf[45]; // Maxinmum 45 for HK data
  /// HK data
  CMOSHK cmoshk;
  /// Event data
  EVTHEAD evtheader;
  EVTDATA evtdata;
  

  int evthdr_cnt = 0;
  int hkhdr_cnt = 0;
  int evtftr_cnt = 0;
  int event_cnt  = 0;
  int packet_cnt = 0;


  char *binfname = argv[1];

  FILE *fp; //, *fout;

  
  fp = fopen(binfname, "rb");
  if(fp==NULL){
    printf("input file = %s could not be opened\n", binfname);
    return 0;
  }


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

  return 0;
}
