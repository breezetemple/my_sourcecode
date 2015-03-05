#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/input.h>
#include <linux/vmalloc.h>

#include <linux/version.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <linux/wait.h>
#include <linux/poll.h>

#include "spiotp.h"

#define MISC_SPI_OTP_MINOR 91

struct gxspi_otp_info 
{
	struct device *device;
	struct resource   *ioarea;
	volatile struct spi_regs   *regs;
	unsigned int size;
	unsigned int jedec;
	unsigned int type;
	unsigned int eon25q_address;
	struct flash_info flash;
	struct mutex lock;
};

static struct platform_device spi_otp_device =
{
	.name = "spiotp0",
	.id = -1,        
};

#ifdef REQUEST_MEM
static int otp_setup(volatile struct gxspi_otp_info *info)
{
	writel(0x8420c002, &info->regs->SPI_CR);

	return 0;
}
#endif

static inline void otp_write(volatile struct gxspi_otp_info *info, unsigned int data)
{
	unsigned int regval;
	writel(data, &info->regs->SPI_TX_DATA);
	regval = readl(&info->regs->SPI_CR);
	regval |= CR_SPGO;
	writel(regval, &info->regs->SPI_CR);

	while(!(readl(&info->regs->SPI_SR) & SR_OPE_RDY));

	regval = readl(&info->regs->SPI_CR);
	regval &= ~CR_SPGO;
	writel(regval, &info->regs->SPI_CR);
}

static inline unsigned int otp_read(volatile struct gxspi_otp_info *info)
{
	unsigned int regval;
	regval = readl(&info->regs->SPI_CR);
	regval |= CR_SPGO;
	writel(regval, &info->regs->SPI_CR);

	while(!(readl(&info->regs->SPI_SR) & SR_OPE_RDY));

	regval = readl(&info->regs->SPI_CR);
	regval &= ~CR_SPGO;
	writel(regval, &info->regs->SPI_CR);

	return readl(&info->regs->SPI_RX_DATA);
}

static inline void otp_set_txlen(volatile struct gxspi_otp_info *info, int size)
{
	int regval;
	regval = readl(&info->regs->SPI_CR);
	switch(size){
		case 1:
			if((regval & CR_ICNT_4) != 0){
				regval &= ~CR_ICNT_4;
				writel(regval, &info->regs->SPI_CR);
			}
			break;
		case 2:
			if((regval & CR_ICNT_4) != CR_ICNT_2){
				regval &= ~CR_ICNT_4;
				regval |= CR_ICNT_2;
				writel(regval, &info->regs->SPI_CR);
			}
			break;
		case 3:
			if((regval & CR_ICNT_4) != CR_ICNT_3){
				regval &= ~CR_ICNT_4;
				regval |= CR_ICNT_3;
				writel(regval, &info->regs->SPI_CR);
			}
			break;
		case 4:
			if((regval & CR_ICNT_4) != CR_ICNT_4){
				regval |= CR_ICNT_4;
				writel(regval, &info->regs->SPI_CR);
			}
			break;
		default:
			break;
	}
}

