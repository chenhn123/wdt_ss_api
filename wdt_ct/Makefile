###########################################
# Simple Makefile for wdt_util
#
# Randy L.
# 2025-09-23
# usage :
# make
# make clean
# make CONFIG=debug 
#  
###########################################

AP	= wdt_util

all: $(AP)

CXXFLAGS	?= -Wall -Os

ifeq ($(CONFIG),debug)
	CXXFLAGS += -g
endif


CPPOBJS  	= wdt_ct.o \
		  w8755_funcs.o \
		  w8760_funcs.o \
		  w8790_funcs.o \
		  wdt_dev_api.o \
		  func_i2c.o \
		  wif2_handler.o \
		  func_hidraw.o 


COBJS = hid_hidraw.o

OBJS      	=  $(CPPOBJS) $(COBJS)
LIBS		= -lrt
	  
$(AP): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o $(AP)

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(AP)

.PHONY: clean
