######################################
# platform
######################################

OS := $(shell uname -s)


#######################################
# toolchains
#

CC = gcc
AR = ar


#######################################
# paths
#######################################
# source path
SOURCES_DIR =  src 
TEST_DIR =  test
# output path
OUTPUT_DIR = build

TARGET =vlcplatform

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

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \


# C includes
C_INCLUDES =  \
-I.  \
-Iinclude \

#######################################
# LDFLAGS
#######################################

# libraries


.PHONY:all
all:$(deps) $(objects)
	@-mkdir $(OUTPUT_DIR) 
	@echo "build lib$(TARGET).a lib$(TARGET).dll lib$(TARGET).dll.a"
	@$(AR) rcs $(OUTPUT_DIR)/lib$(TARGET).a $(objects)
	@$(CC)  -shared -fPIC -o $(OUTPUT_DIR)/lib$(TARGET).dll $(objects) -Wl,--out-implib,$(OUTPUT_DIR)/lib$(TARGET).dll.a,--output-def,$(OUTPUT_DIR)/lib$(TARGET).def -lws2_32 -lwinmm 
# deps
deps := $(patsubst %.c,%.d, $(C_SOURCES))
sinclude $(deps)	

$(objects): %.o: %.c
	@echo "compile $<"
	@$(CC) -o $@  -c $< $(C_INCLUDES) $(CFLAGS)

$(deps):%.d:%.c
	@$(CC) -MM $< $(C_INCLUDES) > $@ 

#######################################
# clean up
#######################################

clean:
	-rm -f $(deps) $(objects)
	-rm -rf $(OUTPUT_DIR)
  

# *** EOF ***
