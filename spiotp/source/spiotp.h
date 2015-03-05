
#ifndef __SPI_OTP_H__
#define __SPI_OTP_H__

#include <linux/init.h>

#define CR_NO_INTER_DELAY		(0x1 << 21)
#define CR_LSB				(0x1 << 20)
#define CR_POS_RX			(0x1 << 19)
#define CR_CSEN				(1 << 17)
#define CR_CSFORCE			(0x1 << 16)
#define CR_CSVALID			(0x3 << 16)
#define CR_ICNT_2			(0x1 << 14)
#define CR_ICNT_3			(0x2 << 14)
#define CR_ICNT_4			(0x3 << 14)
#define CR_SPIE				(0x1 << 13)
#define CR_SPGO				(0x1 << 12)
#define CR_CSPOL_H			(0x1 << 10)
#define CR_CPOL_H			(0x1 << 9)
#define CR_CPHA_H			(0x1 << 8)
#define CR_CSDL(x)			((x) << 5)
#define CR_SPBR(x)			((x) << 0)

#define SR_BUSY				(0x1 << 1)
#define SR_OPE_RDY			(0x1 << 0)

#define SPI_SIZE			16
#define SZ_MULT				0x10

#define SPI_TRANSFER_SIZE	(sizeof(unsigned int))

#define FLASH_PAGESIZE		256

#define READ_DUMMY_DATA 	0x00

/* Flash opcodes. */
#define	OPCODE_WREN		0x06	/* Write enable */
#define	OPCODE_RDSR		0x05	/* Read status register */
#define	OPCODE_WRSR		0x01	/* Write status register 1 byte */
#define	OPCODE_NORM_READ	0x03	/* Read data bytes (low frequency) */
#define	OPCODE_FAST_READ	0x0b	/* Read data bytes (high frequency) */
#define	OPCODE_PP		0x02	/* Page program (up to 256 bytes) */
#define	OPCODE_BE_4K 		0x20	/* Erase 4KiB block */
#define	OPCODE_BE_32K		0x52	/* Erase 32KiB block */
#define	OPCODE_SE		0xd8	/* Sector erase (usually 64KiB) */
#define	OPCODE_RDID		0x9f	/* Read JEDEC ID */

#define CFI_MFR_MACRONIX    0x00C2
/* Used for SST flashes only. */
#define OPCODE_BP       0x02    /* Byte program */
#define OPCODE_WRDI     0x04    /* Write disable */
#define OPCODE_AAI_WP       0xad    /* Auto address increment word program */

/* Used for Macronix flashes only. */
#define OPCODE_EN4B     0xb7    /* Enter 4-byte mode */
#define OPCODE_EX4B     0xe9    /* Exit 4-byte mode */

/* Used for Spansion flashes only. */
#define OPCODE_BRWR     0x17    /* Bank register write */

/* Status Register bits. */
#define	SR_WIP			1	/* Write in progress */
#define	SR_WEL			2	/* Write enable latch */
#define	SR_BP0			4	/* Block protect 0 */
#define	SR_BP1			8	/* Block protect 1 */
#define	SR_BP2			0x10	/* Block protect 2 */
#define	SR_SRWD			0x80	/* SR write protect */

/* Define max times to check status register before we give up. */
#define	MAX_READY_WAIT_COUNT	100000

//Please Notice !!!!
typedef enum
{
    W25Q128FV = 1,
    W25Q64FV,
    GD25Q64B,
    EN25Q,//en25q32 en25q64 en25q128
}SupportList;


/*
 * SPI device driver setup and teardown
 */
struct flash_info {
	char		*name;

	/* JEDEC id zero means "no ID" (most older chips); otherwise it has
	 * a high byte of zero plus three data bytes: the manufacturer id,
	 * then a two byte device id.
	 */
	u32		jedec_id;

	/* The size listed here is what works with OPCODE_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	u16		n_sectors;

	u16		flags;
#define	SECT_4K		0x01		/* OPCODE_BE_4K works uniformly */
    u16     addr_width;
};

/* NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
static struct flash_info spi_flash [] = {

	/* Atmel -- some are (confusingly) marketed as "DataFlash" */
	{ "at25fs010",  0x1f6601, 32 * 1024, 4, SECT_4K, },
	{ "at25fs040",  0x1f6604, 64 * 1024, 8, SECT_4K, },

	{ "at25df041a", 0x1f4401, 64 * 1024, 8, SECT_4K, },
	{ "at25df641",  0x1f4800, 64 * 1024, 128, SECT_4K, },

	{ "at26f004",   0x1f0400, 64 * 1024, 8, SECT_4K, },
	{ "at26df081a", 0x1f4501, 64 * 1024, 16, SECT_4K, },
	{ "at26df161a", 0x1f4601, 64 * 1024, 32, SECT_4K, },
	{ "at26df321",  0x1f4701, 64 * 1024, 64, SECT_4K, },

	/* Spansion -- single (large) sector size only, at least
	 * for the chips listed here (without boot sectors).
	 */
	{ "s25sl004a", 0x010212, 64 * 1024, 8, },
	{ "s25sl008a", 0x010213, 64 * 1024, 16, },
	{ "s25sl016a", 0x010214, 64 * 1024, 32, },
	{ "s25sl032a", 0x010215, 64 * 1024, 64, },
	{ "s25sl064a", 0x010216, 64 * 1024, 128, },

	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf040b", 0xbf258d, 64 * 1024, 8, SECT_4K, },
	{ "sst25vf080b", 0xbf258e, 64 * 1024, 16, SECT_4K, },
	{ "sst25vf016b", 0xbf2541, 64 * 1024, 32, SECT_4K, },
	{ "sst25vf032b", 0xbf254a, 64 * 1024, 64, SECT_4K, },

	/* ST Microelectronics -- newer production may have feature updates */
	{ "m25p05",  0x202010,  32 * 1024, 2, },
	{ "m25p10",  0x202011,  32 * 1024, 4, },
	{ "m25p20",  0x202012,  64 * 1024, 4, },
	{ "m25p40",  0x202013,  64 * 1024, 8, },
	/*{ "m25p80",         0,  64 * 1024, 16, },*/
	{ "m25p16",  0x202015,  64 * 1024, 32, },
	{ "m25p32",  0x202016,  64 * 1024, 64, },
	{ "m25p64",  0x202017,  64 * 1024, 128,},
	{ "m25p128", 0x202018, 256 * 1024, 64, },

	{ "m45pe80", 0x204014,  64 * 1024, 16, },
	{ "m45pe16", 0x204015,  64 * 1024, 32, },

	{ "m25pe80", 0x208014,  64 * 1024, 16, },
	{ "m25pe16", 0x208015,  64 * 1024, 32, SECT_4K, },

	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
	{ "w25x10", 0xef3011, 64 * 1024, 2, SECT_4K, },
	{ "w25x20", 0xef3012, 64 * 1024, 4, SECT_4K, },
	{ "w25x40", 0xef3013, 64 * 1024, 8, SECT_4K, },
	{ "w25x80", 0xef3014, 64 * 1024, 16, SECT_4K, },
	{ "w25x16", 0xef3015, 64 * 1024, 32, SECT_4K, },
	{ "w25x32", 0xef3016, 64 * 1024, 64, SECT_4K, },
	{ "w25x64", 0xef3017, 64 * 1024, 128, SECT_4K, },
	{ "w25q64", 0xef4017, 64 * 1024, 128, SECT_4K, },
	{ "w25x128", 0xef4018, 64 * 1024, 256, SECT_4K, },

	/* Spasion -- s25f "blocks" are 64K, "sectors" are 4KiB */
	{ "s25fl032p", 0x010215, 64 * 1024, 64,  SECT_4K, },
	{ "s25fl064p", 0x010216, 64 * 1024, 128, SECT_4K, },
	{ "s25fl128p", 0x012018, 64 * 1024, 256, SECT_4K, },
		
	{ "mx25l25635e",0xc22019,64 * 1024, 512, SECT_4K, },

	/*  Gigadevice */
	{ "gd25q32", 0xc84016, 64 * 1024, 64,  SECT_4K, },
	{ "gd25q64", 0xc84017, 64 * 1024, 128, SECT_4K, },
	{ "gd25q128", 0xc84018, 64 * 1024, 256, SECT_4K, },

	/* Eon */
	{ "en25q32", 0x1c3016, 64 * 1024, 64,  SECT_4K, },
	{ "en25q64", 0x1c3017, 64 * 1024, 128, SECT_4K, },
	{ "en25q128", 0x1c3018, 64 * 1024, 256, SECT_4K, },
};

struct spi_regs {
	volatile unsigned int SPI_CR;
	volatile unsigned int SPI_SR;
	volatile unsigned int SPI_TX_DATA;
	volatile unsigned int SPI_RX_DATA;
};

struct spi_id {
	u8	cmd;
	u8 	code[8];	
	u8	len;
};

#define SPI_ID              			_IOWR('o', 1, struct spi_id)
#define SPI_OTP_LOCK             		_IOW('o', 2,u8)
#define SPI_OTP_LOCK_STATUS      _IOR('o', 3,u8)
#define SPI_OTP_ERASE            	_IOW('o', 4,u32)

#endif
