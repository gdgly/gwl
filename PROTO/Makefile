.SUFFIXES : .x .o .c .s
ROOT = /usr/local/arm_linux_4.8/usr
LIB =-L$(ROOT)/lib -L/usr/local/arm_linux_4.8/lib/ -L/usr/local/arm_linux_4.8/arm-nuvoton-linux-uclibceabi/lib/
#INC :=-I$(ROOT)/include -I/usr/local/arm_linux_4.8/arm-nuvoton-linux-uclibceabi/include/
INC :=

#CC=arm-linux-gcc -O2 $(INC)
CC=arm-none-linux-gnueabi-gcc
#WEC_LDFLAGS=$(LIB)
WEC_LDFLAGS=
#STRIP=arm-linux-strip
STRIP=strip
TARGET = Server
SRCS := Mrs_fw_proto376_1.c Server_com.c file.c ProtocolD645.c  rtc.c	gpio.c Tools.c EventRecord.c SQLite.c queue.c  MESamBase.c MESamComMgr.c util.c para.c AcSample.c ListTimer.c local_fun.c
LIBS= -lpthread -lsqlite3 -lc -lgcc
#$(STRIP) $(TARGET)
all:
	$(CC) $(WEC_LDFLAGS) $(SRCS) -g -o  $(TARGET) $(LIBS)

clean:
	rm -fv *.o
	rm -fv ${TARGET}
	rm -fv *.gdb
 

 
