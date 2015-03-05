
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/ioctl.h>

#define OTP_START_ADDR	(0x1000)
#define OTP_END_ADDR	(0x3fff)
#define OTP_BLOCK_SIZE	(256)
#define OTP_BLOCK_NUM	(3)

/*
 [0x9F] 00000000  ef 40 18                                        .@.
 [0x90] 00000000  ef 17                                           ..
 [0x4B] 00000000  d2 61 c0 00 27 35 43 29                         .a..'5C)
 [0xAB] 00000000  17                                              .
 */
struct spi_id {
	unsigned char	cmd;
	unsigned char 	code[8];	
	unsigned char	len;
};

#define SPI_ID              		_IOWR('o', 1, struct spi_id)
#define SPI_OTP_LOCK             	_IOW('o', 2,unsigned char)
#define SPI_OTP_LOCK_STATUS      	_IOR('o', 3,unsigned char)
#define SPI_OTP_ERASE            	_IOW('o', 4,unsigned int)

static void dump(unsigned char *buf, int size)
{
    int len, i, j, c;
#define PRINT(...) do { printf(__VA_ARGS__); } while(0)

    for(i=0;i<size;i+=16) {
        len = size - i;
        if (len > 16)
            len = 16;
        PRINT("%08x ", i);
        for(j=0;j<16;j++) {
            if (j < len)
                PRINT(" %02x", buf[i+j]);
            else
                PRINT("   ");
        }
        PRINT(" ");
        for(j=0;j<len;j++) {
            c = buf[i+j];
            if (c < ' ' || c > '~')
                c = '.';
            PRINT("%c", c);
        }
        PRINT("\n");
    }
//	PRINT("\n\n");
#undef PRINT
}


static void spi_otp_dump(int fd, unsigned int addr, char *buf, int len)
{
	int ret = 0;
	
	fprintf(stderr,"read addr: 0x%4X\n", addr);
	lseek(fd,addr,SEEK_SET);
	memset(buf,0,len);
	ret = read(fd,buf,len);
	if(ret != len)
		fprintf(stderr,"out of block ####ret = %d\n",ret);
	if(ret > 0)
		dump(buf,ret);

}


static void spi_otp_read(int fd, unsigned int addr, char *buf, int len)
{
	int ret = 0;
	int i = 0;
	
	printf("0x%4X ", addr);
	lseek(fd,addr,SEEK_SET);
	memset(buf,0,len);
	ret = read(fd,buf,len);
	if(ret != len)
		fprintf(stderr,"out of block ####ret = %d\n",ret);
	if(ret > 0){
		for(i = 0; i < ret; i++){
			printf("%02x ",buf[i]);		
		}
		printf("\n");
	}
}

static void spi_otp_write(int fd, unsigned int addr, char *buf, int len)
{
	int ret = 0;
	int i = 0;
	
	printf("0x%4X ", addr);
	for(i = 0; i < len; i++)
	{
		printf("%2X", buf[i]);
	}
	printf("\n");
	lseek(fd,addr,SEEK_SET);
	ret = write(fd,buf,len);
	if(ret != len)
		fprintf(stderr,"write opt error. \n");

}

static void spi_otp_erase(int fd, unsigned int addr)
{
	
	int err = 0;

	printf("erase addr: 0x%4X\n", addr);
	err = ioctl(fd,SPI_OTP_ERASE,&addr);
	if(err != 0)
		perror("SPI OTP ERASE Error\n");

}

static void spi_otp_lock(int fd, int block)
{
	int err = 0;

	err = ioctl(fd,SPI_OTP_LOCK,&block);
	if(err != 0)
		perror("SPI OTP Lock Error\n");

}

static void spi_otp_status(int fd)
{
	int err = 0;
	unsigned char status;

	printf("Bit order :   7   6   5   4   3   2  1    0\n");
	printf("Bit define: SUS CMP LB3 LB2 LB1 (R) QE SRP1\n");
	printf("                      ^   ^   ^            \n");

	err = ioctl(fd,SPI_OTP_LOCK_STATUS,&status);
	if(err != 0)
		perror("SPI_OTP_LOCK_STATUS\n");
	else
		printf("otp lock status : 0x%02X\n",status);
}

