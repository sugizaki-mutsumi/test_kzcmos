#ifndef _CAPREOLIB_
#define _CAPREOLIB_

#ifdef __cplusplus
extern "C"
{
#endif


#define  NBYTES  5   /* number of bytes in reading data packet  */

typedef struct cmoshk {
  unsigned char fpga_mode;
  unsigned char module_id;
  int time_1;
  int time_2;
  int time_3;
  int frame_ctr;
  unsigned char  err_ctr;
  unsigned short cmd_ctr;
  unsigned char seu;
  unsigned char ddr_done;
  unsigned char ledmon;
  unsigned char ddr_pg;
  unsigned char ddr_en;
  unsigned short temp_cmos;
  unsigned short temp_fpga;
  unsigned short volt_cmos;
  unsigned short volt_fpga;
  unsigned short curr_cmos;
  unsigned short curr_fpga;
  unsigned short crc;
} CMOSHK;


int read_cmoshk(unsigned char *hkbuf, CMOSHK *pcomshk);
int print_cmoshk(CMOSHK *pcomshk);



typedef struct evtheader {
  unsigned char  data_mode;
  unsigned char  module_id;
  unsigned int   time_ind;
  unsigned int   frame_id; 
  unsigned short crc;
} EVTHEAD;

int read_evtheader(unsigned char *buf, EVTHEAD *pevthd);
int read_evtfooter(unsigned char *buf, EVTHEAD *pevthd);
int print_evtheader(EVTHEAD *pevthd);



typedef struct evtdata {
  unsigned short event_x;
  unsigned short event_y;  
  unsigned short adu;       
  unsigned char  flag;  
} EVTDATA;
 
int read_evtdata(unsigned char *evtbuf, EVTDATA *pevt);
int print_evtdata(EVTDATA *pevt);


#ifdef __cplusplus
}
#endif

#endif  
