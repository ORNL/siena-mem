# Copyright 2013-2018 UT-Battelle, LLC.
# See LICENSE.txt for more information.

#Optional Memory Simulators
USE_RAMULATOR=yes
USE_DRAMSIM=yes
USE_NVDIMMSIM=yes
DEBUG=no

SRC_DIR := engine
OBJ_DIR := obj
BIN_DIR := bin
MAIN := $(SRC_DIR)/main.cpp
EXE  := main
DIR  := ${CURDIR}
ASPEN_ROOT=$(DIR)/aspen
DRAMSIM_ROOT=$(DIR)/dramsim2
RAMULATOR_ROOT=$(DIR)/ramulator
NVDIMMSIM_ROOT=$(DIR)/nvdimmsim/src


ASPEN_LIB=$(ASPEN_ROOT)/lib/libaspen.a
DRAMSIM_LIB=$(DRAMSIM_ROOT)/libdramsim.a
RAMULATOR_LIB=$(RAMULATOR_ROOT)/libramulator.a
NVDIMMSIM_LIB=$(NVDIMMSIM_ROOT)/src/libnvdsim.a
DEP_LIBS=$(ASPEN_LIB)


CXX=g++
CXXFLAGS=-std=c++11 -fPIC -O3 -g -Wall -Wno-format -Wno-sign-compare -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-private-field -DHAVE_CXXABI_H
INCFLAGS=-I$(SRC_DIR) -I$(ASPEN_ROOT)/aspen
LIBS=-L$(ASPEN_ROOT)/lib -laspen 
AR=ar
RM=rm -rf


SRCS := $(filter-out $(MAIN) $(SRC_DIR)/ar_ramulator.cpp  $(SRC_DIR)/ar_dramsim.cpp $(SRC_DIR)/ar_nvdsim.cpp , $(wildcard $(SRC_DIR)/*.cpp))

ifeq ($(USE_RAMULATOR),yes)
CXXFLAGS+=-DRAMULATOR
INCFLAGS+=-I$(RAMULATOR_ROOT)/src
LIBS+=-L$(RAMULATOR_ROOT) -lramulator
SRCS += $(SRC_DIR)/ar_ramulator.cpp
DEP_LIBS += $(RAMULATOR_LIB)
endif

ifeq ($(USE_DRAMSIM),yes)
CXXFLAGS+=-DDRAMSIM
INCFLAGS+= -I$(DRAMSIM_ROOT)
LIBS+=-L$(DRAMSIM_ROOT)  -ldramsim
SRCS +=	$(SRC_DIR)/ar_dramsim.cpp
DEP_LIBS += $(DRAMSIM_LIB)
endif

ifeq ($(USE_NVDIMMSIM),yes)
CXXFLAGS+=-DNVDIMMSIM
INCFLAGS+=-I$(NVDIMMSIM_ROOT)
LIBS+=-L$(NVDIMMSIM_ROOT) -lnvdsim
SRCS += $(SRC_DIR)/ar_nvdsim.cpp
DEP_LIBS += $(NVDIMMSIM_LIB)
endif

OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))


#-DDEBUG_CORE -DDEBUG_CACHE -DDEBUG_DATA  -DDEBUG_TRACE -DDEBUG_MEMSIM -DDEBUG_CACHESIM -DDEBUG_PARSE
#-DCACHESIM   -DMEMSIM 
#-DSAVE_TRACE -DPROFILING -DVERBOSE -DDUMP_STATE
CXXFLAGS+= -DPROFILING -DVERBOSE
ifeq ($(DEBUG),yes)
CXXFLAGS+= -DDEBUG_MEMSIM -DDEBUG_CACHESIM -DDEBUG_CORE
endif

.PHONY: all test clean veryclean 

all:  $(DEP_LIBS)  $(OBJS) $(MAIN)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -o $(BIN_DIR)/$(EXE) $(MAIN) $(OBJS) $(LIBS)
	@(echo "#########################")
	@(echo "Compilation is done!")
	@(echo "Run 'make test'")
	@(echo "#########################")

test:
	export RAMULATOR_ROOT=$(DIR)/models/machine/config;export NVDIMMSIM_ROOT=$(DIR)/models/machine/config;$(BIN_DIR)/$(EXE) test/stream.aspen test/hms.aspen test/mapping.aspen

check:
	@(cd testsuites; ./run_tests.sh)
	@(echo "#########################")
	@(echo "Test is done")
	@(echo "#########################")


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c -o $@ $<


$(ASPEN_LIB):$(ASPEN_ROOT)/configure
	@(cd $(ASPEN_ROOT) && ./configure --prefix=$(ASPEN_ROOT) && make aspen)

$(ASPEN_ROOT)/configure:
	@(cd $(ASPEN_ROOT) && autoconf -v)


$(DRAMSIM_LIB):$(DRAMSIM_ROOT)
	@(cd $(DRAMSIM_ROOT) && make libdramsim.a)

$(RAMULATOR_LIB):$(RAMULATOR_ROOT)
	@(cd $(RAMULATOR_ROOT) && make libramulator.a)

$(NVDIMMSIM_LIB):$(NVDIMMSIM_ROOT)
	@(cd $(NVDIMMSIM_ROOT) && make libnvdsim.a)


#
# Clean targets
#

clean:
	@$(RM) $(BIN_DIR) $(OBJ_DIR) *.d *.log *~ */*~

distclean:
	-@(cd $(ASPEN_ROOT) && make clean && make depclean && make distclean && cd ..)
	-@(cd $(RAMULATOR_ROOT) && make clean && cd ..)
	-@(cd $(NVDIMMSIM_ROOT) && make clean && cd ../..)
	-@(cd $(DRAMSIM_ROOT) && make clean && cd ..)
	-@$(RM) $(OBJ_DIR) $(BIN_DIR) *.d *.log *~ */*~
