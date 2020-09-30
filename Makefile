##################################################
##          My first makefile
##################################################

DIR_INC := include/
DIR_SRC := src/
DIR_OBJ := obj/
DIR_BIN := bin/

CPP        := g++
ROOTCFLAGS := -c -g -Wall `root-config --cflags`
ROOTGLIBS  := `root-config --glibs`
ROOTLIBS   := -lSpectrum -lMathMore

TARGET     := QualityCheck
BIN_TARGET := $(DIR_BIN)$(TARGET)

PATHSRCS   := ./Step2_QualityCheck/QualityCheck.C
SOURCES    := $(shell find $(DIR_SRC) -name "*.C") $(PATHSRCS)
INCLUDES   := $(shell find $(DIR_INC) -name "*.h")
OBJECTS    := $(patsubst %.C, %.o, $(SOURCES))

all: $(BIN_TARGET)


$(BIN_TARGET): $(OBJECTS)
	$(CPP) $(OBJECTS) $(ROOTGLIBS) $(ROOTLIBS) -o $@
	rm -f $(OBJECTS)


%.o : %.C $(INCLUDES)
	$(CPP) $(ROOTCFLAGS) $< -o $@


.PHONY: clean
clean:
	rm ./*~ ${BIN_TARGET} ./$(DIR_SRC)*.o
