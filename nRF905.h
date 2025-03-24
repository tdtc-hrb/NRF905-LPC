/**
 * ******************************************************************************************************
 *                                             Wireless Device
 *                                               RF project
 *                                               nRF905 File
 *                                               
 *                              (c) Copyright 2011-2025, Li.Guibin, BeiJing, ZH
 *                                            All Right Reserved
 * Date : 9/18/2011
 * File : nRF905.h
 * By   : Li.Guibin
 * ******************************************************************************************************
 */

#ifndef _NRF905_H
#define _NRF905_H

#include <stdint.h>
#include "../../app/type.h"

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

/**
 * <p> RF - Configuration Register Description </p>
 * @param CH_NO 
 *        Bitwidth:    9
 *        Description: Sets center frequency together with HFREQ_PLL  (default = 001101100[b] = 108[b]
 *                     f[RF] = (422.4 + CH_NO[d]/10) * (1 + HFREQ_PLL[d])MHz
 * @param HFREQ_PLL
 *        Bitwidth:    1
 *        Description: Sets PLL in 433 or 868/915MHz mode(default = 0).
 *                     '0' - Chip operating in 433MHz band
 *                     '1' - Chip operating in 868 or 915 MHz band
 * @param PA_PWR
 *        Bitwidth:    2
 *        Description: Output power (default - 00).
 *                     '00' -10dBm
 *                     '01' -2 dBm
 *                     '10' +6 dBm
 *                     '11' +10dBM
 * @param RX_RED_PWR
 *        Bitwidth:    1
 *        Description: Reduces current in Rx mode by 1.6mA. Sensitivity is reduced (default = 0).
 *                     '0' - Normal operation
 *                     '1' - Reduced power
 * @param AUTO_RETRAN
 *        Bitwidth:    1
 *        Description: Retransmit contents in Tx register if TRX_CE and TXEN are high (default = 0).
 *                     '0' - No retransmission
 *                     '1' - Retransmission of data packet
 * @param RX_AFW
 *        Bitwidth:    3
 *        Description: Rx-address width (default = 100).
 *                     '001' - 1 byte Rx address field width
 *                     '100' - 4 byte Rx address field width
 * @param TX_AFW
 *        Bitwidth:    3
 *        Description: Tx-address width (default = 100).
 *                     '001' - 1 byte Tx address field width
 *                     '100' - 4 byte Tx address field width
 * @param RX_PW
 *        Bitwidth:    6
 *        Description: Rx-payload width (default = 100000).
 *                     '000001' - 1 byte Rx payload field width
 *                     '000010' - 2 byte Rx payload field width
 *                         .
 *                     '100000' - 32 byte Rx payload field width
 * @param TX_PW
 *        Bitwidth:    6
 *        Description: Tx-payload withd (default = 100000).
 *                     '000001' - 1 byte Tx payload field width
 *                     '000010' - 2 byte Tx payload field width
 *                         .
 *                     '100000' - 32 byte Tx payload field width
 * @param RX_ADDRESS
 *        Bitwidth:    32
 *        Description: Rx address identity. Used bytes depend on RX_AFW (default = E7E7E7E7[h]).
 * @param UP_CLK_FREQ
 *        Bitwidth:    2
 *        Description: Output clock frequency (default = 11).
 *                     '00' - 4MHz
 *                     '01' - 2MHz
 *                     '10' - 1MHz
 *                     '11' - 500KHz
 * @param UP_CLK_EN
 *        Bitwidth:    1
 *        Description: Output clock enable (default = 1).
 *                     '0' - No external clock signal available
 *                     '1' - External clock signal enabled
 * @param XOF
 *        Bitwidth:    3
 *        Description: Crystal oscillator frequency. Must be set according to 
 *                     external crystal resonant frequency (default = 100).
 *                     '000' - 4MHz
 *                     '001' - 8MHz
 *                     '010' - 12MHz
 *                     '011' - 16MHz
 *                     '100' - 20MHz
 * @param CRC_EN
 *        Bitwidth:    1
 *        Description: CRC - check enable (default = 1).
 *                     '0' - Disable
 *                     '1' - Enable
 * 
 * @param CRC_MODE
 *        Bitwidth:    1
 *        Description: CRC - mode (default = 1).
 *                     '0' - 8 CRC check bit
 *                     '1' - 16 CRC check bit
 */

/*-------------------------------------------------------------------------------------------
nrf905配置寄存器宏定义 author:HotPower
        nrf905配置寄存器(10Byte)
工作频率f=(422.4+CH_NO/10)*(1+HFREQ_PLL)MHz
-------------------------------------------------------------------------------------------*/
#define RX_ADDRESS           0x12345678                //接收有效地址(本方)
#define TX_ADDRESS           0x12345678                //发送有效地址(对方)

