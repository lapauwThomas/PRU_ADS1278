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

//#include "CC_count.asm"

//extern "C" {
//extern uint32_t getCcount(); /* declare external asm function */
//}
//
//extern "C" {
//extern void enableCcount(); /* declare external asm function */
//}
//
//extern "C" {
//extern void disableCcount(); /* declare external asm function */
//}
//
//extern "C" {
//extern void clearCcount(); /* declare external asm function */
//}
//
//extern "C" {
//extern void initCcount(); /* declare external asm function */
//}


#define PRU_SHAREDMEM 0x00010000
volatile register uint32_t __R30;
volatile register uint32_t __R31;

//PRU0
#define DATA_IN     ( (uint8_t)( __R31 & 0x000000FF ) )  //bit 7-0
#define nDRDY       ( __R31 & 0x00004000 )  //bit 14
#define SCLK        ( __R30 & 0x00008000 )  //bit 15

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








int main(void) {

  struct pru_rpmsg_transport transport;
  uint16_t src, dst, len;
  volatile uint8_t *status;
  uint8_t payload[256];
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
         PRU_RPMSG_SUCCESS) {  }

  initCcount();
//  enableCcount();
//
//
//  uint32_t cCount = getCcount();
//  uint8_t test[4];
//  test[0] = (uint8_t)((cCount & 0xFF000000)>>24);
//  test[1] = (uint8_t)((cCount & 0x00FF0000)>>16);
//  test[2] = (uint8_t)((cCount & 0x0000FF00)>>8);
//  test[3] = (uint8_t)((cCount & 0x000000FF));
//  pru_rpmsg_send(&transport, dst, src, test, 4); //buffer length check
//
//  __delay_cycles(10000);
//
  uint32_t cCount = getCcount();
//
//  test[0] = (uint8_t)((cCount & 0xFF000000)>>24);
//  test[1] = (uint8_t)((cCount & 0x00FF0000)>>16);
//  test[2] = (uint8_t)((cCount & 0x0000FF00)>>8);
//  test[3] = (uint8_t)((cCount & 0x000000FF));
//  pru_rpmsg_send(&transport, dst, src, test, 4); //buffer length check

  int count=0;
//  uint32_t stampLSB=0;
//  uint32_t stampMSB=0;


  while (1)
  {
      int k=0;
      for(k=0;k<17;k++){

         while( nDRDY );

         int i = 0;
         payload[24*k+0] = (uint8_t)((count & 0xFF000000)>>24);
         payload[24*k+1] = (uint8_t)((count & 0x00FF0000)>>16);
         payload[24*k+2] = (uint8_t)((count & 0x0000FF00)>>8);
         payload[24*k+3] = (uint8_t)((count & 0x000000FF));
         for ( i = 0; i < 24; i++)  //  Inner single sample loop
         {
             //cycle clock
             SCLK_SET; //clock high for first bit

             payload[24*k+4+i] = DATA_IN; //only 8 lsb to payload

             SCLK_CLR;

         }
         count++;

      }

     pru_rpmsg_send(&transport, dst, src, payload, (24+4)*17); //buffer length check


  }

  __halt(); // halt the PRU
}




uint32_t getCcount(){

    asm("   LBCO   &r14, C28, 0xC, 4 ");
    asm("    JMP r3.w2 ");
}
//
//void enableCcount(){
//    __asm__ __volatile__
//    (
//    " LBCO   &r2, C28, 0, 4  \n"
//    " SET    r2, r2.t3 \n"
//    " SBCO   &r2, C28, 0, 4 \n"
//    " JMP r3.w2 \n"
//             );
//}
//
//
//void disableCcount(){
//    __asm__ __volatile__
//    (
//    " LBCO   &r2, C28, 0, 4  \n"
//    " CLR    r2, r2.t3 \n"
//    " SBCO   &r2, C28, 0, 4 \n"
//    " JMP r3.w2 \n"
//             );
//}
//
//
////
////.global disableCcount
////disableCcount:
////LBCO   &r2, C28, 0, 4
////CLR    r2, r2.t3
////SBCO   &r2, C28, 0, 4
////JMP r3.w2
////
//
//void clearCcount(){
//
//__asm__ __volatile__
//    (
//    " LBCO   &r2, C28, 0, 4  \n"
//    " CLR   r2, r2.t3 \n"
//    " SBCO   &r2, C28, 0, 4 \n"
//    " LBCO   &r2, C28, 0, 4 \n"
//    " LBCO   &r2, C28, 0, 4  \n"
//    " SET    r2, r2.t3 \n"
//    " SBCO   &r2, C28, 0, 4 \n"
//    " JMP r3.w2 \n"
//             );
//
//}
//
void initCcount(){

   asm( " LDI32    r0, 0x22028  \n" );
   asm( " LDI32    r1, 0x00000220 \n" );
   asm( " SBBO   &r1, r0, 0, 4 \n" );
   asm( " JMP r3.w2 \n" );

//}

//.global clearCcount
//clearCcount:
//LBCO   &r2, C28, 0, 4
//SET    r2, r2.t3
//SBCO   &r2, C28, 0, 4
//LBCO   &r2, C28, 0, 4
//SBCO   &r2, C28, 0xC, 4
//LBCO   &r2, C28, 0, 4
//SET    r2, r2.t3
//SBCO   &r2, C28, 0, 4
//JMP r3.w2
//
//.global initCcount
//initCcount:
//LDI32    r0, 0x22028
//LDI32    r1, 0x00000220
//SBBO   &r1, r0, 0, 4
//JMP r3.w2

