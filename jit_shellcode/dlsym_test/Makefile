CC = gcc
OBJCOPY = objcopy
CFLAGS = -O3 -no-pie -std=gnu11 -Wno-int-conversion -masm=intel -nostartfiles -fcf-protection=none -fno-stack-protector -ffreestanding -fno-builtin -Tscript.ld
NAME = dlsym

.PHONY: all
all: $(NAME).bin

$(NAME).bin: $(NAME).c
	$(CC) $(CFLAGS) -c $(NAME).c -o $(NAME).o
	$(CC) $(NAME).o -o $(NAME).elf $(CFLAGS)
	$(OBJCOPY) -O binary --only-section=.text $(NAME).elf $@
	-rm -f $(NAME).elf $(NAME).o

.PHONY: clean
clean:
	-rm -f $(NAME).o $(NAME).bin $(NAME).elf