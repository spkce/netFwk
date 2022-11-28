ROOT_DIR = $(shell pwd)
DIR_LIB ?=  $(ROOT_DIR)/lib
#DIR_INC ?
CC ?= gcc
CXX ?= g++
AR ?= ar

INC := -I Include -I Include/server -I Include/terminal -I Include/protocol -I Include/client -I library/Infra/Include

CFLAGS = -L./lib -Wall  -lpthread -lrt -lstdc++ -std=c++11 -ldl -lInfra -g

SRC_DIR += \
./		\
./src/server \
./src/Infra \
./src/terminal \
./src/protocol \
./src/client \

DIR_OBJ := ./obj
TARGET := a.out

.PHONY: all CHECKDIR submodules clean 

all: CHECKDIR $(TARGET)

CPP_SRCS += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))
C_SRCS += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c))

CPP_OBJS :=  $(patsubst %.cpp, %.o, $(CPP_SRCS))
C_OBJS :=  $(patsubst %.c, %.o, $(C_SRCS))


COBJS   := $(addprefix $(DIR_OBJ)/, $(C_OBJS))
CPPOBJS := $(addprefix $(DIR_OBJ)/, $(CPP_OBJS))

$(COBJS): $(DIR_OBJ)/%.o: %.c
	@echo "CC $(notdir $@)"
	@mkdir -p $(dir $(@))
	@$(CC) $(CFLAGS) -o $@ -c $< $(INC)
	
$(CPPOBJS): $(DIR_OBJ)/%.o: %.cpp
	@echo "CC $(notdir $@)"
	@mkdir -p $(dir $(@))
	@$(CXX) $(CFLAGS) -o $@ -c $< $(INC)

$(TARGET): submodules $(COBJS) $(CPPOBJS)
	@$(CC) $(COBJS) $(CPPOBJS) -g -o $@ $(INC) $(CFLAGS) 


submodules:
	@make CC="$(CC)" CXX="$(CXX)" AR="$(AR)" DIR_LIB="$(DIR_LIB)" -C library/Infra

CHECKDIR:
	@mkdir -p $(DIR_OBJ)
	

clean:
	@make clean CC="$(CC)" CXX="$(CXX)" AR="$(AR)" DIR_LIB="$(DIR_LIB)" -C library/Infra
	@rm -rf ${DIR_OBJ}
ifeq ($(TARGET), $(wildcard $(TARGET)))
	@rm $(TARGET)
endif