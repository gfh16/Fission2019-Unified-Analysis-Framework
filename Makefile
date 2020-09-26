#
# setup control
#
OutPut   := test1
TOP      := $(shell pwd)/
OBJ      := $(TOP)obj/
BIN      := $(TOP)bin/
SRC      := $(TOP)src/
INCLUDE  := $(TOP)include/

CPP       = g++
CPPFLAGS  = -O -Wall -fPIC -I$(INCLUDE)
ROOTLIB   = $(shell root-config --cflags --glibs)
ROOTINC   = -I$(shell root-config --incdir)
ROOTBIN   = $(shell root-config --bindir)
SYSLIB    = -lstdc++

############## Make Executables ####################################
all:	$(OutPut)

$(OutPut)	: $(patsubst $(SRC)%.C,$(OBJ)%.o,$(wildcard $(SRC)*.C))
	$(CPP) $^ $(CPPLIBS) -o $(BIN)$(notdir $@) $(ROOTLIB) $(SYSLIB) $(ROOTINC) $(ROOTBIN)
	@echo

######################################################
$(OBJ)%.o : 	$(SRC)%.C
	$(CPP)  $(CPPFLAGS) -c $(SRC)$(notdir $<) -o $(OBJ)$(notdir $@) $(ROOTLIB) $(SYSLIB) $(ROOTINC) $(ROOTBIN)
	@echo

.PHONY:clean
clean:
	rm -f $(OBJ)*.o
	rm -f $(BIN)*
