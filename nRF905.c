/**
 * ******************************************************************************************************
 *                                             Wireless Device
 *                                               RF project
 *                                               nRF905 File
 *                                               
 *                              (c) Copyright 2011-2025, Li.Guibin, BeiJing, ZH
 *                                            All Right Reserved
 * Date : 9/20/2011
 * File : nRF905.c
 * By   : Li.Guibin
 * ******************************************************************************************************
 */

#include <stdint.h>
#include "nRF905.h"
//#include "spi.h"

#if (CM3LPC17xx)
#include "LPC17xx.h"
#include "../ssp/ssp17xx.h"
#else
#include "LPC11xx.h"
#include "../ssp/ssp11xx.h"
#include "../gpio/gpio.h"
#endif

#if (!CM3LPC17xx)                      /* LPC11xx Read PIN data */
    #define GPIO_BASE                  0x50000000UL
    #define GPIO0MASKED_ACCESS(bit)    (*(volatile uint32_t *) (GPIO_BASE + 0x00000 + (bit)))
    #define GPIO1MASKED_ACCESS(bit)    (*(volatile uint32_t *) (GPIO_BASE + 0x10000 + (bit)))
    #define GPIO2MASKED_ACCESS(bit)    (*(volatile uint32_t *) (GPIO_BASE + 0x20000 + (bit)))
    #define GPIO3MASKED_ACCESS(bit)    (*(volatile uint32_t *) (GPIO_BASE + 0x30000 + (bit)))

    /**
     * <p> example </p>
     * <p> This function not validate! </p>
     *
     * data = GPIO2MASKED_ACCESS(1UL << 9);    // Read from PIO2.7 status
     */
#endif

volatile uint8_t TxBuf[32] = "TestData", RxBuf[32] = "0123456789";

typedef struct 
{
    uint8_t len;
    uint8_t buf[10];
} RFconfig;

RFconfig TxRxConf =
{
    10,
                                                   /* RF - Configuration Register */
    0x01,    /* Byte 0 - CH_NO: CH_NO[7:0]: Init value = 0110_1100 */
    0x0C,    /* Byte 1 - AUTO_RETRAN, RX_RED_PWR, PA_PWR[1:0], HFREQ_PLL, CH_NO[8]: IVal = 0000_0000 */
    0x44,    /* Byte 2 - TX_AFW[2:0], RX_AFW[2:0]: Init value = 0100_0100 */
    0x20,    /* Byte 3 - RX_PW[5:0]: Init value = 0010_0000 */
    0x20,    /* Byte 4 - TX_PW[5:0]: Init value = 0010_0000 */
    0xCC,    /* Byte 5 - RX_ADDRESS(device identity) byte 0: Init value = E7 */
    0xCC,    /* Byte 6 - RX_ADDRESS(device identity) byte 1: Init value = E7 */
    0xCC,    /* Byte 7 - RX_ADDRESS(device identity) byte 2: Init value = E7 */
    0xCC,    /* Byte 8 - RX_ADDRESS(device identity) byte 3: Init value = E7 */
    0x58     /* Byte 9 - CRC_MODE, CRC_EN, XOF[2:0], UP_CLK_EN, UP_CLK_FREQ[1:0]: IVal = 1110_0111 */
};

typedef struct
{
   uint8_t len;
   uint8_t buf[4];
} TxAddress;

TxAddress TxAddr = 
{
    4,
    
    0xCC,    /* Byte 0 - TX_ADDRESS[7:0]:   Init value = E7 */
    0xCC,    /* Byte 1 - TX_ADDRESS[15:8]:  Init value = E7 */
    0xCC,    /* Byte 2 - TX_ADDRESS[23:16]: Init value = E7 */
    0xCC     /* Byte 3 - TX_ADDRESS[31:24]: Init value = E7 */
};
 
/**
 * <p> Delay 28 ns </p>
 * @param  uint32_t ns
 * @return void
 */
void    Delay28ns(uint32_t ns)
{
    while (ns--)
        __nop();
}

/**
 * <p> Delay 1 us </p>
 * @param uint8_t us
 * @return void
 */
void Delay1us(uint8_t us)
{
    uint8_t i, j;
    for (i = 0; i < us; i++)
        for (j = 0; j < 0x42; j++)                 /* delay, minimum 1us */
            ;
}

/**
 * <p> Delay 1 ms </p>
 * @param uint32_t ms
 * @return void
 */
void Delay1ms(uint32_t ms)
{
    uint32_t i, j;
    for (i = 0; i < ms; i++)
        for ( j = 0; j < 0x10000; j++ )            /* delay, minimum 1ms */
            ;
}


/**
 * <p> System Board Init </p>
 * @param  void
 * @return void
 */
