#include "spi.h"

#include <lpc2000/lpc214x.h>
#include <lpc2000/io.h>

#define PCB_PINSEL0_ALL_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL1_ALL_GPIO      ((unsigned int) 0x00000000)

#define PCB_PINSEL0_P00_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P00_TXD0      ((unsigned int) 0x00000001)
#define PCB_PINSEL0_P00_PWM1      ((unsigned int) 0x00000002)
#define PCB_PINSEL0_P00_RSVD3     ((unsigned int) 0x00000003)
#define PCB_PINSEL0_P00_MASK      ((unsigned int) 0x00000003)

#define PCB_PINSEL0_P01_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P01_RXD0      ((unsigned int) 0x00000004)
#define PCB_PINSEL0_P01_PWM3      ((unsigned int) 0x00000008)
#define PCB_PINSEL0_P01_EINT0     ((unsigned int) 0x0000000c)
#define PCB_PINSEL0_P01_MASK      ((unsigned int) 0x0000000c)

#define PCB_PINSEL0_P02_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P02_SCL0      ((unsigned int) 0x00000010)
#define PCB_PINSEL0_P02_CAP00     ((unsigned int) 0x00000020)
#define PCB_PINSEL0_P02_RSVD3     ((unsigned int) 0x00000030)
#define PCB_PINSEL0_P02_MASK      ((unsigned int) 0x00000030)

#define PCB_PINSEL0_P03_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P03_SDA0      ((unsigned int) 0x00000040)
#define PCB_PINSEL0_P03_MAT00     ((unsigned int) 0x00000080)
#define PCB_PINSEL0_P03_EINT1     ((unsigned int) 0x000000c0)
#define PCB_PINSEL0_P03_MASK      ((unsigned int) 0x000000c0)

#define PCB_PINSEL0_P04_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P04_SCK0      ((unsigned int) 0x00000100)
#define PCB_PINSEL0_P04_CAP01     ((unsigned int) 0x00000200)
#define PCB_PINSEL0_P04_RSVD3     ((unsigned int) 0x00000300)
#define PCB_PINSEL0_P04_MASK      ((unsigned int) 0x00000300)

#define PCB_PINSEL0_P05_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P05_MISO0     ((unsigned int) 0x00000400)
#define PCB_PINSEL0_P05_MAT01     ((unsigned int) 0x00000800)
#define PCB_PINSEL0_P05_AD06      ((unsigned int) 0x00000c00)
#define PCB_PINSEL0_P05_MASK      ((unsigned int) 0x00000c00)

#define PCB_PINSEL0_P06_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P06_MOSI0     ((unsigned int) 0x00001000)
#define PCB_PINSEL0_P06_CAP02     ((unsigned int) 0x00002000)
#define PCB_PINSEL0_P06_AD10      ((unsigned int) 0x00003000)
#define PCB_PINSEL0_P06_MASK      ((unsigned int) 0x00003000)

#define PCB_PINSEL0_P07_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P07_SSEL0     ((unsigned int) 0x00004000)
#define PCB_PINSEL0_P07_PWM2      ((unsigned int) 0x00008000)
#define PCB_PINSEL0_P07_EINT2     ((unsigned int) 0x0000c000)
#define PCB_PINSEL0_P07_MASK      ((unsigned int) 0x0000c000)

#define PCB_PINSEL0_P08_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P08_TXD1      ((unsigned int) 0x00010000)
#define PCB_PINSEL0_P08_PWM4      ((unsigned int) 0x00020000)
#define PCB_PINSEL0_P08_AD11      ((unsigned int) 0x00030000)
#define PCB_PINSEL0_P08_MASK      ((unsigned int) 0x00030000)

#define PCB_PINSEL0_P09_GPIO      ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P09_RXD1      ((unsigned int) 0x00040000)
#define PCB_PINSEL0_P09_PWM6      ((unsigned int) 0x00080000)
#define PCB_PINSEL0_P09_EINT3     ((unsigned int) 0x000c0000)
#define PCB_PINSEL0_P09_MASK      ((unsigned int) 0x000c0000)

#define PCB_PINSEL0_P010_GPIO     ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P010_RTS1     ((unsigned int) 0x00100000)
#define PCB_PINSEL0_P010_CAP10    ((unsigned int) 0x00200000)
#define PCB_PINSEL0_P010_AD12     ((unsigned int) 0x00300000)
#define PCB_PINSEL0_P010_MASK     ((unsigned int) 0x00300000)

#define PCB_PINSEL0_P011_GPIO     ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P011_CTS1     ((unsigned int) 0x00400000)
#define PCB_PINSEL0_P011_CAP11    ((unsigned int) 0x00800000)
#define PCB_PINSEL0_P011_SCL1     ((unsigned int) 0x00c00000)
#define PCB_PINSEL0_P011_MASK     ((unsigned int) 0x00c00000)

#define PCB_PINSEL0_P012_GPIO     ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P012_DSR1     ((unsigned int) 0x01000000)
#define PCB_PINSEL0_P012_MAT10    ((unsigned int) 0x02000000)
#define PCB_PINSEL0_P012_AD13     ((unsigned int) 0x03000000)
#define PCB_PINSEL0_P012_MASK     ((unsigned int) 0x03000000)

#define PCB_PINSEL0_P013_GPIO     ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P013_DTR1     ((unsigned int) 0x04000000)
#define PCB_PINSEL0_P013_MAT11    ((unsigned int) 0x08000000)
#define PCB_PINSEL0_P013_AD14     ((unsigned int) 0x0c000000)
#define PCB_PINSEL0_P013_MASK     ((unsigned int) 0x0c000000)

