# Compiler and archiver
CXX = g++
AR = ar

# Flags
CXXFLAGS = -Wall -O2 -std=c++11
INCLUDES = -I.

# Files
LIB_NAME = libwdtss.a
LIB_OBJS = wdt_ss_api.o \
           wdt_ct/wdt_ct.o \
           wdt_ct/w8755_funcs.o \
	   wdt_ct/w8760_funcs.o \
           wdt_ct/w8790_funcs.o \
           wdt_ct/wdt_dev_api.o \
           wdt_ct/func_i2c.o \
           wdt_ct/wif2_handler.o \
	   wdt_ct/func_hidraw.o 


COBJS = wdt_ct/hid_hidraw.o


TEST_SRC = test.cpp
TEST_BIN = test_app

# Default target
all: $(LIB_NAME) $(TEST_BIN)

# Build static library from object file
$(LIB_NAME): $(LIB_OBJS) $(COBJS)
	$(AR) rcs $@ $^

# Compile test.cpp and link with static library
$(TEST_BIN): $(TEST_SRC) $(LIB_NAME)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -L. -lwdtss $(INCLUDES) -o $(TEST_BIN)

# Compile source files into object files
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean everything
clean:
	rm -f *.o $(LIB_NAME) $(TEST_BIN)

