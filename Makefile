##################################################
##          My first makefile
##################################################

DIR_INC := include/
DIR_SRC := src/
DIR_OBJ := obj/
DIR_BIN := bin/
DIR_EXE := exec/

CPP        := g++
ROOTCFLAGS := -c -g -Wall `root-config --cflags`
ROOTLIBS   := `root-config --glibs --libs`
ROOTLIBS   += -lSpectrum
ROOTLIBS   += -lMathMore

# exec_DataAnalysis.C
# exec_QualityCheck.C
# exec_SSDDataAnlysis.C

TARGET     := exec_SSDDataAnlysis
PATHSRCS   := exec/exec_SSDDataAnlysis.C

SOURCES    := $(shell find $(DIR_SRC) -name "*.C") $(PATHSRCS)
INCLUDES   := $(shell find $(DIR_INC) -name "*.h")
OBJECTS    := $(patsubst %.C, %.o, $(SOURCES))


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CPP) $(OBJECTS) $(ROOTLIBS) -o $@
	rm -f $(OBJECTS)


%.o : %.C $(INCLUDES)
	$(CPP) $(ROOTCFLAGS) $< -o $@


.PHONY: clean
clean:
	rm ./*~ ${TARGET} ./$(DIR_SRC)*.o ./$(DIR_EXE)*.o
