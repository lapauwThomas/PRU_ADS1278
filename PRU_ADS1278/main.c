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



//PRU0
#define DATA_IN     ( (uint8_t)( __R31 & 0x000000FF ) )  //bit 7-0
#define nDRDY       ( __R31 & 0x00004000 )  //bit 14
#define SCLK        ( __R30 & 0x00008000 )  //bit 15
#define PPS         ( __R31 & 0x00010000 )  //bit 16

#define SCLK_SET    ( __R30 =  __R30 | 0x00008000 )  //set bit 15
#define SCLK_CLR    ( __R30 =  __R30 & 0xFFFF7FFF )  //clear bit 15
#define SCLK_TOGGLE ( __R30 =  __R30 ^ 0x00008000 )  //toggle bit 15


//PRU1
//#define nDRDY       ( (uint8_t)( __R31 & 0x00000001 ) )  //bit 0
//#define SCLK        ( (uint8_t)( __R31 & 0x00000002 ) )  //bit 1


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

//  Used to make sure the Linux drivers are ready for RPMsg communication
//  Found at linux-x.y.z/include/uapi/linux/virtio_config.h
#define VIRTIO_CONFIG_S_DRIVER_OK 4

//  Buffer used for PRU to ARM communication.
uint8_t payload[480];

int8_t test[2];

#define PRU_SHAREDMEM 0x00010000
volatile register uint32_t __R30;
volatile register uint32_t __R31;

int lastPPsState = 0;

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



  SCLK_CLR;
  uint32_t cCount = 0;

//
 //initCcount();


  payload[0] = 0x41+ (uint8_t)((cCount & 0xFF000000)>>24);
  payload[1] = 0x41+ (uint8_t)((cCount & 0x00FF0000)>>16);
  payload[2] = 0x41+ (uint8_t)((cCount & 0x0000FF00)>>8);
  payload[3] = 0x41+ (uint8_t)((cCount & 0x000000FF));




  payload[0] = 0x41+ (uint8_t)((cCount & 0xFF000000)>>24);
  payload[1] = 0x41+ (uint8_t)((cCount & 0x00FF0000)>>16);
  payload[2] = 0x41+ (uint8_t)((cCount & 0x0000FF00)>>8);
  payload[3] = 0x41+ (uint8_t)((cCount & 0x000000FF));



  enableCcount();

  while (1)
  {

//
//                     while( nDRDY ){
//                             checkPPS();
//                         }
//                         cCount = getCcount();
//
//                    int i = 0;
//                    payload[0] = (uint8_t)((cCount & 0xFF000000)>>24);
//                    payload[1] = (uint8_t)((cCount & 0x00FF0000)>>16);
//                    payload[2] = (uint8_t)((cCount & 0x0000FF00)>>8);
//                    payload[3] = (uint8_t)((cCount & 0x000000FF));
//                    checkPPS();
//                    for ( i = 0; i < 24; i++)  //  Inner single sample loop
//                    {
//                        //cycle clock
//                        SCLK_SET; //clock high for first bit
//
//                        payload[4+i] = DATA_IN; //only 8 lsb to payload
//
//                        SCLK_CLR;
//
//                    }
//
//
//
//                pru_rpmsg_send(&transport, dst, src, payload, (24+4)); //buffer length check
//




          /*
           * Collect 17 samples before packing them into a packet
           */
             int k=0;
           for(k=0;k<17;k++){

               //Wait until dataReady goes low
               while( nDRDY ){
                       checkPPS(); //meanwhile check the PPS signal
                   }
                   cCount = getCcount(); //get the cycle counter value when the sample is collected


              //pack the 32 bits cycle counter value in the payload by splitting into bytes
              payload[28*k+0] = (uint8_t)((cCount & 0xFF000000)>>24);
              payload[28*k+1] = (uint8_t)((cCount & 0x00FF0000)>>16);
              payload[28*k+2] = (uint8_t)((cCount & 0x0000FF00)>>8);
              payload[28*k+3] = (uint8_t)((cCount & 0x000000FF));

              checkPPS();

              int i = 0; //Retrieve the data using the parallel protocol
              for ( i = 0; i < 24; i++)  //  Inner single sample loop
              {
                  SCLK_SET; //clock high for first bit

                  payload[(28*k)+ 4 + i] = DATA_IN; //only 8 lsb to payload

                  SCLK_CLR; //Set clock low

                  checkPPS(); //meanwhile check the PPS signal each time

              }


           }

           //Transfer the packet to the host.
          pru_rpmsg_send(&transport, dst, src, payload,  476);



//

  }

  __halt(); // halt the PRU
}

/*
 * Function to initialize the Pointer register C28 to point to the cycle counter offset
 */
void initCcount(){

   asm volatile ("  LDI32    r0, 0x00022028 \n");
   asm volatile( " LDI32    r26, 0x00022000 \n" );
   asm volatile( " SBBO   &r26, r0, 0, 4 \n" );
   asm volatile( " JMP r3.w2 \n" );
}
/*
 * Function to enable the cycle counter
 */
void enableCcount(){
  asm volatile( " LDI32    r26, 0x00022000 \n" );
    //asm volatile("   LBCO   &r2, C28, 0, 4  \n");
    asm volatile("  LBBO    &r27, r26, 0, 4 \n" );
    asm volatile( " SET    r27, r27.t3 \n" );
    asm volatile(" SBBO   &r27, r26, 0, 4 \n" );
    asm volatile(" JMP r3.w2 \n" );

}

/*
 * Function to get the current value of the cycle counter
 */
uint32_t getCcount(){
    asm volatile( " LDI32    r26, 0x00022000 \n" );
    asm volatile ("   LBBO   &r14, r26, 0xC, 4 ");
    asm volatile("    JMP r3.w2 ");
}

/*
 * Stops the counter, clears the value and starts it again
 */
void clearCcount(){


  asm volatile( " LDI32    r26, 0x00022000 \n" );
  asm volatile("  LBBO    &r27, r26, 0, 4 \n" );
  asm volatile( " CLR    r27, r27.t3 \n" );
  asm volatile(" SBBO   &r27, r26, 0, 4 \n" );

  asm volatile( " LDI32    r28, 0 \n" );
  asm volatile ("   SBBO   &r28, r26, 0xC, 4 ");

  asm volatile("  LBBO    &r27, r26, 0, 4 \n" );
  asm volatile( " SET    r27, r27.t3 \n" );
  asm volatile(" SBBO   &r27, r26, 0, 4 \n" );
  asm volatile("    JMP r3.w2 ");
}

/*
 * Function to check the rising edge of the PPS input, and to clear the cycle counter on the rising edge
 */
void checkPPS(){
    if( PPS > 0 && lastPPsState == 0 ){ //rising edge detected
            clearCcount();
    }
    lastPPsState = PPS;
    return;
}




