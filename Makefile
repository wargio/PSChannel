#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path>")
endif

include $(PSL1GHT)/ppu_rules

PSCHANNEL_VERSION = 	1.11

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source
DATA		:=	data
INCLUDES	:=	include

ICON0		:=	$(CURDIR)/ICON0.PNG
SFOXML		:=	$(CURDIR)/sfo.xml
PKGFILES	:=	$(CURDIR)/release

TITLE		:=	PSChannel
APPID		:=	PSCHANNEL
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS		=	-mcpu=cell $(MACHDEP) $(INCLUDE) \
			-I$(PS3DEV)/portlibs/ppu/include/edje-1 \
			-I$(PS3DEV)/portlibs/ppu/include/ecore-1 \
			-I$(PS3DEV)/portlibs/ppu/include \
			-I$(PS3DEV)/portlibs/ppu/include/embryo-1 \
			-I$(PS3DEV)/portlibs/ppu/include/evas-1 \
			-I$(PS3DEV)/portlibs/ppu/include/eet-1 \
			-I$(PS3DEV)/portlibs/ppu/include/eina-1 \
			-I$(PS3DEV)/portlibs/ppu/include/eina-1/eina \
			-I$(PS3DEV)/portlibs/ppu/include/escape-0 \
			-I$(PS3DEV)/portlibs/ppu/include/libpng14 \
			-I$(PS3DEV)/portlibs/ppu/include/SDL \
			-I$(PS3DEV)/portlibs/ppu/include/freetype2 \
			-DPSCHANNEL_VERSION=$(PSCHANNEL_VERSION)
CXXFLAGS	=	$(CFLAGS)

LDFLAGS		=	$(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lz -lzip -ledje -lecore -lecore_psl1ght -lecore_evas \
		-lecore_input_evas -lecore_input -lecore_file -lecore_con -lcurl \
		-lsysutil -levas -leina -lescape -lnet -lsysmodule -liberty -leet \
		-lembryo -llua -lm -lfontconfig -lcares -lexpat -lSDL \
		-lecore_psl1ght -lecore_sdl -lecore_imf -lecore_imf_evas\
		-lspurs -lgem -lrsx -lgcm_sys -lrt -lcamera -laudio -lio -llv2 \
		-lsysutil -lsysmodule -lpng -lpngdec -ljpeg -ljpgdec -lz -lzip  \
		-lfreetype -lsysfs -lpolarssl

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export BUILDDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(LIBPSL1GHT_INC) \
					-I$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					$(LIBPSL1GHT_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD): edj
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(PKGFILES)/USRDIR/theme/*.edj *.self *.pkg
	@make --no-print-directory -C edc/ clean

#---------------------------------------------------------------------------------
run:
	ps3load $(OUTPUT).self

#---------------------------------------------------------------------------------
edj:
	@echo "Building edj."
	@make --no-print-directory -C edc/
#---------------------------------------------------------------------------------
pkg:	$(BUILD) $(OUTPUT).pkg

#---------------------------------------------------------------------------------
debug: pkg
	@clear
	@echo "Ready.."
	@nc -l -u -p 18194

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf:	$(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
