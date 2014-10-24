## makefile for decode.C
ANALYSIS	:= ../decode
OPTIONS		:= -d-32
INPUT		:= /groupshare_ipp/testbeam_diamond_oct2014/padruns/
INPUT_FINAL := input/
OUTPUT		:= /scratch/PAD-testbeams/CERN_oct_14/pad_out/
PICS_OUT	:= /scratch/analysis/output/PAD/
FNAME       := run_*.dat
FDIR        := $(INPUT)/$(FNAME)
SOURCES     := $(wildcard $(INPUT)/run_*.dat)
SOURCES_FINAL:= $(patsubst $(INPUT)/%.dat,$(INPUT_FINAL)/%.dat,$(SOURCES))
OBJECTS     := $(patsubst $(INPUT)/%.dat,$(OUTPUT)/%.root,$(SOURCES))

RM  	    := rm
MV      	:= mv





# Rules ====================================
#all: $(SOURCES) $(EXECUTABLE)
#all: printer analysis
all: analysis

printer:
	@echo 'OBJECTS $(OBJECTS)'
	@echo 'INPUT $(INPUT)'
	@echo 'OUTPUT $(OUTPUT)'
	@echo 'SOURCES $(SOURCES)'
	@echo 'FNAME $(FNAME)'
	@echo 'FDIR $(FDIR)'
	@echo 'INPUT_FINAL $(INPUT_FINAL)'
	@echo 'SOURCES_FINAL $(SOURCES_FINAL)'

analysis: $(OBJECTS) $(ANALYSIS)

$(OBJECTS): |$(OUTPUT)

$(OUTPUT):
	mkdir -p $(OUTPUT)

$(INPUT_FINAL)/%.dat: 
	ln -s $(patsubst $(INPUT_FINAL)/%.dat,$(INPUT)/%.dat,$@) $@

$(OUTPUT)/%.root:  $(INPUT)/%.dat $(INPUT_FINAL)/%.dat $(ANALYSIS) 
	@echo '$<'
	@echo '$@'
	@echo '%'
	$(ANALYSIS)  $(OPTIONS) -i $(patsubst $(OUTPUT)/%.root,$(INPUT_FINAL)/%.dat,$@)
	$(MV) $(patsubst $(OUTPUT)/%.root, $(INPUT_FINAL)/%.root,$@) $@
	$(MV) $(INPUT_FINAL)/*.png $(PICS_OUT)
## $(EXECUTABLE): $(OBJECTS)
## 	$(CC) $(LDFLAGS) -o $@ $<  $(LIBS)

clean:
	find $(SRCDIR) -name '*.o' -exec $(RM) -v {} ';' 
	$(RM) decode

