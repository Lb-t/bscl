######################################
# platform
######################################

OS := $(shell uname -s)

######################################
# target
######################################
TARGET = Server

#######################################
# toolchains
#

TOOLCHAINPATH = 
#arm-none-eabi- 
PREFIX = 
CC = $(BINPATH)$(PREFIX)gcc



#######################################
# paths
#######################################
# source path
SOURCES_DIR =  \
container \
common \
algorithm 


TEST_DIR = test
# output path
OUTPUT_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  $(wildcard $(patsubst %,%/*.c,$(SOURCES_DIR)))
objects = $(patsubst %.c,%.o, $(C_SOURCES))
# ASM sources
ASM_SOURCES =  \



 
#######################################
# CFLAGS
#######################################
CFLAGS = 
ifeq ($(conf),test)
CFLAGS +=-DUNIT_TESTING
endif

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-I.  \
-Iinclude \
-Icontainer \
-Icommon \
-Ialgorithm \
-Iio 


#######################################
# LDFLAGS
#######################################

# libraries
ifeq ($(OS),Linux)
CFLAGS += -Wl,-rpath,'$ORIGIN'
LIBS = -lpthread -lspeex -lportaudio -lrt -lm -lasound
LIBDIR = \
-Lthirdpart/portaudio/lib/linux \
-Lthirdpart/speex/lib/linux 
LIBS_CPY = \
thirdpart/speex/lib/linux/libspeex.so \
thirdpart/portaudio/lib/linux/libportaudio.so
else
ifeq ($(findstring MINGW, $(OS)),MINGW)
LIBS = -lwinmm -lws2_32 -lpthread -lspeex.dll -lportaudio.dll
LIBDIR = \
-Lthirdpart/portaudio/lib/win32 \
-Lthirdpart/speex/lib/win32 
LIBS_CPY = \
thirdpart/speex/lib/win32/libspeex.dll \
thirdpart/portaudio/lib/win32/libportaudio.dll
else
#unknow OS
$(error error is unknow OS $(OS))
endif
endif

.PHONY:all
all:$(deps) $(objects)
	-mkdir $(OUTPUT_DIR) 
	$(CC) -o $(OUTPUT_DIR)/$(TARGET) $(objects) $(C_INCLUDES) $(LIBDIR) $(LIBS)
	cp $(LIBS_CPY) $(OUTPUT_DIR)
# deps
deps := $(patsubst %.c,%.d, $(C_SOURCES))
sinclude $(deps)	

$(objects): %.o: %.c
	$(CC) -o $@  -c $< $(C_INCLUDES) $(CFLAGS)

$(deps):%.d:%.c
	echo "create deps"
	$(CC) -MM $< $(C_INCLUDES) > $@ 

# test
.PHONY:test
test:$(deps) $(objects) $(TEST_DIR)/test_vector.o
	-mkdir $(OUTPUT_DIR) 
	$(CC) -o $(OUTPUT_DIR)/test_vector $(TEST_DIR)/test_vector.o $(objects) $(C_INCLUDES)
#$(CC) -o $(OUTPUT_DIR)/test_vector $(TEST_DIR)/test_vector.o $(objects) $(C_INCLUDES) -Ltest/cmockery -lcmockery

$(TEST_DIR)/test_vector.o:$(TEST_DIR)/test_vector.c
	$(CC) -o $@ -c $< $(C_INCLUDE)

#######################################
# clean up
#######################################

clean:
	-rm -f $(deps) $(objects)
	-rm -rf $(OUTPUT_DIR)
  

# *** EOF ***
