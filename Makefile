.SUFFIXES : .c .o

TARGET = soju

SRCS = 	soju.c han.c \
	imdkit.c message.c parser.c ximproto.c protocolhandler.c common.c
	
VPATH = $(IMDKIT_PATH)/src

OBJS = $(SRCS:.c=.o)

CFLAGS += -O3 $(patsubst %,-I%,$(subst :, ,$(VPATH)))

LFLAGS += -lxcb -l:libxcb-keysyms.a -lxcb-util

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LFLAGS)
	strip $(TARGET)

clean:
	rm $(OBJS) $(TARGET)
