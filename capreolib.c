#include <stdio.h>
#include <stdlib.h>

#include "capreolib.h"

//#define DEBUG


int read_cmoshk(unsigned char *hkbuf, CMOSHK *pcmoshk){
  unsigned char data_flag;
  unsigned char *bytearr;
  int idx;
  for(idx=0; idx<9; idx++){

    bytearr = hkbuf+idx*NBYTES;
    data_flag = (bytearr[0] >> 6) & 0x03;
#ifdef DEBUG
    printf("## %02d-%02d: %02x %02x %02x %02x %02x dflag=%d\n",
	   idx*NBYTES, (idx+1)*NBYTES-1, bytearr[0], bytearr[1], bytearr[2], bytearr[3], bytearr[4], data_flag);
#endif // DEBUG
    
    switch(idx){
    case 0 :
      pcmoshk->fpga_mode = (bytearr[0] >> 2) & 0x03;
      pcmoshk->time_1  = (u_int32_t)bytearr[1] << 24 | (u_int32_t)bytearr[2] << 16 | (u_int32_t)bytearr[3] << 8| (u_int32_t)bytearr[4];
      printf("  time1 = %d, fpga_mode = %d\n", pcmoshk->time_1, pcmoshk->fpga_mode);
      break;
    case 1:
      pcmoshk->module_id = (bytearr[0] >> 1) & 0x1F;
      pcmoshk->time_2  = (u_int32_t)bytearr[1] << 24 | (u_int32_t)bytearr[2] << 16 | (u_int32_t)bytearr[3] << 8| (u_int32_t)bytearr[4];
      printf("  time2 = %d, module_id = %d\n", pcmoshk->time_2, pcmoshk->module_id);
      break;
    case 2:
      pcmoshk->time_3  = (u_int32_t)bytearr[1] << 24 | (u_int32_t)bytearr[2] << 16 | (u_int32_t)bytearr[3] << 8| (u_int32_t)bytearr[4];
      printf("  time3 = %d\n", pcmoshk->time_3);
      break;
    case 3:
      pcmoshk->frame_ctr = (u_int32_t)(bytearr[1] & 0x7F) << 16 | (u_int32_t)bytearr[2] << 8 | (u_int32_t)bytearr[3];  
      pcmoshk->err_ctr = bytearr[4];
      printf("  frame_ctr = %d, error_ctr = %d\n", pcmoshk->frame_ctr, pcmoshk->err_ctr);
      break;
    case 4:
      pcmoshk->cmd_ctr   = ((uint16_t)bytearr[1] << 8) | ((uint16_t)bytearr[2]);
      pcmoshk->seu       = bytearr[3] & 0x01;
      pcmoshk->ddr_done  = (bytearr[4] >> 3) & 0x01;
      pcmoshk->ledmon    = (bytearr[4] >> 2) & 0x01;
      pcmoshk->ddr_pg    = (bytearr[4] >> 1) & 0x01;
      pcmoshk->ddr_en    = bytearr[4] & 0x01;
      break;
    case 5:
      pcmoshk->temp_fpga = (uint16_t)bytearr[1] << 8 | (uint16_t)bytearr[2];
      pcmoshk->temp_cmos = (uint16_t)bytearr[3] << 8 | (uint16_t)bytearr[4];
      printf("   temp_fpga = %d, temp_cmos = %d\n", pcmoshk->temp_fpga, pcmoshk->temp_cmos);
      break;
    case 6:
      pcmoshk->volt_fpga = (uint16_t)bytearr[1] << 8 | (uint16_t)bytearr[2];
      pcmoshk->volt_cmos = (uint16_t)bytearr[3] << 8 | (uint16_t)bytearr[4];
      printf("   volt_fpga = %d, volt_cmos = %d\n", pcmoshk->volt_fpga, pcmoshk->volt_cmos);
      break;
    case 7:
      pcmoshk->curr_fpga = (uint16_t)bytearr[1] << 8 | (uint16_t)bytearr[2];
      pcmoshk->curr_cmos = (uint16_t)bytearr[3] << 8 | (uint16_t)bytearr[4];
      printf("   curr_fpga = %d, curr_cmos = %d\n", pcmoshk->curr_fpga, pcmoshk->curr_cmos);
      break;
    case 8:
      pcmoshk->crc = (uint16_t)bytearr[3] << 8 | (uint16_t)bytearr[4];
      //printf("   HK_header:%d crc = %d\n", header_idx, crc);
      //header_idx=0; /// End of HK header
      break;
    }
  }
  
  return 1; // OK
}


