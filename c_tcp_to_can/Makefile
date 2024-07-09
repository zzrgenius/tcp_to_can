CROSS_COMPILE ?=  

AS		    = $(CROSS_COMPILE)as
LD		    = $(CROSS_COMPILE)ld
CC		    = $(CROSS_COMPILE)gcc
CPP		    = $(CC) -E
AR		    = $(CROSS_COMPILE)ar
NM		    = $(CROSS_COMPILE)nm
######################################
DEBUG   =-DUSE_DEBUG
 
 
 ######################################
#
#
######################################
  
#target you can change test to what you want
#¹²Ïí¿âÎÄ¼þÃû£¬lib*.so
VERSION  = 1.00
TARGET  := tcp_to_can.exe
  
#compile and lib parameter

MKPATH = mkdir -p
	# Include directory
#INC = -I/home/zirun/scu/buildroot-2019.02.6/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include
#INC = -I/usr/include
# INC += -I./
# INC += -I./inc
# INC += -I/home/zirun/scu/mqtt/install_arm/include
INCLUDES = ./
INCLUDES += ./inc
 

# Library directory
LIBS_DIR = ./lib
  #LIB = -L/home/zirun/scu/buildroot-2019.02.6/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/lib
 
LIBS    := $(LIB) 


WARNINGS := -Wall -Wextra -Werror\
		-Wshadow -Wundef -Wmaybe-uninitialized   \
		-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized \
		 -Wno-missing-field-initializers  -Wno-parentheses -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default  \
		-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated  \
		-Wempty-body   -Wno-unused-parameter \
		-Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith -Wno-pointer-to-int-cast  -Wno-pointer-sign
#-Wstack-usage=4096

LDFLAGS :=  -lpthread -lrt  
LDFLAGS += $(addprefix -L,$(LIBS_DIR))
DEFINES :=
 #CFLAGS  := -g -Wall -O3 $(DEFINES) $(INCLUDE)
# CFLAGS := -O0  -g -Wall  $(WARNINGS) -std=c11 -D_GNU_SOURCE -D_THREAD_SAFE   -Dlinux  $(DEFINES)  
CFLAGS := -O1  -g -Wall   -std=c11    -Dlinux  $(DEFINES)  

CFLAGS += $(addprefix -I,$(INCLUDES))
CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H
SHARE   := -fPIC -shared 
  
#i think you should do anything here
  
#source file
vpath %.c src
SOURCE  := $(wildcard *.c)  $(notdir $(wildcard src/*.c) )
# vpath %.c mt_timer
# SOURCE  +=     $(notdir $(wildcard mt_timer/*.c) )

DIR_OBJS = ./obj
 
DIRS := $(DIR_OBJS)  

OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

OBJS := $(addprefix $(DIR_OBJS)/,$(OBJS))
 
.PHONY : everything objs clean veryclean rebuild
  
everything : $(TARGET)
define  mk-objdir
    @bash -c "if [ ! -e $(DIR_OBJS) ]; then ${MKPATH} $(DIR_OBJS); fi"
endef

 
all:  $(DIRS) $(TARGET)  
$(DIRS):
# $(MKPATH) $(DIR_OBJS)
	$(mk-objdir)
# all : ${TARGET}
# $(DIR_OBJS):
# 	${mk-objdir}
$(TARGET): $(OBJS)
	@echo linking...
#$(CC) $^ $(CFLAGS) $(LDFLAGS)      -o $@
	$(CC)    $(LDFLAGS) -o $@ $^ 

$(DIR_OBJS)/%.o:%.c | $(DIRS) 
	@echo compile $<...
	$(CC) $(CFLAGS)    -c $< -o $@

clean:
	@echo try to clean...
	rm -rf *.o $(DIR_OBJS)/*.o  
 
