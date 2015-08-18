CC  = avr-gcc
CFLAGS   = -mmcu=$(MCU) -std=c99 -pipe -Os -Wall
CFLAGS  += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

NM      = avr-nm
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size

# This is the part of the makefile that you need to change
# The names should be clear enough, but just in case
#
# PROGNAME = name of the program you are making.
# MCU = name of the chip you are using.
# PROGRAMMER_MCU = the name of the chip by the avrdude, but you can use the normal name.
# LOW_FUSE = the low fuse for the project (be carefull with the fuses, it can brick your chip).
# HIGH_FUSE = the high fuse for the project (be carefull with the fuses, it can brick your chip).
# AVRDUDE_PROGRAMMERID = the programer you are using, in the example is the usbtiny by ladyada.
# AVRDUDE_PORT = the port your programmer use, usb, lpt1 (parallel) or com1(serial) problably.
PROGNAME = main
MCU = attiny2313
PROGRAMMER_MCU = t2313
LOW_FUSE = 0xFF
HIGH_FUSE = 0xDF
AVRDUDE_PROGRAMMERID = usbtiny
AVRDUDE_PORT = usb

# Make targets:
# all, hex, writeflash/go, writefuse
all: $(PROGNAME).hex $(PROGNAME).eep

hex: $(PROGNAME).hex

writeflash: hex
	avrdude -c $(AVRDUDE_PROGRAMMERID) \
	-p $(PROGRAMMER_MCU) \
	-P $(AVRDUDE_PORT) \
	-U flash:w:$(PROGNAME).hex

go: writeflash

writefuse:
	avrdude -c $(AVRDUDE_PROGRAMMERID) \
	-p $(PROGRAMMER_MCU) \
	-P $(AVRDUDE_PORT) \
	-U lfuse:w:$(LOW_FUSE):m \
	-U hfuse:w:$(HIGH_FUSE):m \



#The variables:
# $@ - The name of the target of the rule.
# $? - The names of all the prerequisites that are newer than the target.
# $< - The name of the first prerequisite.
# $^ - The names of all the prerequisites.

#Pattern-rules
%.s: %.c
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $(ASFLAGS) -o $@ $<
%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(ASFLAGS) -o $@ $<
%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(ASFLAGS) -o $@ $<
%.o: %.h
%.elf: %.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

# Create final output files (.hex, .eep) from ELF output file.
# Use -O ihex for Intel Hex format
# Use -O binary for binary format
%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	#$(OBJCOPY) -j .text -j .data -O ihex $< $@
%.eep: %.elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O ihex $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	$(OBJDUMP) -h -S -C $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	$(NM) -n $< > $@


