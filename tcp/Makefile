#CFLAGS= -Wall -O2
CFLAGS=  -O -fPIC -shared 
CC=mipsel-linux-gcc
COMMAND=-L/yuan/pro/https/lib -lbngo_info -lm 
obj += tcp_interface.o cJSON.o
TARGET = libtcp_interface.so
main: $(obj)
	$(CC) $(CFLAGS) -o $(TARGET) $(obj)

clean:
	rm -rf *.o