#define CH_NO_FREQ_422_4MHz  0x000                     //工作频率422.4MHz(433MHz频段最低频率)
#define CH_NO_FREQ_422_5MHz  0x001                     //工作频率422.5MHz
#define CH_NO_FREQ_425_0MHz  0x01a                     //工作频率425.0MHz
#define CH_NO_FREQ_427_5MHz  0x033                     //工作频率427.5MHz

#define CH_NO_FREQ_430_0MHz  0x04c                     //工作频率430.0MHz
#define CH_NO_FREQ_433_0MHz  0x06a                     //工作频率433.0MHz(433MHz频段基准频率)
#define CH_NO_FREQ_433_1MHz  0x06b                     //工作频率433.1MHz
#define CH_NO_FREQ_433_2MHz  0x06c                     //工作频率433.2MHz
#define CH_NO_FREQ_434_7MHz  0x07b                     //工作频率434.7MHz
#define CH_NO_FREQ_473_5MHz  0x1ff                     //工作频率473.5MHz(433MHz频段最高频率)

#define CH_NO_FREQ_844_8MHz  0x000                     //工作频率844.8MHz(868MHz频段最低频率)

#define CH_NO_FREQ_862_0MHz  0x056                     //工作频率862.0MHz
#define CH_NO_FREQ_868_0MHz  0x074                     //工作频率868.0MHz(868MHz频段基准频率)
#define CH_NO_FREQ_868_2MHz  0x075                     //工作频率868.2MHz
#define CH_NO_FREQ_868_4MHz  0x076                     //工作频率868.4MHz
#define CH_NO_FREQ_869_8MHz  0x07d                     //工作频率869.8MHz
#define CH_NO_FREQ_895_8MHz  0x0ff                     //工作频率895.8MHz
#define CH_NO_FREQ_896_0MHz  0x100                     //工作频率896.0MHz
#define CH_NO_FREQ_900_0MHz  0x114                     //工作频率900.0MHz
#define CH_NO_FREQ_902_2MHz  0x11f                     //工作频率902.2MHz
#define CH_NO_FREQ_902_4MHz  0x120                     //工作频率902.4MHz
#define CH_NO_FREQ_915_0MHz  0x15f                     //工作频率915.0MHz(915MHz频段基准频率)
#define CH_NO_FREQ_927_8MHz  0x19f                     //工作频率927.8MHz

#define CH_NO_FREQ_947_0MHz  0x1ff                     //工作频率947.0MHz(915MHz频段最高频率)

#define CH_NO_FREQ           CH_NO_FREQ_430_0MHz       //工作频率433.0MHz


#define CH_NO_BYTE           CH_NO_FREQ & 0xff         //工作频率低8位       Byte0       01101100

#define AUTO_RETRAN          0x20                      //重发数据包          Byte1.5     0
#define RX_RED_PWR           0x10                      //接收低功耗模式       Byte1.4     0
#define PA_PWR__10dBm        0x00                      //输出功率-10dBm      Byte1.3~2   00
#define PA_PWR_2dBm          0x04                      //输出功率+2dBm       Byte1.3~2
#define PA_PWR_6dBm          0x08                      //输出功率+6dBm       Byte1.3~2
#define PA_PWR_10dBm         0x0c                      //输出功率+10dBm      Byte1.3~2   
#define HFREQ_PLL_433MHz     0x00                      //工作在433MHz频段    Byte1.1     0
#define HFREQ_PLL_868MHz     0x02                      //工作在868MHz频段    Byte1.1
#define HFREQ_PLL_915MHz     0x02                      //工作在915MHz频段    Byte1.1
#define CH_NO_BIT8           CH_NO_FREQ >> 8           //工作频率第9位       Byte1.0     0

#define TX_AFW_1BYTE         1 * 16                    //发送地址宽度1字节   Byte2.7~4
#define TX_AFW_2BYTE         2 * 16                    //发送地址宽度2字节   Byte2.7~4
#define TX_AFW_3BYTE         3 * 16                    //发送地址宽度3字节   Byte2.7~4
#define TX_AFW_4BYTE         4 * 16                    //发送地址宽度4字节   Byte2.7~4   100
#define RX_AFW_1BYTE         1                         //接收地址宽度1字节   Byte2.3~0
#define RX_AFW_2BYTE         2                         //接收地址宽度2字节   Byte2.3~0
#define RX_AFW_3BYTE         3                         //接收地址宽度3字节   Byte2.3~0
#define RX_AFW_4BYTE         4                         //接收地址宽度4字节   Byte2.3~0   100

