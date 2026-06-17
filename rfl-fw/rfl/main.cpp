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
#include "sender.hpp"
#include "receiver.hpp"

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

  uint32_t devid = HAL_GetUIDw0() ^ HAL_GetUIDw1() ^ HAL_GetUIDw2();

  SX1281 sx{SPI1, sx_cs_GPIO_Port, sx_cs_Pin, sx_busy_GPIO_Port, sx_busy_Pin, sx_nreset_GPIO_Port, sx_nreset_Pin,
  sky_cps_GPIO_Port, sky_cps_Pin, sky_csd_GPIO_Port, sky_csd_Pin, sky_ctx_GPIO_Port, sky_ctx_Pin, sky_antsel_GPIO_Port, sky_antsel_Pin};

  sx.begin();

  char mode = 'x';
  Sender s{sx};
  Receiver r{sx};
  uint32_t last_status_ts{};
  while(true){
	  if(rx_ready){
		  LL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
		  char c = getchar();
		  switch (c){
			  case 'd': printf("OK\n"); break;
			  case 'r': mode = 'r'; printf("Mode Receiver\n"); break;
			  case 's': mode = 's'; printf("Mode Sender\n");break;
			  case 'i': printf("Incr\n"); s.channels[0]++; break;
			  case 'b': enter_bootloader(1234); break;
		  }
	  }
	  switch(mode){
	  case 'r':{
		  r.tick();
		  if(HAL_GetTick() - last_status_ts > 1000){last_status_ts = HAL_GetTick(); printf("Rx %i %ims %idBm\n", r.channels[0], (int)r.avg_delta, r.lastRSSI);}
		  break;
	  }
	  case 's': s.tick(); break;
	  default: break;
	  }
  }
}