static int otp_spi_tx_rx(volatile struct gxspi_otp_info *info,u8 *cmd,u32 cmdlen, u8 *txbuf,u32 txlen,u8 *rxbuf,u32 rxlen)
{
	unsigned char *p;
	unsigned int len = 0, data, regval;

	regval = readl(&info->regs->SPI_CR);
	regval |= CR_CSVALID;
	writel(regval, &info->regs->SPI_CR);

	if(cmd && cmdlen > 0)
	{
		p = cmd;

		len = cmdlen;

		if(len >= SPI_TRANSFER_SIZE)
			otp_set_txlen(info, 4);

		while(len >= SPI_TRANSFER_SIZE)
		{
			data = (*p++) << 24;
			data |= (*p++) << 16;
			data |= (*p++) << 8;
			data |= (*p++);
			otp_write(info, data);
			len -= SPI_TRANSFER_SIZE;
		}

		if(len > 0)
		{
			otp_set_txlen(info, 1);
			while(len--){
				data = *p++;
				otp_write(info, data);
			}
		}
	}
	
	if(txbuf && txlen > 0)
	{
		p = txbuf;

		len = txlen;

		if(len >= SPI_TRANSFER_SIZE)
			otp_set_txlen(info, 4);

		while(len >= SPI_TRANSFER_SIZE)
		{
			data = (*p++) << 24;
			data |= (*p++) << 16;
			data |= (*p++) << 8;
			data |= (*p++);
			otp_write(info, data);
			len -= SPI_TRANSFER_SIZE;
		}

		if(len > 0)
		{
			otp_set_txlen(info, 1);
			while(len--){
				data = *p++;
				otp_write(info, data);
			}
		}
	}

	if(rxbuf && rxlen > 0)
	{
		p = rxbuf;

		len = rxlen;

		if(len >= SPI_TRANSFER_SIZE)
			otp_set_txlen(info, 4);

		while(len >= SPI_TRANSFER_SIZE)
		{
			data = otp_read(info);
			*p++ = (data >> 24) & 0xff;
			*p++ = (data >> 16) & 0xff;
			*p++ = (data >>  8) & 0xff;
			*p++ = data & 0xff;
			len -= SPI_TRANSFER_SIZE;
		}

		if(len > 0){
			otp_set_txlen(info, 1);
			while(len--)
			{
				data = otp_read(info);
				*p++ = data & 0xff;
			}
		}
	}

	regval = readl(&info->regs->SPI_CR);
	regval &= ~CR_CSVALID;
	writel(regval, &info->regs->SPI_CR);

	return 0;
}

static int read_sr(struct gxspi_otp_info *info)
{
	ssize_t retval;
	u8 code = OPCODE_RDSR;
	u8 val;

	retval = otp_spi_tx_rx(info, &code, 1, NULL,0,&val, 1);

	if (retval < 0) {
		printk("[%s:%d]error %d reading SR\n",
			__func__,__LINE__,(int) retval);
		return retval;
	}

	return val;
}

static inline int write_enable(struct gxspi_otp_info *info)
{
	u8	code = OPCODE_WREN;

	return otp_spi_tx_rx(info, &code, 1, NULL,0,NULL, 0);
}

static int wait_till_ready(struct gxspi_otp_info *info)
{
	int count;
	int sr;

	/* one chip guarantees max 5 msec wait here after page writes,
	 * but potentially three seconds (!) after page erase.
	 */
	for (count = 0; count < MAX_READY_WAIT_COUNT; count++) {
		if ((sr = read_sr(info)) < 0)
			break;
		else if (!(sr & SR_WIP))
			return 0;

		/* REVISIT sometimes sleeping would be best */
	}

	return 1;
}

static int eon_enter_otp(struct gxspi_otp_info *info,u8 onoff)
{
	u8 code = 0x3a;

	if(!onoff)
		code = 0x04;

	return otp_spi_tx_rx(info, &code, 1, NULL,0,NULL, 0);
}

static int read_lb(struct gxspi_otp_info *info,u8 *val)
{
	u8 code = 0x35;

	/* Wait until finished previous write command. */
	if (wait_till_ready(info))
		return -EINVAL;
	
	if(EN25Q == info->type)
	{	
		eon_enter_otp(info,1);
		
		code = 0x05;		
		otp_spi_tx_rx(info, &code, 1, NULL,0,val, 1);
		
		if(((*val) & 0x80) == 0x80)
			(*val) |= 7 << 3;/* 1 ~ 3 LB */
		
		eon_enter_otp(info,0);
		return 0;	
	}
	
	return otp_spi_tx_rx(info, &code, 1, NULL,0,val, 1);
}

