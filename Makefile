G_DEBUG=-g
CFLAGS=-O2 -Wall -std=c++11 
CC=g++
LIB=-lpthread -lmysqlclient
EXEC=example
SRC:=$(wildcard *.cpp)
SRCS:=$(notdir $(SRC))                 
OBJS:=$(patsubst %.cpp,%.o,$(SRCS)) 

# 下面判断是debug还是release
# 
DEBUG:=1
ifeq ($(DEBUG),1)
EXEC_DIR:=$(DEBUG_DIR)
CFLAGS:=$(CFLAGS) $(FLAG_DEBUG)
else
EXEC_DIR:=$(RELEASE_DIR)
endif

all: $(EXEC)


$(EXEC) : $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $(OBJS)  $(LIB)
	
$(OBJS): %.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXEC)
	rm -rf $(OBJS) 

.PHONY: clean

