BINARY = LeuchtSchild

BROOT = build

SRC_FILES = $(shell ls src/*.c)
HDR_FILES = $(shell ls src/*.h)

DATE=$(shell date +%F)
HOUR=$(shell date +%H)
MIN=$(shell date +%M)
COMPILE_DATE="$(DATE) $(HOUR):$(MIN) CET"
CC=avr-gcc
OBJCPY=avr-objcopy
AVRDUDE=sudo avrdude
AVRDUDEARCH=m64
AVRDUDEPROG=usbtiny

BR=$(BROOT)/bin



OBJTMP=$(shell ls src/*.c | sed s/src.//)
OBJR=$(OBJTMP:%.c=$(BR)/obj/%.o)

RELEASE_DEFINES =  -DCOMPILE_DATE=\"$(COMPILE_DATE)\"
RELEASE_DEFINES += -DF_CPU=16000000UL
RELEASE_DEFINES += -DCOLOR
BASE_CFLAGS= -mmcu=atmega64m1 -Os -pedantic -fPIC -std=c99
CFLAGS=$(BASE_CFLAGS)



.PHONY: all objcopy clean
release: $(BR)/$(BINARY).hex
all: release



#####
# Link binary
$(BR)/$(BINARY).elf: $(BR) $(OBJR)
	@printf "LINKING \033\13301;33m-\033\13301;37m %-20s\n\033\13300;39m" $@
	@$(CC) $(CFLAGS) $(OBJR) $(LDFLAGS) -o $(BR)/$(BINARY).elf



#####
# Compile actual files
$(BR)/obj/%.o: src/%.c $(HDR_FILES)
	@printf "BUILD   \033\13301;33m-\033\13301;37m %-20s %s\033\13300;39m\n" $< $@
	@$(CC) -c $(CFLAGS) $(RELEASE_DEFINES) -o $@  $<



#####
# ObjCopy
$(BR)/$(BINARY).hex: $(BR)/$(BINARY).elf
	@printf "OBJCPY  \033\13301;33m-\033\13301;37m %-20s %s\033\13300;39m\n" $@
	@$(OBJCPY) -O ihex $(BR)/$(BINARY).elf $(BR)/$(BINARY).hex



#####
# Cleanup
clean:
	@rm -rf $(BROOT)



#####
# Flash
flash:
	@$(AVRDUDE) -p $(AVRDUDEARCH) -c $(AVRDUDEPROG) -F -P usb -U flash:w:"$(BR)/$(BINARY).hex":i



#####
# Setup directories
$(BR):
	@if [ ! -d $(BROOT) ];then mkdir -p $(BROOT);fi
	@if [ ! -d $(BROOT)/gcc_release ];then mkdir -p $(BR); mkdir -p $(BR)/obj;fi
	@if [ ! -d $(@) ];then mkdir $@;fi
