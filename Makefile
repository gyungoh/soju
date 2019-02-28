.SUFFIXES : .c .o

IMDKIT_PATH := ../xcb-imdkit

TARGET = soju

SRCS = 	soju.c \
	imdkit.c \
	message.c \
	parser.c \
	ximproto.c \
	protocolhandler.c \
	common.c

VPATH = $(IMDKIT_PATH)/src

OBJS = $(SRCS:.c=.o)

CFLAGS += -O3 $(patsubst %,-I%,$(subst :, ,$(VPATH))) \
	-I/home/pi/Desktop/234/usr/include

LFLAGS = -L/home/pi/Desktop/234/usr/lib/arm-linux-gnueabihf \
	-lxcb \
	-l:libxcb-keysyms.a \
	-lxcb-util
#	-l:libxcb-util.a

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LFLAGS)
	strip $(TARGET)

clean:
	rm $(OBJS) $(TARGET)
