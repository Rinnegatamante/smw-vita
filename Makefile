TARGET		:= SuperMarioWar
TITLE	    := SMWAR0000
SOURCES		:= src
INCLUDES	:= src

PKG_CONFIG = arm-vita-eabi-pkg-config
SDL_CFLAGS := $(shell $(PKG_CONFIG) --cflags SDL_net SDL_mixer SDL_image)
SDL_LIBS := $(shell $(PKG_CONFIG) --static --libs SDL_net SDL_mixer SDL_image)

LIBS	:= $(SDL_LIBS) -lSDL_net -lSDL_mixer -lSDL -lSDL_image -limgui -lvitaGL -lmathneon -lSceAppMgr_stub \
	   -lpng16 -lz -ljpeg -lFLAC -lvorbisfile -lvorbis -logg -lmpg123 -lc -lmikmod -lmad \
	   -lm -lout123 -lSceNet_stub -lSceNetCtl_stub  -lSceCtrl_stub -lSceCommonDialog_stub -lSceIofilemgr_stub \
	   -lSceGxm_stub -lSceSysmodule_stub -lSceHid_stub -lSceAudio_stub -lSceDisplay_stub -lSceTouch_stub -lScePower_stub \
	   -lvitashark -lSceShaccCgExt -ltaihen_stub -lSceShaccCg_stub \
	   -lSceKernelDmacMgr_stub -lSceAppUtil_stub

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
OBJS     := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 


export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))


PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = $(INCLUDE) -g -Wl,-q -O2 -mtune=cortex-a9 -mfpu=neon -ffast-math -ftree-vectorize -w -D__vita__ -DSDL_JOYSTICK_PSP2

# includes ...
CFLAGS += -I$(SOURCES)
CFLAGS += $(SDL_CFLAGS)


export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))


CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: $(TARGET).velf
	vita-make-fself -c -s $< eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE) -d ATTRIBUTE2=12 "$(TARGET)" param.sfo
	cp -f param.sfo sce_sys/param.sfo
	
	vita-pack-vpk -s param.sfo -b eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea=sce_sys/livearea --add shaders=shaders \
		$(if $(wildcard assets),--add assets=assets) $@

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf param.sfo sce_sys/param.sfo $(TARGET).velf $(TARGET).elf $(TARGET).vpk $(TARGET).elf.unstripped.elf eboot.bin $(OBJS)