static int write_lb(struct gxspi_otp_info *info,u8 num)
{
	u8 code = 0x01;
	u8 val;
    u8 lockval[2];
	
	read_lb(info,&val);

	/* Wait until finished previous write command. */
	if (wait_till_ready(info))
		return -EINVAL;

	if(EN25Q == info->type)
	{
		eon_enter_otp(info,1);
		
		/* Send write enable, then erase commands. */
		write_enable(info);
	
		code= 0x01;
		val 	= 0x80;
		otp_spi_tx_rx(info, &code, 1, &val,1,NULL, 0);

		/* Wait until finished previous write command. */
		if (wait_till_ready(info))
		{
			if(EN25Q == info->type)
				eon_enter_otp(info,0);
			return -EINVAL;
		}

		eon_enter_otp(info,0);
		
		return 0;
	}
	else if(GD25Q64B == info->type)
	{		
		write_enable(info);
		
		val |= (1 << 2); /* Just ONE LB */
        lockval[0] = (int)(read_sr(info));
        lockval[1] = val;
			
        printk("\n ==>> 0x%x 0x%x\n",lockval[0],lockval[1]);
		otp_spi_tx_rx(info, &code, 1, lockval,2,NULL, 0);

		/* Wait until finished previous write command. */
		if (wait_till_ready(info))
			return -EINVAL;

		return 0;
    }
    else if(W25Q64FV == info->type)
    {
		write_enable(info);
		
		val |= (1 << (num + 2)); /* 1 ~ 3 LB */
        lockval[0] = (int)(read_sr(info));
        lockval[1] = val;
			
        printk("\n ==>> 0x%x 0x%x\n",lockval[0],lockval[1]);
		otp_spi_tx_rx(info, &code, 1, lockval,2,NULL, 0);

		/* Wait until finished previous write command. */
		if (wait_till_ready(info))
			return -EINVAL;

		return 0;
    }
    else//w25q128fv
    {
        code = 0x31;
        write_enable(info);

        val |= (1 << (num + 2)); /* 1 ~ 3 LB */ 

        otp_spi_tx_rx(info, &code, 1, &val,1,NULL, 0);

        /* Wait until finished previous write command. */
        if (wait_till_ready(info))
            return -EINVAL;

        return 0;
    }
}

static inline int erase_otp(struct gxspi_otp_info *info,u32 address)
{
	u8 cmd[5];
	
	/* Wait until finished previous write command. */
	if (wait_till_ready(info))
		return -EINVAL;

	if(EN25Q == info->type)
		eon_enter_otp(info,1);
	
	/* Send write enable, then erase commands. */
	write_enable(info);


//	printk("[%s:%d] address : %x\n",__FUNCTION__,__LINE__,address);
	if(EN25Q == info->type)
	{
		cmd[0] = 0x20;
		address = info->eon25q_address;
	}
    else if(GD25Q64B == info->type)
    {
		cmd[0] = 0x44;
        if (address < 0x100)
            address = 0x000;
        else if (address < 0x200)
            address = 0x100;
        else if (address < 0x300)
            address = 0x200;
        else
            address = 0x300;
    }
	else
	{
		cmd[0] = 0x44;
        if (address < 0x2000)
            address = 0x1000;
        else if (address < 0x3000)
            address = 0x2000;
        else
            address = 0x3000;
	}
	cmd[1] = address >> 16;
	cmd[2] = address >> 8;
	cmd[3] = address >> 0;
	otp_spi_tx_rx(info,cmd,4,NULL,0,NULL,0);

	/* Wait until finished previous write command. */
	if (wait_till_ready(info))
	{
		if(EN25Q == info->type)
			eon_enter_otp(info,0);
		return -EINVAL;
	}
	
	if(EN25Q == info->type)
		eon_enter_otp(info,0);

	return 0;
}

