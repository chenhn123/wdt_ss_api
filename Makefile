###########################################
# Simple Makefile for wdt_util
#
# chenhn
# 2017-01-20
# usage :
# make
# make clean
# make CONFIG=debug 
#  
###########################################

AP	= wdt_ct_oss

all: $(AP)

CXXFLAGS	?= -Wall -Os

ifeq ($(CONFIG),debug)
	CXXFLAGS += -g
endif

CPPOBJS  	= wdt_ct/wdt_ct_oss.o \
		  wdt_ct/w8755_funcs.o \
		  wdt_ct/w8760_funcs.o \
		  wdt_ct/w8790_funcs.o \
		  wdt_ct/wdt_dev_api.o \
		  wdt_ct/func_i2c.o \
		  wdt_ct/wif2_handler.o \
		  wdt_ct/func_hidraw.o 


COBJS = wdt_ct/hid_hidraw.o

OBJS      	= $(CPPOBJS) 
LIBS		= -pthread -lrt 
	  
$(AP): $(OBJS) $(COBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o $(AP)

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(AP)

.PHONY: clean