void    ConfigGPIO(void)
{
                                                   /* Config GPIO      */
#if (CM3LPC17xx)                                   /* LPC17xx          */
      // SPI
      // LPC_PINCON->PINSEL0 |= 0x03 << 30;          /* P0.15(Function when 11) - SCK  */
      // LPC_PINCON->PINSEL1 |= 0x03 << 4;           /* P0.18(Function when 11) - MOSI */
      // LPC+PINCON->PINSEL1 |= 0x03 << 2;           /* P0.17(Function when 11) - MISO */
    
    SSP0Init();
                                                   /* GPIO Output      */
    LPC_GPIO2->FIODIR |= TX_EN | TRX_CE | PWR_UP;
    LPC_GPIO2->FIODIR |= CSN;
    // LPC_GPIO0->FIODIR |= SCK | MOSI;
    
                                                   /* GPIO Input       */
    // LPC_GPIO0->FIODIR &= ~MISO;
    LPC_GPIO2->FIODIR &= (~CD);
    LPC_GPIO2->FIODIR &= (~AM);
    LPC_GPIO2->FIODIR &= (~DREADY);
    
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
    
}

/**
 * <p> Write nRF905 Register </p>
 * @param  void
 * @return void
 */
void Config905(void)
{
    uint8_t i;

#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR |= CSN;                      /* Set CSN Low      */
#else
    GPIOSetValue(PORT0, CSN, 0);                   /* Set CSN = 0      */
#endif
                                                   /* Write RF Command */
    
    MSpiWrite(0, WC);

    for (i = 0; i < TxRxConf.len; i++) {
       MSpiWrite(0, TxRxConf.buf[i]); 
    }
    
    /*
    
    MSpiWrite(WC);                                 // Write config command
    MSpiWrite(CH_NO_BYTE);                         //0x4c  //中心频率低8位
    MSpiWrite(PA_PWR_10dBm | HFREQ_PLL_433MHz);    //0x0c  //发射+10dBm,发射频率433MHz,中心频率第9位=0
    MSpiWrite(TX_AFW_4BYTE | RX_AFW_4BYTE);        //0x44  //接收地址宽度4字节,发送地址宽度4字节
    MSpiWrite(RX_PW_32BYTE);                       //0x20  //接收数据宽度32字节
    MSpiWrite(TX_PW_32BYTE);                       //0x20  //发送数据宽度32字节
    MSpiWrite(RX_ADDRESS_0);                       //0x12  //接收有效地址第1字节
    MSpiWrite(RX_ADDRESS_1);                       //0x34  //接收有效地址第2字节
    MSpiWrite(RX_ADDRESS_2);                       //0x56  //接收有效地址第3字节
    MSpiWrite(RX_ADDRESS_3);                       //0x78  //接收有效地址第4字节
    MSpiWrite(CRC16_EN | XOF_16MHz);               //0xD8  //CRC16模式使能,晶体振荡器频率16MHz
    
    */

    
#if (CM3LPC17xx)
    LPC_GPIO2->FIOSET |= CSN;                      /* Set P2.6(CSN) High    */
#else
    GPIOSetValue(PORT0, CSN, 1);                   /* Set PIO0_2(CSN) High  */
#endif

}

/**
 * <p> Init nRF905 </p>
 * @param  void
 * @return void
 */