static inline int read_id(struct gxspi_otp_info *info,struct spi_id *jedec)
{
	u8 code;
	u8 id[8];

	code = jedec->cmd;

	if(code == 0x9f) /* Read JEDEC ID */
	{
		otp_spi_tx_rx(info, &code, 1, NULL,0,id, 3);
		
		memcpy(jedec->code,id,3);
		jedec->len = 3;
	}
	else if(code == 0x90) /* Read Manufacturer / Device ID */
	{
		u8 cmd[5];
		
		cmd[0] = code;
		cmd[1] = 0;
		cmd[2] = 0;
		cmd[3] = 0;
		otp_spi_tx_rx(info,cmd,4,NULL,0,id,2);

		memcpy(jedec->code,id,2);
		jedec->len = 2;
	}
	else if(code == 0x4b) /* Read Unique ID Number */
	{
		u8 cmd[5];

		if(EN25Q == info->type)
		{
			eon_enter_otp(info,1);
			
			cmd[0] = 0x5A;
			cmd[1] = 0x03;
			cmd[2] = 0;
			cmd[3] = 0;
			cmd[4] = 0;
			otp_spi_tx_rx(info,cmd,5,NULL,0,id,8);

			eon_enter_otp(info,0);
		}
		else
		{
			cmd[0] = code;
			cmd[1] = 0;
			cmd[2] = 0;
			cmd[3] = 0;
			cmd[4] = 0;
			otp_spi_tx_rx(info,cmd,5,NULL,0,id,8);
		}
		
		memcpy(jedec->code,id,8);
		jedec->len = 8;
	}
	else if(code == 0xab) /* Release Power-down / Device ID */
	{
		u8 cmd[5];
		
		cmd[0] = code;
		cmd[1] = 0;
		cmd[2] = 0;
		cmd[3] = 0;
		otp_spi_tx_rx(info,cmd,4,NULL,0,id,1);

		memcpy(jedec->code,id,1);
		jedec->len = 1;
	}
	else
	{
		printk("[%s:%d]error unknow code %02x\n",
			__func__,__LINE__,code);
		return -EINVAL;
	}
	
	return 0;
}

static inline int read_jedec(struct gxspi_otp_info *info)
{
	int tmp;
	u8 code = OPCODE_RDID;
	u8 id[3];
	u32 jedec;
	struct flash_info *spi;
	
	tmp = otp_spi_tx_rx(info, &code, 1, NULL,0,id, 3);
	if (tmp < 0) {
		printk("[%s:%d]error %d reading JEDEC ID\n",
			__func__,__LINE__,tmp);
		return -EINVAL;
	}
	jedec = id[0];
	jedec = jedec << 8;
	jedec |= id[1];
	jedec = jedec << 8;
	jedec |= id[2];

	for (tmp = 0, spi = spi_flash;
		tmp < ARRAY_SIZE(spi_flash);
		tmp++, spi++) 
	{
		if (spi->jedec_id == jedec) 
		{
			info->size = spi->n_sectors * spi->sector_size;
			info->jedec = spi->jedec_id;
			if((jedec & 0x1c3000) == 0x1c3000)
			{
				info->type = EN25Q;
				if(info->size == 0x400000)
					info->eon25q_address = 0x3FF000;
				else if(info->size == 0x800000)
					info->eon25q_address = 0x7FF000;
				else if(info->size == 0x1000000)
					info->eon25q_address = 0xFFF000;
			}

			if(jedec == 0xc84017)
			{
				info->type = GD25Q64B;
			}

			if(jedec == 0xef4018)
			{
				info->type = W25Q128FV;
			}

			if(jedec == 0xef4017)
			{
				info->type = W25Q64FV;
			}
			printk("\033[32mfound flash %s %dMB Flash Type:%d\033[0m\n",spi->name,info->size/0x100000,info->type);
			memcpy(&info->flash,spi,sizeof(struct flash_info));
			return 0;
	        }
	}

	return -EINVAL;
}

static int spi_otp_do_open(struct inode *inode, struct file *filp)
{
	int err = 0;
	struct gxspi_otp_info *info = platform_get_drvdata(&spi_otp_device);

	if (!info)
		return -EINVAL;

	filp->private_data = info;

#ifdef REQUEST_MEM
	otp_setup(info);
#endif

	return err;
}

static int spi_otp_do_release(struct inode *inode, struct file *filp)
{
	struct gxspi_otp_info *info = filp->private_data; 

	if (!info)
		return -EINVAL;

	return 0;
}

