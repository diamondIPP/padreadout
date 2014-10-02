## makefile for decode.C
DEBUG       := -g3 -Wall
IDIR 	    := include
SDIR        := src
ODIR		:= obj

ALL_SRC     := $(wildcard src/*.cc)
SOURCES     := $(filter-out decode_make.cc, $(ALL_SRC))
#src/decode_make.cc
OBJECTS     := $(SOURCES:.cc=.o)
OBJECTS     := $(patsubst src/%.cc,$(ODIR)/%.o,$(SOURCES)) 
EXECUTABLE  := decode
CC          := g++
LDFLAGS     := $(DEBUG)
ROOTLIBS    := $(shell root-config --glibs)
ROOTCFLAGS  := $(shell root-config --cflags)
LIBS        := $(ROOTLIBS)
INCLUDES    := -I./$(IDIR) -I$(ROOTSYS)/include
CXXFLAGS    := -c $(DEBUG) -fPIC -fno-var-tracking -Wno-deprecated -D_GNU_SOURCE -O2 $(INCLUDES) $(ROOTCFLAGS) -DDEV $(DEBUG)
INC			:= -I$(IDIR)

RM          := rm



# Rules ====================================
#all: $(SOURCES) $(EXECUTABLE)
all: decode
	$ @echo "SOURCES $(SOURCES)"
	$ @echo "OBJECTS $(OBJECTS)"

decode:  $(OBJECTS)
	$ @echo "SOURCES $(SOURCES)"
	$ @echo "OBJECTS $(OBJECTS)"
	$ @echo "@       $@"
	$ @echo "<       $<"
	$ @echo "^       $^"
	$(CC) $(LDFLAGS) -o $@ $^  $(LIBS)

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	    mkdir -p $(OBJDIR)


$(ODIR)/%.o:  $(SDIR)/%.cc $(IDIR)/%.hh
	mkdir -p $(ODIR)
	$ #$ @echo "CREATING OBJECT $%
	$ $(CC) -c $(INC) -o $@  $(CXXFLAGS) -c $< 
	#$ $(CC) -c $(INC) -o $@ $< $(CFLAGS)
## $(EXECUTABLE): $(OBJECTS)
## 	$(CC) $(LDFLAGS) -o $@ $<  $(LIBS)

clean:
	find $(ODIR) -name '*.o' -exec $(RM) -v {} ';' 
	$(RM) decode

