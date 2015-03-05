
CC	= csky-linux-gcc

OBJ = otp_test

CFLAGS += -I../source

all : $(OBJ).o 
	$(CC) -o $(OBJ) $(OBJ).o --static -lpthread

clean:
	@@rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions $(OBJ)