static int spi_otp_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, void *parg)
{
	struct gxspi_otp_info *info = filp->private_data; 
	int err = 0;

	if (!info)
		return -EINVAL;

	switch(cmd)
	{
		case SPI_ID:
			err = read_id(info,(struct spi_id *)parg);
			break;
		case SPI_OTP_LOCK:
			//printk("[%s:%d] num : %d\n",__FUNCTION__,__LINE__,*(u8 *)parg);
			err = write_lb(info,*(u8 *)parg);
			break;
		case SPI_OTP_LOCK_STATUS:
			err = read_lb(info,(u8 *)parg);
			//printk("[%s:%d] status : %02x\n",__FUNCTION__,__LINE__,*(u8 *)parg);
			break;
		case SPI_OTP_ERASE:
			err = erase_otp(info,*(u32 *)parg);
			break;
		default:  /* redundant, as cmd was checked against MAXNR */
			return -EINVAL;
	}

	return err;
}

static int spi_otp_user_copy(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg,
		int (*func)(struct inode *inode, struct file *file,
			unsigned int cmd, void *arg))
{
	char    sbuf[128];
	void    *mbuf = NULL;
	void    *parg = NULL;
	int     err  = -EINVAL;

	/*  Copy arguments into temp kernel buffer  */
	switch (_IOC_DIR(cmd)) {
		case _IOC_NONE:
			/*
			 * For this command, the pointer is actually an integer
			 * argument.
			 */
			parg = (void *) arg;
			break;
		case _IOC_READ:
		case _IOC_WRITE:
		case (_IOC_WRITE | _IOC_READ):
			if (_IOC_SIZE(cmd) <= sizeof(sbuf)) {
				parg = sbuf;
			} else {
				/* too big to allocate from stack */
				mbuf = kmalloc(_IOC_SIZE(cmd),GFP_KERNEL);
				if (NULL == mbuf)
					return -ENOMEM;
				parg = mbuf;
			}

			err = -EFAULT;
			if (copy_from_user(parg, (void __user *)arg, _IOC_SIZE(cmd)))
				goto out;
			break;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	/* call driver */
	mutex_lock(&ca_mutex);
	if ((err = func(file, cmd, parg)) == -ENOIOCTLCMD)
		err = -EINVAL;
	mutex_unlock(&ca_mutex);
#else
	/* call driver */
	if ((err = func(inode, file, cmd, parg)) == -ENOIOCTLCMD)
		err = -EINVAL;
#endif

	if (err < 0)
		goto out;

	/*  Copy results into user buffer  */
	switch (_IOC_DIR(cmd))
	{
		case _IOC_READ:
		case (_IOC_WRITE | _IOC_READ):
			if (copy_to_user((void __user *)arg, parg, _IOC_SIZE(cmd)))
			err = -EFAULT;
			break;
	}

out:
	kfree(mbuf);
	return err;
}

static int spi_otp_do_ioctl(struct inode *inode, struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	return spi_otp_user_copy(inode, filp, cmd, arg, spi_otp_ioctl);
}

ssize_t spi_otp_do_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	struct gxspi_otp_info *info = filp->private_data; 
	u32 address = filp->f_pos;
	u32 page_offset,len;
	u8 cmd[5],*rx;

//	printk("[%s:%d] f_pos : %llx\n",__FUNCTION__,__LINE__,filp->f_pos);

	if(EN25Q == info->type)
	{
		if(address >= FLASH_PAGESIZE)
			return -EINVAL;

		page_offset = address;
		
		address = info->eon25q_address | page_offset;
	}
	else if(GD25Q64B == info->type)
	{
		/* what page do we start with? */
		page_offset = address % FLASH_PAGESIZE;

		if (address < 0x100)
			address = 0x000 | page_offset;
		else if (address < 0x200)
			address = 0x100 | page_offset;
		else if (address < 0x300)
			address = 0x200 | page_offset;
		else
			address = 0x300 | page_offset;
	}
    else
    {
		/* what page do we start with? */
		page_offset = address % FLASH_PAGESIZE;

        if (address < 0x2000)
            address = 0x1000 | page_offset;
        else if (address < 0x3000)
            address = 0x2000 | page_offset;
        else
            address = 0x3000 | page_offset;
    }
	
	len = count;
	
	/* do all the bytes fit onto one page? */
	if (page_offset + count > FLASH_PAGESIZE)
	{
		len = FLASH_PAGESIZE - page_offset;
	}

//	printk("[%s:%d] address : %x, len : %d\n",__FUNCTION__,__LINE__,address,len);

	rx = vmalloc(len);
	if(rx == NULL)
		return -EINVAL;
	
	/* Wait until finished previous write command. */
	if (wait_till_ready(info))
	{	
		vfree(rx);
		return -EINVAL;
	}

	if(EN25Q == info->type)
	{
		eon_enter_otp(info,1);
		cmd[0] = 0x03;
		cmd[1] = address >> 16;
		cmd[2] = address >> 8;
		cmd[3] = address >> 0;
		otp_spi_tx_rx(info,cmd,4,NULL,0,rx,len);
		eon_enter_otp(info,0);
	}
	else
	{
		cmd[0] = 0x48;
		cmd[1] = address >> 16;
		cmd[2] = address >> 8;
		cmd[3] = address >> 0;
		cmd[4] = READ_DUMMY_DATA;
		otp_spi_tx_rx(info,cmd,5,NULL,0,rx,len);
	}

	if(copy_to_user(buf,rx,len))
	{
		vfree(rx);
		return -EFAULT;
	}

	vfree(rx);

	filp->f_pos += len;
	
	return len;
}

