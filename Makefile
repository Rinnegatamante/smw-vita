TARGET		:= SuperMarioWar
TITLE	    := SMWAR0000
SOURCES		:= src
INCLUDES	:= src

VITASDK	:= C:\vitasdk

LIBS	:= -lSDL_net -lSDL_mixer -lSDL -lSDL_image -limgui \
	   -lvitaGL -lpng -lz -ljpeg -lFLAC -lvorbisfile -lvorbis -logg -lmpg123 -lc -lmikmod -lmad \
	   -lm -lout123 -lSceNet_stub -lSceNetCtl_stub  -lSceCtrl_stub -lSceCommonDialog_stub -lSceIofilemgr_stub \
	   -lSceGxm_stub -lSceSysmodule_stub -lSceHid_stub -lSceAudio_stub -lSceDisplay_stub -lSceTouch_stub -lScePower_stub

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
OBJS     := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 


export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))


PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = $(INCLUDE) -g -Wl,-q -O2 -w -D__vita__ -DSDL_JOYSTICK_PSP2

# includes ...
CFLAGS += -I$(SOURCES)
CFLAGS += -I$(VITASDK)/$(PREFIX)/include/SDL


export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))


CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: $(TARGET).velf
	vita-make-fself -s $< eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE) -d ATTRIBUTE2=12 "$(TARGET)" param.sfo
	cp -f param.sfo sce_sys/param.sfo
	
	#------------ Comment this if you don't have 7zip ------------------
	7z a -tzip ./$(TARGET).vpk -r ./sce_sys ./eboot.bin ./assets
	#-------------------------------------------------------------------

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf param.sfo sce_sys/param.sfo $(TARGET).velf $(TARGET).elf $(TARGET).vpk $(TARGET).elf.unstripped.elf eboot.bin $(OBJS)


