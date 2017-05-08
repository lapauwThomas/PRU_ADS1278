//  pru0adc.c
//  Attempt to duplicate Derek Molloy's
//  SPI ADC read program in C from assembly.
//  Chip Select:  P9.27 pr1_pru0_pru_r30_5
//  MOSI:         P9.29 pr1_pru0_pru_r30_1
//  MISO:         P9.28 pr1_pru0_pru_r31_3
//  SPI CLK:      P9.30 pr1_pru0_pru_r30_2
//  Sample Clock: P8.46 pr1_pru1_pru_r30_1  (testing only)
//  Copyright (C) 2016  Gregory Raven
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "resource_table_0.h"
#include <am335x/pru_cfg.h>
#include <am335x/pru_intc.h>
#include <pru_rpmsg.h>
#include <rsc_types.h>
#include <stdint.h>
#include <stdio.h>

// Define remoteproc related variables.
#define HOST_INT ((uint32_t)1 << 30)

//  The PRU-ICSS system events used for RPMsg are defined in the Linux device
//  tree.
//  PRU0 uses system event 16 (to ARM) and 17 (from ARM)
//  PRU1 uses system event 18 (to ARM) and 19 (from ARM)
#define TO_ARM_HOST 16
#define FROM_ARM_HOST 17

//  Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
//  at linux-x.y.x/drivers/rpmsg_pru.c
#define CHAN_NAME "rpmsg-pru"
#define CHAN_DESC "Channel 30"
#define CHAN_PORT 30
#define PULSEWIDTH 300

//  Used to make sure the Linux drivers are ready for RPMsg communication
//  Found at linux-x.y.z/include/uapi/linux/virtio_config.h
#define VIRTIO_CONFIG_S_DRIVER_OK 4

//  Buffer used for PRU to ARM communication.
int8_t payload[256];

#define PRU_SHAREDMEM 0x00010000
volatile register uint32_t __R30;
volatile register uint32_t __R31;
uint32_t spiCommand;

int main(void) {
  struct pru_rpmsg_transport transport;
  uint16_t src, dst, len;
  volatile uint8_t *status;

  //  1.  Enable OCP Master Port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
  //  Clear the status of PRU-ICSS system event that the ARM will use to 'kick'
  //  us.
  CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

  //  Make sure the drivers are ready for RPMsg communication:
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK))
    ;

  //  Initialize pru_virtqueue corresponding to vring0 (PRU to ARM Host
  //  direction).
  pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0,
                 &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

  // Create the RPMsg channel between the PRU and the ARM user space using the
  // transport structure.
  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC,
                           CHAN_PORT) != PRU_RPMSG_SUCCESS)
    ;
  //  The above code should cause an RPMsg character to device to appear in the
  //  directory /dev.
  //  The following is a test loop.  Comment this out for normal operation.

  //  This section of code blocks until a message is received from ARM.
  while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) !=
         PRU_RPMSG_SUCCESS) {
  }
  /* Priming the 'hostbuffer' with a message */
  uint8_t hello[] = "hello \n";
  pru_rpmsg_send(&transport, dst, src, hello, 10); //buffer length check

  while (1) {

      // wait for data Ready low
      while ((__R31 & (1<<14))) { //while bit 14 is set
          __R30 ^= (1<<14); //  togle PRU 30_14
      }
      __R30 = __R30 & ~(1<<15); //  Clock to low
      int i = 0;
      for ( i = 0; i < 24; i = i + 1) { //  Inner single sample loop

          //cycle clock

          __R30 = __R30 | (1<<15); //clock high for first bit
        //  payload[i] = __R31 & 0x000F; //only 8 lsb to payload
          payload[i] = (uint8_t) (0x41 + (__R31 & 0xFF)); //only 8 lsb to payload
          __R30 = __R30 & ~(1<<15); //  Clock to low
          }
      __R30 = __R30 | (1<<15); //clock high for first bit
     // payload[i] = (uint8_t)"\n";
      //send data to host
 //     uint8_t hello[] = "hello \n";
 //     pru_rpmsg_send(&transport, dst, src, hello, 10); //buffer length check
      pru_rpmsg_send(&transport, dst, src, payload, 24); //buffer length check

  }

  __halt(); // halt the PRU
}