int print_cmoshk(CMOSHK *p){

  printf("### CMOS/FPGA HK data ###\n");
  printf("fpga_mode  = %d\n", p->fpga_mode);
  printf("module_id  = %d\n", p->module_id);
  printf("time_1     = %d\n", p->time_1   );
  printf("time_2     = %d\n", p->time_2   );
  printf("time_3     = %d\n", p->time_3   );
  printf("frame_ctr  = %d\n", p->frame_ctr);
  printf("err_ctr    = %d\n", p->err_ctr  );
  printf("cmd_ctr    = %d\n", p->cmd_ctr  );
  printf("seu        = %d\n", p->seu	  );
  printf("ddr_done   = %d\n", p->ddr_done );
  printf("ledmon     = %d\n", p->ledmon   );
  printf("ddr_pg     = %d\n", p->ddr_pg   );
  printf("ddr_en     = %d\n", p->ddr_en   );
  printf("temp_cmos  = %d\n", p->temp_cmos);
  printf("temp_fpga  = %d\n", p->temp_fpga);
  printf("volt_cmos  = %d\n", p->volt_cmos);
  printf("volt_fpga  = %d\n", p->volt_fpga);
  printf("curr_cmos  = %d\n", p->curr_cmos);
  printf("curr_fpga  = %d\n", p->curr_fpga);
  printf("crc        = %d\n", p->crc      );

  return 1; // OK
}



int read_evtheader(unsigned char *buf, EVTHEAD *pevthd){
  unsigned char *bytearr = buf;
  unsigned char data_flag;
  int idx;
  for(idx=0; idx<2; idx++){
    bytearr = buf+idx*NBYTES;
    data_flag = (bytearr[0] >> 6) & 0x03;
#ifdef DEBUG
    printf("## %02d-%02d: %02x %02x %02x %02x %02x dflag=%d\n",
	   idx*NBYTES, (idx+1)*NBYTES-1, bytearr[0], bytearr[1], bytearr[2], bytearr[3], bytearr[4], data_flag);
#endif // DEBUG

    switch(idx){
    case 0 :
      pevthd->time_ind = (u_int32_t)bytearr[1] << 24 | (u_int32_t)bytearr[2] << 16 | (u_int32_t)bytearr[3] << 8| (u_int32_t)bytearr[4];
      break;
    case 1:
      pevthd->module_id = (bytearr[0] >> 1) & 0x1F;
      pevthd->frame_id = (u_int32_t)(bytearr[1] & 0x7F) << 16 | (u_int32_t)bytearr[2] << 8 | (u_int32_t)bytearr[3];  
      break;
    }
  }
  
  return 1; // OK
}


int read_evtfooter(unsigned char *buf, EVTHEAD *pevthd){
  unsigned char *bytearr = buf;
  unsigned char data_flag;
  int rtn;
  data_flag = (bytearr[0] >> 6) & 0x03;
  if( data_flag == 3 ) { // event data
#ifdef DEBUG
    printf("## %02d-%02d: %02x %02x %02x %02x %02x dflag=%d\n",
	   0, NBYTES-1, bytearr[0], bytearr[1], bytearr[2], bytearr[3], bytearr[4], data_flag);
#endif // DEBUG
    pevthd->crc    = (uint16_t)bytearr[3] << 8 | (uint16_t)bytearr[4];
    rtn = 1;  // OK
  } else {
    rtn = -1; // Error
  }
  return rtn;
}


int print_evtheader(EVTHEAD *p){  
  /*
  printf("fpga_mode  = %d\n", p->data_mode);
  printf("module_id  = %d\n", p->module_id);
  printf("time_inf   = %d\n", p->time_ind );
  printf("frame_id   = %d\n", p->frame_id );
  printf("crc        = %d\n", p->crc );
  */
  printf("fpga_mode = %d, module_id = %d, time_ind = %d, frame_id = %d, crc = %d\n",
	 p->data_mode, p->module_id, p->time_ind, p->frame_id, p->crc);
  return 1; // OK
}




int read_evtdata(unsigned char *bytearr, EVTDATA *pevt){
  unsigned char data_flag;
  int rtn;
  data_flag = (bytearr[0] >> 6) & 0x03;
  if( data_flag == 3 ) { // event data
    pevt->event_x = (uint16_t)(bytearr[0] & 0x3F) << 5 | (uint16_t)bytearr[1] >> 3;
    pevt->event_y = (uint16_t)(bytearr[1] & 0x07) << 8 | (uint16_t)bytearr[2];
    pevt->adu     = (uint16_t)bytearr[3] << 4 | (uint16_t)bytearr[4] >> 4;
    pevt->flag    = bytearr[4] >> 3 & 0x01;
    rtn = 1;  // OK
  } else {
    rtn = -1; // Error
  }
  return rtn;
}


int print_evtdata(EVTDATA *pevt){
  printf("  event_x=%6d event_y=%6d adu=%4d flag=%d\n", pevt->event_x, pevt->event_y, pevt->adu, pevt->flag);
  return 1; // OK
}
