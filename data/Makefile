## makefile for decode.C
ANALYSIS	:= ../decode
INPUT		:= input
OUTPUT		:= output
SOURCES     := $(wildcard input/run_2014*.dat)
OBJECTS     := $(patsubst $(INPUT)/%.dat,$(OUTPUT)/%.root,$(SOURCES))

RM          := rm
MV			:= mv



# Rules ====================================
#all: $(SOURCES) $(EXECUTABLE)
all: analysis

analysis: $(OBJECTS) $(ANALYSIS)

$(OUTPUT)/%.root:  $(INPUT)/%.dat $(ANALYSIS)
	$(ANALYSIS) -i $< 
	$(MV) $(patsubst $(OUTPUT)/%.root, $(INPUT)/%.root,$@) $@
## $(EXECUTABLE): $(OBJECTS)
## 	$(CC) $(LDFLAGS) -o $@ $<  $(LIBS)

clean:
	find $(SRCDIR) -name '*.o' -exec $(RM) -v {} ';' 
	$(RM) decode