#define PCB_PINSEL0_P014_GPIO     ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P014_DCD1     ((unsigned int) 0x10000000)
#define PCB_PINSEL0_P014_EINT1    ((unsigned int) 0x20000000)
#define PCB_PINSEL0_P014_SDA1     ((unsigned int) 0x30000000)
#define PCB_PINSEL0_P014_MASK     ((unsigned int) 0x30000000)

#define PCB_PINSEL0_P015_GPIO     ((unsigned int) 0x00000000)
#define PCB_PINSEL0_P015_RI1      ((unsigned int) 0x40000000)
#define PCB_PINSEL0_P015_EINT2    ((unsigned int) 0x80000000)
#define PCB_PINSEL0_P015_AD15     ((unsigned int) 0xc0000000)
#define PCB_PINSEL0_P015_MASK     ((unsigned int) 0xc0000000)

#define SCB_PCONP_PCSPI0    (0x00000100)

#define SPI_SPCR_BITENABLE    ((unsigned int) 0x00000004)
#define SPI_SPCR_CPHA         ((unsigned int) 0x00000008)
#define SPI_SPCR_CPHA1ST      ((unsigned int) 0x00000000)
#define SPI_SPCR_CPHA2ND      ((unsigned int) 0x00000008)
#define SPI_SPCR_CPOL         ((unsigned int) 0x00000010)
#define SPI_SPCR_CPOLACTHIGH  ((unsigned int) 0x00000000)
#define SPI_SPCR_CPOLACTLOW   ((unsigned int) 0x00000010)
#define SPI_SPCR_SLAVE        ((unsigned int) 0x00000000)
#define SPI_SPCR_MSTR         ((unsigned int) 0x00000020)
#define SPI_SPCR_MSBF         ((unsigned int) 0x00000000)
#define SPI_SPCR_LSBF         ((unsigned int) 0x00000040)
#define SPI_SPCR_SPIE         ((unsigned int) 0x00000080)
#define SPI_SPCR_BITS8        ((unsigned int) 0x00000800)
#define SPI_SPCR_BITS9        ((unsigned int) 0x00000900)
#define SPI_SPCR_BITS10       ((unsigned int) 0x00000a00)
#define SPI_SPCR_BITS11       ((unsigned int) 0x00000b00)
#define SPI_SPCR_BITS12       ((unsigned int) 0x00000c00)
#define SPI_SPCR_BITS13       ((unsigned int) 0x00000d00)
#define SPI_SPCR_BITS14       ((unsigned int) 0x00000e00)
#define SPI_SPCR_BITS15       ((unsigned int) 0x00000f00)
#define SPI_SPCR_BITS16       ((unsigned int) 0x00000000)
#define SPI_SPCR_BITSMASK     ((unsigned int) 0x00000f00)

#define SPI_SPSR_ABRT         ((unsigned int) 0x00000008)
#define SPI_SPSR_MODF         ((unsigned int) 0x00000010)
#define SPI_SPSR_ROVR         ((unsigned int) 0x00000020)
#define SPI_SPSR_WCOL         ((unsigned int) 0x00000040)
#define SPI_SPSR_SPIF         ((unsigned int) 0x00000080)


/***********************************************************************/
/** \brief Initilialise the SPI peripheral
 *
 * Description: init the LPC2106 SPI peripheral. Simple non flexible version
 *              
 *              
 * \author Iain Derrington

 */
/**********************************************************************/

void initSPI(void)
{
    uint32_t spsr;

    //
    //  Setup SCK0, MISO0 and MOSI0.  SSEL0 (P0.7) is GPIO
    //
    PINSEL0 =
        (PINSEL0 &
         ~(PCB_PINSEL0_P04_MASK | PCB_PINSEL0_P05_MASK | PCB_PINSEL0_P06_MASK | PCB_PINSEL0_P07_MASK)) |
        (PCB_PINSEL0_P04_SCK0 | PCB_PINSEL0_P05_MISO0 | PCB_PINSEL0_P06_MOSI0 | PCB_PINSEL0_P07_GPIO);

    //
    //  Set P0.7 as output, deselect by driving high
    //
    IODIR0 |= BIT7;
    IOSET0  = BIT7;

#define CS_SEL	    BIT26|BIT27
    PINSEL0 &= ~CS_SEL;

    IODIR0 |= BIT4 | BIT5;
    IODIR0 &= ~BIT6;

    //
    //  Turn on SPI module power
    //
    PCONP |= SCB_PCONP_PCSPI0;

    //
    //  SPI configuration:
    //    Master mode
    //    Sample on 1st edge of SCK (CPHA=0)
    //    SCK is active high (CPOL=0)
    //    MSB out first
    //    No interrupts
    //
    S0SPCR = SPI_SPCR_MSTR | SPI_SPCR_CPHA1ST | SPI_SPCR_CPOLACTHIGH | SPI_SPCR_MSBF;

    //
    //  Reading the status register clears it
    //
    spsr = S0SPSR;

    //
    //  SCK0 = (PCLK = (CCLK/APB)) / 8 will be 6Mhz (putt putt putt...)
    //
    S0SPCCR = 7;
}


uint8_t spiPut (uint8_t valueIn)
{
    uint8_t res;
    S0SPDR = valueIn;

    while (!(S0SPSR & SPI_SPSR_SPIF));
    res = S0SPDR;

    //TRACE("%x <--> %x", valueIn, res);
    return res;
}
