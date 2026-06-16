/*
 * main.cpp
 *
 *  Created on: Jun 14, 2026
 *      Author: hannes
 */

#include "main.h"
#include <stdio.h>

#include "usbd_cdc_if.h"

#include "boot_config.hpp"
#include "sx128x_stm.hpp"

extern "C" int _write(int file, char *ptr, int len) {
    CDC_Transmit_FS((uint8_t*) ptr, len);
    return len;
}
extern "C" int _read(int file, char *ptr, int len)
{
    while (!rx_ready)
    {
        // wait for USB data
    }

    *ptr = rx_char;
    rx_ready = 0;

    return 1;
}

int main()
{
  early_configure_option_bytes();
  init();

  SX1281 sx{SPI1, sx_cs_GPIO_Port, sx_cs_Pin, sx_busy_GPIO_Port, sx_busy_Pin, sx_nreset_GPIO_Port, sx_nreset_Pin,
  sky_cps_GPIO_Port, sky_cps_Pin, sky_csd_GPIO_Port, sky_csd_Pin, sky_ctx_GPIO_Port, sky_ctx_Pin, sky_antsel_GPIO_Port, sky_antsel_Pin};

  sx.begin();
  uint8_t tx_data[8];
  uint8_t rx_data[8];
  while(true){
	  LL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
	  char c = getchar();
	  switch (c){
	  	  case 'd': printf("OK\n"); break;
		  case 'r':{
			  for(int i = 0; i < 5; i++){
				  int r = sx.receive(rx_data);
				  if(r){
					  printf("Received %i with %iDbm: %i\n", r, sx.lastRSSIDbm, rx_data[0]);
				  }
				  else printf("Fail\n");
			  }
			  break;
		  }
		  case '1': tx_data[0] = 1; sx.transmit(tx_data, 8); break;
		  case '2': tx_data[0] = 2; sx.transmit(tx_data, 8); break;
		  case 'b': enter_bootloader(1234); break;
	  }
  }
}

