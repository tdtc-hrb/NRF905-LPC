/**
 * ******************************************************************************************************
 *                                             Wireless Device
 *                                               RF project
 *                                               nRF905 File
 *                                               
 *                              (c) Copyright 2011-2025, Li.Guibin, BeiJing, ZH
 *                                            All Right Reserved
 * Date : 9/7/2011
 * File : nRF905.c
 * By   : Li.Guibin
 * ******************************************************************************************************
 */

#include <stdint.h>
#include "nRF905.h"

#if (CM3LPC17xx)
#include "LPC17xx.h"
#include "spi.h"
#include "ssp17xx.h"
#else
#include "LPC11xx.h"
#include "ssp11xx.h"
#include "gpio.h"
#endif
    
volatile uint8_t TxBuf[32] = "TestData", uint8_t RxBuf[32] = "0123456789";

typedef struct 
{
    uint8_t len;
    uint8_t buf[10];
} RFconfig;

RFconfig TxRxConf =
{
    10,
    0x01, 0x0C, 0x44, 0x20, 0x20, 0xCC, 0xCC, 0xCC, 0xCC, 0x58          /* 433MHz     */
    // 0x4C, 0x0e, 0x44, 0x20, 0x20, 0xCC, 0xCC, 0xCC, 0xCC, 0x58       /* 868/915MHZ */
}

/**
 * <p> Delay 100 us </p>
 * @param uint8_t us
 * @return void
 */
void Delay100us(uint8_t us)                        /* Bus Clock: 48MHz -- 100us */
{
    uint8_t i, j;
    for (i = 0; i < us; i++)
        for (j = 0; j < 405; j++)
            ;
}

/**
 * <p> Delay 1 ms </p>
 * @param uint32_t ms
 * @return void
 */
void Delay1ms(uint32_t ms)                         /* Bus Clock: 48MHz -- 1ms   */
{
    uint32_t i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 4005; j++)
            ;
}

/**
 * <p> System Board Init </p>
 * <p> Write nRF905 Register </p>
 * @param  void
 * @return void
 */