void    Init905(void)
{
    ConfigGPIO();
    SetStandbyMode();
    //Delay1ms(3);                                 /* Must be >3ms */
    Delay28ns(108000);
    Config905();
    SetRxMode();                                   /* Set nRF905 in Rx Mode          */
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
    __nop();
    __nop();
    __nop();
    LPC_GPIO2->FIOSET |= PWR_UP;
    __nop();
    LPC_GPIO2->FIOSET |= TRX_CE;
    __nop();
    LPC_GPIO2->FIOSET |= TX_EN;
#else
    __nop();
    __nop();
    __nop();
    GPIOSetValue(PORT2, PWR_UP, 1);
    __nop();
    GPIOSetValue(PORT2, TRX_CE, 1);
    __nop();
    GPIOSetValue(PORT2, TX_EN,  1);;
#endif

    //Delay1ms(1);
    Delay28ns(108000);
}

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
void    SetPowerOffMode(void)
{
#if (CM3LPC17xx)
    __nop();
    __nop();
    __nop();
    LPC_GPIO2->FIOCLR |= PWR_UP;
#else
    __nop();
    __nop();
    __nop();
    GPIOSetValue(PORT2, PWR_UP, 0);
#endif

    // Delay1ms(3);
    Delay28ns(108000);
}

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
void    SetStandbyMode(void)
{
#if (CM3LPC17xx)
    __nop();
    __nop();
    __nop();
    LPC_GPIO2->FIOSET |= PWR_UP;
    __nop();
    LPC_GPIO2->FIOCLR |= TRX_CE;
#else
    __nop();
    __nop();
    __nop();
    GPIOSetValue(PORT2, PWR_UP, 1);
    __nop();
    GPIOSetValue(PORT2, TRX_CE, 0);
#endif

    // Delay1ms(1);
    Delay28ns(108000);
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
    __nop();
    __nop();
    __nop();
    LPC_GPIO2->FIOSET |= PWR_UP;
    __nop();
    LPC_GPIO2->FIOSET |= TRX_CE;
    __nop();
    LPC_GPIO2->FIOCLR |= TX_EN;
#else
    __nop();
    __nop();
    __nop();
    GPIOSetValue(PORT2, PWR_UP, 1);
    __nop();
    GPIOSetValue(PORT2, TRX_CE, 1);
    __nop();
    GPIOSetValue(PORT2, TX_EN,  0);
#endif
    // Delay1ms(1);
    Delay28ns(108000); 
}

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
void TxPacket(void)
{
    uint8_t i;
    
    SetStandbyMode();
    __nop();

                                                        /* Write 4 bytes Address  */
#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR &= CSN;                           /* SPI Enable             */
#else
    GPIOSetValue(PORT0, CSN, 0);
#endif
     
    MSpiWrite(0, WTA);                                  /* Write Address command  */
     
    for (i = 0; i < 4; i++) {
        MSpiWrite(0, TxAddr.buf[i]);
    }

#if (CM3LPC17xx)
    LPC_GPIO2->FIOSET |= CSN;                           /* SPI Disable            */
#else
    GPIOSetValue(PORT0, CSN, 1);
#endif 
    
                                                        /* Write 32 bytes Tx Data */
#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR &= CSN;                           /* SPI Enable             */
#else
    GPIOSetValue(PORT0, CSN, 0);
#endif

    MSpiWrite(0, WTP);
    for (i = 0; i < 32; i++) {
        MSpiWrite(0, TxBuf[i]);
    }

#if (CM3LPC17xx)
    LPC_GPIO2->FIOSET |= CSN;                           /* SPI Disable            */
#else
    GPIOSetValue(PORT0, CSN, 1);
#endif

    //Delay1us(100);
    Delay28ns(3572);
                                                        /* Enables chip for receive and transmit       */
    SetTxMode();    
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
    SetStandbyMode();
    //Delay1ms(3);                                      /* Must be >3ms */
    Delay28ns(108000);

                                                        /* TX_EN = Low */
#if (CM3LPC17xx)
    if (!(LPC_GPIO2->FIOSET & TX_EN)) {
#else
    if (!(LPC_GPIO2->MASKED_ACCESS[1UL << TX_EN] & (1UL << TX_EN))) {
#endif
                                                        /* CD = Low, AM = Low */
#if (CM3LPC17xx)
        if ((LPC_GPIO2->FIOPIN & CD) || (LPC_GPIO2->FIOPIN & AM)) {
#else
        if ((LPC_GPIO2->MASKED_ACCESS[1UL << CD] & (1UL << CD)) 
          || (LPC_GPIO2->MASKED_ACCESS[1UL << AM] & (1UL << AM))) {
#endif

            SetStandbyMode();
            __nop();
                                                        /* Read data and save to buffer */ 
#if (CM3LPC17xx)
            LPC_GPIO2->FIOCLR |= CSN;
#else
            GPIOSetValue(PORT0, CSN, 0);
#endif
            MSpiWrite(0, RRP);
            for (i = 0; i < 32; i++) {
                RxBuf[i] = MSpiRead(0);
            }
#if (CM3LPC17xx)
            LPC_GPIO2->FIOSET |= CSN;
#else
            GPIOSetValue(PORT0, CSN, 1);
#endif
                                                        /* Disable chip for receive and transmit */
#if (CM3LPC17xx)
            if (LPC_GPIO2->FIOSET & TX_EN) {
                LPC_GPIO2->FIOSET |= TRX_CE;
            }                        
#else
            if ((LPC_GPIO2->MASKED_ACCESS[1UL << TX_EN] & (1UL << TX_EN))) {
                GPIOSetValue(PORT2, TRX_CE, 1);
            }
#endif
        }
    }
}
    
/**
 * <p> Test SPI </p>
 * @param  uint8_t cmd
 * @return uint8_t status
 */
uint8_t    MSpiTest(uint8_t cmd)
{
    uint8_t i, status;

#if (CM3LPC17xx)
    LPC_GPIO2->FIOCLR |= CSN;
#else
    GPIOSetValue(PORT0, CSN, 0);
#endif
    
    MSpiWrite(0, cmd);
    
    for (i = 0; i < TxRxConf.len; i++) {
        RxBuf[i] = MSpiRead(0);
    }
    
#if (CM3LPC17xx)    
    LPC_GPIO2->FIOSET |= CSN;
#else 
    GPIOSetValue(PORT0, CSN, 1);
#endif
    status = RxBuf[0];

    return (status);
    
}

/**
 * ******************************************************************************************************
 * End Of File
 * ******************************************************************************************************
 */