#define RX_PW_1BYTE          1                         //接收数据宽度1字节   Byte3.5~0
#define RX_PW_32BYTE         32                        //接收数据宽度32字节  Byte3.5~0   00100000
#define TX_PW_1BYTE          1                         //发送数据宽度1字节   Byte4.5~0 
#define TX_PW_32BYTE         32                        //发送数据宽度32字节  Byte4.5~0   00100000

#define RX_ADDRESS_0         RX_ADDRESS >> 24          //接收有效地址第1字节 Byte5       11100111
#define RX_ADDRESS_1         (RX_ADDRESS >> 16) & 0xff //接收有效地址第2字节 Byte6       11100111 
#define RX_ADDRESS_2         (RX_ADDRESS >> 8) & 0xff  //接收有效地址第3字节 Byte7       11100111
#define RX_ADDRESS_3         RX_ADDRESS & 0xff         //接收有效地址第4字节 Byte8       11100111
       
#define CRC_MODE_16BIT       0x80                      //CRC16模式         Byte9.7     1
#define CRC_MODE_8BIT        0x00                      //CRC8模式          Byte9.7     
#define CRC_EN               0x40                      //CRC使能           Byte9.6     1
#define CRC16_EN             0xc0                      //CRC16模式使能      Byte9.7~6   11
#define CRC8_EN              0x40                      //CRC8模式使能       Byte9.7~6
#define XOF_20MHz            0x20                      //晶体振荡器频率20MHz Byte9.5~3
#define XOF_16MHz            0x18                      //晶体振荡器频率16MHz Byte9.5~3   100
#define XOF_12MHz            0x10                      //晶体振荡器频率12MHz Byte9.5~3
#define XOF_8MHz             0x08                      //晶体振荡器频率8MHz  Byte9.5~3
#define XOF_4MHz             0x00                      //晶体振荡器频率4MHz  Byte9.5~3
#define UP_CLK_EN            0x40                      //输出时钟使能        Byte9.2     1
#define UP_CLK_FREQ_500kHz   0x03                      //输出时钟频率500kHz  Byte9.1~0   11
#define UP_CLK_FREQ_1MHz     0x02                      //输出时钟频率1MHz    Byte9.1~0
#define UP_CLK_FREQ_2MHz     0x01                      //输出时钟频率2MHz    Byte9.1~0
#define UP_CLK_FREQ_4MHz     0x00                      //输出时钟频率4MHz    Byte9.1~0

#define UP_CLK_EN_500kHz     0x43                      //输出时钟频率500kHz  Byte9.2~0   111
#define UP_CLK_EN_1MHz       0x42                      //输出时钟频率1MHz    Byte9.2~0
#define UP_CLK_EN_2MHz       0x41                      //输出时钟频率2MHz    Byte9.2~0
#define UP_CLK_EN_4MHz       0x40                      //输出时钟频率4MHz    Byte9.2~0

#define TX_ADDRESS_0         TX_ADDRESS >> 24          //发送有效地址第1字节
#define TX_ADDRESS_1         (TX_ADDRESS >> 16) & 0xff //发送有效地址第2字节
#define TX_ADDRESS_2         (TX_ADDRESS >> 8) & 0xff  //发送有效地址第3字节
#define TX_ADDRESS_3         TX_ADDRESS & 0xff         //发送有效地址第4字节



extern volatile uint8_t TxBuf[32], RxBuf[32];

/**
 * <p> Set nRF905 Operating Mode: Power down and SPI programming </p>
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
void    SetPowerOffMode(void);

/**
 * <p> Set nRF905 Operating Mode: Standby and SPI Porgramming </p>
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
void    SetStandbyMode(void);

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
 * ------------+----------------
 * |           |               |
 * |   ADDR    |   PAYLOAD     |
 * |           |               |
 * ------------+----------------
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
 * @param  void
 * @return void
 */
void    ConfigGPIO(void);

/**
 * <p> Write nRF905 Register </p>
 * @param  void
 * @return void
 */
void    Config905(void);

/**
 * <p> Init nRF905 </p>
 * @param  void
 * @return void
 */
void    Init905(void);
                                        /* Delay  */
/**
 * <p> Delay 28 ns </p>
 * @param  uint32_t ns
 * @return void
 */
void    Delay28ns(uint32_t ns);         /* LPC1752 a Cycle is 28ns */
    
/**
 * <p> Delay 1 us </p>
 * @param  uint8_t us
 * @return void
 */
void    Delay1us(uint8_t us);           /* 1 us   */

/**
 * <p> Delay 1 ms </p>
 * @param  uint32_t ms
 * @return void
 */
void    Delay1ms(uint32_t ms);          /* 1ms    */

/**
 * <p> Test SPI </p>
 * @param  uint8_t cmd
 * @return uint8_t status
 */
uint8_t    MSpiTest(uint8_t cmd);


#endif

/**
 * ******************************************************************************************************
 * End Of File
 * ******************************************************************************************************
 */