void Config905(void)
{
    uint8_t i;
                                                   /* Config GPIO      */
#if (CM3LPC17xx)                                   /* LPC17xx          */
      // SPI
      // LPC_PINCON->PINSEL0 |= 0x03 << 30;          /* P0.15(Function when 11) - SCK  */
      // LPC_PINCON->PINSEL1 |= 0x03 << 4;           /* P0.18(Function when 11) - MOSI */
      // LPC+PINCON->PINSEL1 |= 0x03 << 2;           /* P0.17(Function when 11) - MISO */
    
    SSPOInit();
                                                   /* GPIO Output      */
    LPC_GPIO2->FIODIR |= TX_EN | TRX_CE | PWR_UP;
    // LPC_GPIO0->FIODIR |= CSN | SCK | MOSI;
    
                                                   /* GPIO Input       */
    // LPC_GPIO0->FIODIR &= ~MISO;
    LPC_GPIO2->FIODIR &= (~CD) & (~AM) & (~DREADY);
    
    LPC_GPIO->FIODIR |= CSN;
    
#else                                              /* LPC11xx          */
    SSP_IOConfig(0);                               /* SSP0             */
    SSP_Init(0);
    // LPC_IOCON->PIO0_8 &= ~0x07;                 
    // LPC_IOCON->PIO0_8 |= 0x01;                  /* PIO0.8 SSP MISO  */
    
    // LPC_IOCON->PIO0_9 &= ~0x07;
    // LPC_IOCON->PIO0_9 |= 0x01;                  /* PIO0.9 SSP MOSI  */
    
    // LPC_IOCON->SCK_LOC = 0x01;
    // LPC_IOCON->PIO2_11 = 0x01;                  /* PIO2.11 SSP SCK  */
    
                                                   /* GPIO Output      */
    LPC_IOCON->PIO0_2 &= ~0x07;
    LPC_IOCON->PIO2_4 &= ~0x07;
    LPC_IOCON->PIO2_5 &= ~0x07;
    LPC_IOCON->PIO2_6 &= ~0x07;
                                                   /* GPIO Input       */
    LPC_GPIO2->DIR |= 0x07 << 4;
    LPC_GPIO0->DIR |= 0x01 << 2;
    
#endif
    
                                                   /* Set High or Low  */
#if (CM3LPC17xx)                                   /* LPC17xx          */
    LPC_GPIO2->FIOSET &= PWR_UP;                   /* Set PWR_UP High  */
    LPC_GPIO2->FIOCLR |= TRX_CE;                   /* Set TRX_CE Low   */
    LPC_GPIO2->FIOCLR |= TX_EN;                    /* Set TX_EN Low    */
    
    LPC_GPIO2->FIOCLR |= CSN;                      /* Set CSN Low      */
#else                                              /* LPC11xx          */
    GPIOSetValue(PORT2, PWR_UP, 1);                /* Set PWR_UP = 1   */
    GPIOSetValue(PORT2, TRX_CE, 0);                /* Set TRX_CE = 0   */
    GPIOSetValue(PORT2, TX_EN, 0);                 /* Set TX_EN = 0    */

    GPIOSetValue(PORT0, CSN, 0);                   /* Set CSN = 0      */
#endif
    
                                                   /* Write RF Command */
#if (CM3LPC17xx)                                   /* LPC17xx          */
      // SPI
      // SPI0SendData(WC);
    SSPSend(0, WC, sizeof(WC));
      
    for (i = 0; i < TxRxBuf.len; i++) {
          SSPSend(0, TxRxBuf.buf[i], sizeof(TxRxBuf.buf[i]));
    }
#else
    SSP_Send(0, WC, sizeof(WC));
      
    for (i = 0; i < TxRxBuf.len; i++) {
          SSP_Send(0, TxRxBuf.buf[i], sizeof(TxRxBuf.buf[i]));
    }
    
#endif
    
#if (CM3LPC17xx)
    LPC_GPIO2->FIOSET &= CSN;                      /* Set P2.6(CSN) High    */
#else
    GPIOSetValue(PORT0, CSN, 1);                   /* Set PIO0_2(CSN) High  */
#endif

}

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
void SetTxMode(void)
{
#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR |= PWR_UP;
    LPC_GPIO2->FIOCLR |= TRX_CE;
    LPC_GPIO2->FIOCLR |= TX_EN;
#else
    GPIOSetValue(PORT2, PWR_UP, 1);
    GPIOSetValue(PORT2, TRX_CE, 1);
    GPIOSetValue(PORT2, TX_EN,  1);
#endif
    Delay1ms(1);
}

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
void SetRxMode(void)
{
#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR |= PWR_UP;
    LPC_GPIO2->FIOCLR |= TRX_CE;
    LPC_GPIO2->FIOSET &= TX_EN;
#else
    GPIOSetValue(PORT2, PWR_UP, 1);
    GPIOSetValue(PORT2, TRX_CE, 1);
    GPIOSetValue(PORT2, TX_EN,  0);
#endif
    Delay1ms(1);
}

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
void TxPacket(void)
{
    uint8_t i;
                                                   /* Write 32 bytes Tx Data */
#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR |= CSN;                      /* SPI Enable             */
    
    SSPSend(0, WTP, sizeof(WTP));                  /* Write Payload command  */
    
    for (i = 0; i < 32; i++)
        SSPSend(0, TxBuf[i], sizeof(TxBuf[i])); 
    
    LPC_GPI2->FIOSET &= CSN;                       /* SPI Disable            */
#else
    GPIOSetValue(PORT0, CSN, 0);
    
    SSP_Send(0, WTP, sizeof(WTP));
    
    for (i = 0; i < 32; i++)
        SSP_Send(0, TxBuf[i], sizeof(TxBuf[i]));
        
    GPIOSetValue(PORT0, CSN, 1);
#endif

    Delay100us(1);
    
                                                                 /* Write 4 bytes Address  */
#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR |= CSN;                                    /* SPI Enable             */
    
    SSPSend(0, WA, sizeof(WA));                                  /* Write Address command  */
    
    for (i = 0; i < 4; i++)
        SSPSend(0, TxRxConf.buf[i], sizeof(TxRxConf.buf[i]));
        
    LPC_GPIO2->FIOSET &= CSN;                                     /* SPI Disable            */
#else
    GPIOSetValue(PORT0, CSN, 0);
    
    SSP_Send(0, WA, sizeof(WA));
    
    for (i = 0; i < 4; i++)
        SSP_Send(0, TxRxConf.buf[i], sizeof(TxRxConf.buf[i]));
        
    GPIOSetValue(PORT0, CSN, 1);
#endif

                                                        /* Enables chip for receive and transmit       */
#if (CM3LPC17xx)
    LPC_GPIO2->FIOSET &= TRX_CE;                        /* Set TRX_CE high, start Tx data transmission */
    
    Delay100us(1);
    
    LPC_GPIO2->FIOCLR &= TRX_CE;                        /* Set TRX_CE low */
#else
    GPIOSetValue(PORT0, TRX_CE, 1);
    
    Delay100us(1);
    
    GPIOSetValue(PORT0, TRX_CE, 0);
#endif
    
}

/**
 * <p> Read Rx Data when DR and AM is low </p>
 * @param  void
 * @return void
 */
void RxPacket(void)
{
    uint8_t i;
                                                        /* Set nRF905 in standby mode */
#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR |= TRX_CE;                        
#else
    GPIOSetValue(PORT2, TRX_CE, 0);
#endif

                                                        /* Read data and save to buffer */ 
#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR |= CSN;
    
    SSPSend(0, RRP, sizeof(RRP));
    
    SSPReceive(0, RxBuf, sizeof(RxBuf));
    
    LPC_GPIO2->FIOSET &= CSN;
#else
    GPIOSetValue(PORT0, CSN, 0);
    
    SSP_Send(0, RRP, sizeof(RRP));
    
    SSP_Receive(0, RxBuf, sizeof(RxBuf));
    
    GPIOSetValue(PORT0, CSN, 1);
#endif

// I don't usage?
#if (CM3LPC17xx)
    while ((~(LPC_GPIO2->FIOPIN & DREADY)) || (~(LPC_GPIO2->FIOPIN & AM)) != 0);
#else
    while ((~(LPC_IOCON->PIO2_9 & DREADY)) || (~(LPC_ICON->PIO2_8 & AM)) != 0);
#endif

                                                        /* Disable chip for receive and transmit */
#if (CM3LPC17xx)
    LPC_GPIO2->FIOSET &= TRX_CE;                        
#else
    GPIOSetValue(PORT2, TRX_CE, 1);
#endif

}

/**
 * ******************************************************************************************************
 * End Of File
 * ******************************************************************************************************
 */