ssize_t spi_otp_do_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
	struct gxspi_otp_info *info = filp->private_data; 
	u32 address = filp->f_pos;
	u32 page_offset,len;
	u8 cmd[5],*tx;

//	printk("[%s:%d] f_pos : %llx\n",__FUNCTION__,__LINE__,filp->f_pos);

	if(EN25Q == info->type)
	{
		if(address >= FLASH_PAGESIZE)
			return -EINVAL;

		page_offset = address;
		
		address = info->eon25q_address | page_offset;
	}
	else if(GD25Q64B == info->type)
	{
		/* what page do we start with? */
		page_offset = address % FLASH_PAGESIZE;

		if (address < 0x100)
			address = 0x000 | page_offset;
		else if (address < 0x200)
			address = 0x100 | page_offset;
		else if (address < 0x300)
			address = 0x200 | page_offset;
		else
			address = 0x300 | page_offset;
	}
    else
    {
		/* what page do we start with? */
		page_offset = address % FLASH_PAGESIZE;

        if (address < 0x2000)
            address = 0x1000 | page_offset;
        else if (address < 0x3000)
            address = 0x2000 | page_offset;
        else
            address = 0x3000 | page_offset;
    }
		
	len = count;
	
	/* do all the bytes fit onto one page? */
	if (page_offset + count > FLASH_PAGESIZE)
	{
		len = FLASH_PAGESIZE - page_offset;
	}

//	printk("[%s:%d] address : %x, len : %d\n",__FUNCTION__,__LINE__,address,len);

	tx = vmalloc(len);
	if(tx == NULL)
		return -EINVAL;

	if(copy_from_user(tx,buf,len))
	{
		vfree(tx);
		return -EFAULT;
	}
	
	/* Wait until finished previous write command. */
	if (wait_till_ready(info))
	{	
		vfree(tx);
		return -EINVAL;
	}

	if(EN25Q == info->type)
		eon_enter_otp(info,1);
	
	write_enable(info);

	if(EN25Q == info->type)
		cmd[0] = 0x02;
	else
		cmd[0] = 0x42;
	cmd[1] = address >> 16;
	cmd[2] = address >> 8;
	cmd[3] = address >> 0;
	otp_spi_tx_rx(info,cmd,4,tx,len,NULL,0);

	/* Wait until finished previous write command. */
	if (wait_till_ready(info))
	{
        if(EN25Q == info->type)
			eon_enter_otp(info,0);
		return -EINVAL;
	}

    if(EN25Q == info->type)
		eon_enter_otp(info,0);

	vfree(tx);

	filp->f_pos += len;
	
	return len;
}