static void spi_otp_id(int fd)
{
	struct spi_id id;

	printf("Read IDs: JEDEC ID, Manufacture/Device ID, Unique ID\n");
	memset(&id,0,sizeof(struct spi_id));
	id.cmd = 0x9f;
	ioctl(fd,SPI_ID,&id);
	dump(id.code,id.len);	

	memset(&id,0,sizeof(struct spi_id));
	id.cmd = 0x90;
	ioctl(fd,SPI_ID,&id);
	dump(id.code,id.len);	

	memset(&id,0,sizeof(struct spi_id));
	id.cmd = 0x4b;
	ioctl(fd,SPI_ID,&id);
	dump(id.code,id.len);	

	memset(&id,0,sizeof(struct spi_id));
	id.cmd = 0xab;
	ioctl(fd,SPI_ID,&id);
	dump(id.code,id.len);	

}

static int check_otp_addr(int addr)
{
//	printf("addr = 0x%X \n",addr);
        if(addr < OTP_START_ADDR || addr > OTP_END_ADDR){
		fprintf(stderr,"otp start addr wrong!\n");
		return 1;
	}else{
		return 0;
	}
}

static char *usage_tip = 
        "\n"
        "-a addr 	- otp start addr\n"
	"-r num		- read flash otp\n"
        "-w value	- write flash otp \n"
        "-e 		- erase flash otp\n"
        "-d 		- dump flash otp\n"
        "-l block	- lock flash otp\n"
        "-g 		- get lock status\n"
        "-i 		- get flash uid\n"
        "-h 		- print this help message\n";

static void usage(void)
{
        printf("%s",usage_tip);
}

typedef enum otp_op {
	OP_READ,
	OP_WRITE,
	OP_ERASE,
	OP_DUMP,
	OP_LOCK,
	OP_STATUS,
	OP_ID,
}flag_op_t;


int main(int argc,char *argv[])
{
	static unsigned char buf[OTP_BLOCK_SIZE];
	flag_op_t  spi_op;		
	int fd,opt;
	unsigned int address = 0;
	unsigned int value = 0;
	unsigned int number = 0;
	unsigned int block = 0;
	
        if(argc < 2){
                usage();
                return 0;
        }

	memset(buf, 0, OTP_BLOCK_SIZE);
        //printf("in fun %s line %d\n",__FUNCTION__,__LINE__);
        while((opt = getopt(argc,argv,"a:r:w:edl:gih")) != -1){
                switch(opt){
                      case 'a':
                                address = strtoul(optarg,NULL,0);
				if (check_otp_addr(address) > 0) 
					return 0;
                               break;
                        case 'r':
                                number = strtoul(optarg,NULL,0);
                                if(number < 0 || number > OTP_BLOCK_SIZE){
                                	fprintf(stderr,"otp read len wrong!\n");
					return 0;
				}
				spi_op = OP_READ;
                                break;
                        case 'w':
                                spi_op = OP_WRITE;
                                value = strtoul(optarg,NULL,0);
                                break;
                        case 'e':
				spi_op = OP_ERASE;
                                break;
                        case 'd':
				spi_op = OP_DUMP;
                                break;
                        case 'l':
                                block = strtoul(optarg,NULL,0);
                                if(block < 0 || block > OTP_BLOCK_NUM){
                                	fprintf(stderr, "otp block order wrong!\n");
					return 0;
				}
				spi_op = OP_LOCK;
                                break;
                        case 'g':
				spi_op = OP_STATUS;
                                break;
                        case 'i':
				spi_op = OP_ID;
                                break;
			case 'h':
                        default:
                                usage();
                                return 0;
                }
        }

	fd = open("/dev/spiotp0",O_RDWR);
	if (fd < 0) 
	{
		printf("/dev/spiotp0 open fail!\n");
		goto out;
	}

	switch(spi_op){
		case OP_DUMP:
			if(check_otp_addr(address) > 0)
				goto out;	
			spi_otp_dump(fd, (address & 0xF000), buf, OTP_BLOCK_SIZE);
			break;
		case OP_READ:
			if(check_otp_addr(address) > 0)
				goto out;	
			if(number > 32){
				fprintf(stderr, "use dump option.\n");
				goto out;
			}
			spi_otp_read(fd, address, buf, number);
			break;
		case OP_WRITE:
			if(check_otp_addr(address) > 0)
				goto out;	
			spi_otp_write(fd, address, (char*)&value, sizeof(value));
			break;
		case OP_ERASE:
			if(check_otp_addr(address) > 0)
				goto out;	
			spi_otp_erase(fd, (address & 0xF000));
			break;
		case OP_LOCK:
			spi_otp_lock(fd, block);
			break;
		case OP_STATUS:
			spi_otp_status(fd);
			break;
		case OP_ID:
			spi_otp_id(fd);
			break;
		default:
			printf("unkown how to operate.\n");	
	}

out:
	close(fd);

	exit(0);
}

