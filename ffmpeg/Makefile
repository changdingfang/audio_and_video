CC = gcc

CUR_DIR = $(PWD)

FFMPEG_PATH = $(CUR_DIR)/ffmpegLib

CFLAGS = -Wall -O3 -fPIC -g -Wno-unknown-pragmas -Wno-reorder
	
INCLUDE = -I$(FFMPEG_PATH)/include

LDFLAGS = -L$(FFMPEG_PATH)/lib -lavformat -lavutil -lavcodec

SRC = test.c

objects = ${patsubst %.c, %.o, ${SRC}}

target = test

all:$(target)

$(target):$(objects)
	$(CC) $(objects) $(CFLAGS) $(INCLUDE) $(LDFLAGS) -o $(target)

$(objects): %.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE) $(LDFLAGS)

.PHONE : clean

clean:
	-rm -fr $(objects) $(target)