loff_t spi_otp_do_llseek(struct file *filp, loff_t off, int whence)
{
	struct gxspi_otp_info *info = filp->private_data; 
	loff_t newpos;

	switch(whence)
	{
		case 0: /* SEEK_SET */
			newpos = off;
			break;
		case 1: /* SEEK_CUR */
			newpos = filp->f_pos + off;
			break;
		case 2: /* SEEK_END */
			newpos = info->size + off;
			break;
		default: /* can't happen */
			return -EINVAL;
	}

	if (newpos < 0)
		return -EINVAL;

	filp->f_pos = newpos;
	
	return newpos;
}

struct file_operations spi_otp_fops = 
{
	.owner = THIS_MODULE,
	.open = spi_otp_do_open,
	.llseek = spi_otp_do_llseek,
	.read = spi_otp_do_read,
	.write = spi_otp_do_write,
	.ioctl = spi_otp_do_ioctl,
	.release = spi_otp_do_release,
};

static struct miscdevice spi_otp_miscdev = 
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "spiotp0",
	.fops = &spi_otp_fops,
};

static int spi_otp_remove(struct platform_device *pdev)
{
	struct gxspi_otp_info *info = platform_get_drvdata(pdev);

	if (!info)
		return -EINVAL;

	platform_set_drvdata(pdev, NULL);

	iounmap((void *)info->regs);

#ifdef REQUEST_MEM
	release_resource(info->ioarea);
	kfree(info->ioarea);
#endif

	kfree(info);

	misc_deregister(&spi_otp_miscdev);

	return 0;
}

static int spi_otp_probe(struct platform_device *pdev)
{
	struct gxspi_otp_info *info = NULL;    
	int err = 0;

	info = kmalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		printk("spi otp probe: no memory for info\n");
		err = -ENOMEM;
		goto err_out;
	}

	memset(info, 0, sizeof(*info));
	platform_set_drvdata(pdev, info);

	mutex_init(&info->lock);

	info->device = &pdev->dev;

#ifdef REQUEST_MEM
	info->ioarea = request_mem_region(0x00302000, SPI_SIZE, "SPI");
	if(info->ioarea == NULL)
	{
		dev_err(&pdev->dev, "Cannot reserve region\n");
		err = -ENXIO;
		goto err_out;
	}
#endif

	info->regs = ioremap(0x00302000, SPI_SIZE);
	if(info->regs == NULL)
	{
		dev_err(&pdev->dev, "Cannot map IO\n");
		err = -ENXIO;
		goto err_out;
	}

	err = misc_register(&spi_otp_miscdev);
	if (err) {   
		printk("spi otp probe: failed to register misc device\n");
		goto err_out;
	}

	return read_jedec(info);
	
err_out:
	err = spi_otp_remove(pdev);
	if (!err)
		err = -EINVAL;
	return err;
}

static struct platform_driver spi_otp_driver =
{
	.probe = spi_otp_probe,
	.remove = spi_otp_remove,
	.driver =
	{
		.name = "spiotp0",
		.owner = THIS_MODULE,
	}, 
};

static int __init spi_otp_init(void)
{
	printk("SPI OTP Driver, (c) 2013 Technologies\n");

	platform_device_register(&spi_otp_device);
	return platform_driver_register(&spi_otp_driver);
}

static void __exit spi_otp_exit(void)
{
	platform_driver_unregister(&spi_otp_driver);
	platform_device_del(&spi_otp_device);
}

module_init(spi_otp_init);
module_exit(spi_otp_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("linjj,others");
MODULE_DESCRIPTION("GX31X0 SPI OTP driver");
