/**
 * ******************************************************************************************************
 *                                             Wireless Device
 *                                               RF project
 *                                               nRF905 File
 *                                               
 *                              (c) Copyright 2011-2025, Li.Guibin, BeiJing, ZH
 *                                            All Right Reserved
 * Date : 9/7/2011
 * File : nRF905.h
 * By   : Li.Guibin
 * ******************************************************************************************************
 */

#ifndef _NRF905_H
#define _NRF905_H

#include <stdint.h>
#include "type.h"

#define CM3LPC17xx           TRUE

#define WC       0x00        /* Write Configuration register command */
#define RC       0x10        /* Read  Configuration register command */
#define WTP      0x20        /* Write Tx Payload command             */
#define RTP      0x21        /* Read  Tx Payload command             */
#define WTA      0x22        /* Write Tx Address command             */
#define RTA      0x23        /* Read  Tx Address command             */
#define RRP      0x24        /* Read  Rx Payload command             */
    

#if (CM3LPC17xx)             /* LPC17xx        */

/**
 * 1. Output - Input
 * P2.3 - TXE
 * P2.4 - CE
 * P2.5 - PW
 * 2. Input - Output
 * P2.8 - CD
 * P2.9 - AM
 * P2.10 - DR
 * 
 * SPI Interface:
 * P0.15 (SCK)
 * P0.16 (SSEL) - pull-up
 * P0.17 (MISO)
 * P0.18 (MOSI)
 * 
 * P2.6 (GPIO) - CSN
 */
                             /* Input          */
#define CD      (1UL << 8)   /* P2.8           */
#define AM      (1UL << 9)   /* P2.9           */
#define DREADY  (1UL << 10)  /* P2.10 DR       */
#define MISO    (1UL << 17)  /* P0.17          */
                             /* Output         */
#define CSN     (1UL << 6)   /* P2.6           */
#define TX_EN   (1UL << 3)   /* P2.3           */
#define TRX_CE  (1UL << 4)   /* P2.4           */
#define PWR_UP  (1UL << 5)   /* P2.5           */
#define MOSI    (1UL << 18)  /* P0.18          */
#define SCK     (1UL << 15)  /* P0.15          */

#else                        /* LPC11xx        */

/**
 * 1. Output - Input
 * P2.4 - TXE
 * P2.5 - CE
 * P2.6 - PW
 * 2. Input - Output
 * P2.7 - CD
 * P2.8 - AM
 * P2.9 - DR
 *
 * SPI Interface:
 * P0.2 (CSN)
 * P2.11 (SCK0)
 * P0.8 (MISO0)
 * P0.9 (MOSI0)
 */
                             /* Input          */
#define CD      (7UL)        /* P2.7           */
#define AM      (8UL)        /* P2.8           */
#define DREADY  (9UL)        /* P2.9 DR        */
#define MISO    (8UL)        /* P0.8           */
                             /* Output         */
#define CSN     (2UL)        /* P0.2           */
#define TX_EN   (4UL)        /* P2.4           */
#define TRX_CE  (5UL)        /* P2.5           */
#define PWR_UP  (6UL)        /* P2.6           */
#define MOSI    (9UL)        /* P0.9           */
#define SCK     (11UL)       /* P2.11          */

#endif

extern volatile uint8_t TxBuf[32], RxBuf[32];

/**
 * <p> Set nRF905 Operating Mode: Radio Enable - ShockBurst TX </p>
 * <table border>
 * <tr><th>PWR_UP</th><th>TRX_CE</th><th>TX_EN</th><th>Operating Mode</th></tr>
 * <tr><td>1</td><td>X</td><td>X</td><td>Power down and SPI programming</td></tr>
 * <tr><td>1</td><td>0</td><td>X</td><td>Standby and SPI programming</td></tr>
 * <tr><td>1</td><td>X</td><td>0</td><td>Read data from Rx Register</td></tr>
 * <tr><td>1</td><td>1</td><td>0</td><td>Radio Enable - ShockBurst RX</td></tr>
 * <tr><td>1</td><td>1</td><td>1</td><td>Radio Enable - ShockBurst TX</td></tr>
 * </table border>
 * @param  void
 * @return void
 */
void    SetTxMode(void);

/**
 * <p> Set nRF905 Operating Mode: Radio Enable - ShockBurst RX </p>
 * <table border>
 * <tr><th>PWR_UP</th><th>TRX_CE</th><th>TX_EN</th><th>Operating Mode</th></tr>
 * <tr><td>1</td><td>X</td><td>X</td><td>Power down and SPI programming</td></tr>
 * <tr><td>1</td><td>0</td><td>X</td><td>Standby and SPI programming</td></tr>
 * <tr><td>1</td><td>X</td><td>0</td><td>Read data from Rx Register</td></tr>
 * <tr><td>1</td><td>1</td><td>0</td><td>Radio Enable - ShockBurst RX</td></tr>
 * <tr><td>1</td><td>1</td><td>1</td><td>Radio Enable - ShockBurst TX</td></tr>
 * </table border>
 * @param  void
 * @return void
 */
void    SetRxMode(void);

/**
 * <p> Fill Tx Data Packet </p>
 * ----------------------------
 * |           |               |
 * |   ADDR    |   PAYLOAD     |
 * |           |               |
 * ----------------------------
 *
 * @param  void
 * @return void
 */
void    TxPacket (void);

/**
 * <p> Read Rx Data when DR and AM is low </p>
 * @param  void
 * @return void
 */
void    RxPacket (void);

/**
 * <p> System Board Init </p>
 * <p> Write nRF905 Register </p>
 * @param  void
 * @return void
 */
void    Config905(void);
                                 /* Delay  */
/**
 * <p> Delay 100 us </p>
 * @param  uint8_t us
 * @return void
 */
void   Delay100us(uint8_t us);   /* 100 us */

/**
 * <p> Delay 1 ms </p>
 * @param  uint32_t ms
 * @return void
 */
void   Delay1ms  (uint32_t ms);  /* 1ms    */


#endif

/**
 * ******************************************************************************************************
 * End Of File
 * ******************************************************************************************************
 */
