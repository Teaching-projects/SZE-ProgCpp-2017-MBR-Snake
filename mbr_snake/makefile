# I cba to re-learn how makefiles work every single time, so just deal with it. It works.


CC = g++
CC_FLAGS = -Os -m32 -march=i386 -mtune=intel -std=c++14 -Wno-narrowing -fno-ident -fomit-frame-pointer -ffreestanding -fmerge-constants -c
LD = ld
LD_FLAGS = -static -Tlinker.ld --nmagic -nostdlib -m elf_i386

EXEC = mbr_snake
SOURCE = $(EXEC).cpp
SOURCE_DIR = src
OBJECT = $(SOURCE:.cpp=.o)
OBJECT_DIR = obj
OUTPUT_DIR = bin
OUTPUT = $(SOURCE:.cpp=.bin)


# Main target
$(EXEC): $(OBJECT)
	ld -static -Tlinker.ld --nmagic -nostdlib -m elf_i386 -o $(OUTPUT_DIR)/$(EXEC).elf $(OBJECT_DIR)/$(OBJECT)
	objcopy -O binary $(OUTPUT_DIR)/$(EXEC).elf $(OUTPUT_DIR)/$(OUTPUT)
	rm $(OUTPUT_DIR)/$(EXEC).elf

# No boot sector signature
nosig: $(OBJECT)
	ld -static -Tlinker_nosig.ld --nmagic -nostdlib -m elf_i386 -o $(OUTPUT_DIR)/$(EXEC).elf $(OBJECT_DIR)/$(OBJECT)
	objcopy -O binary $(OUTPUT_DIR)/$(EXEC).elf $(OUTPUT_DIR)/$(OUTPUT)
	rm $(OUTPUT_DIR)/$(EXEC).elf

$(OBJECT):
	$(CC) $(CC_FLAGS) -o $(OBJECT_DIR)/$(OBJECT) $(SOURCE_DIR)/$(SOURCE)
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag $(OBJECT_DIR)/$(OBJECT)

clean:
	rm -f bin/* obj/*
