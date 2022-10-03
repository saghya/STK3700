include includes.mk

DEVICE = EFM32GG990F1024

SRC_DIR = ./src
EMLIB_DIR = ./emlib
CMSIS_DIR = ./CMSIS/EFM32GG
BSP_DIR = ./BSP
DRIVERS_DIR = ./Drivers
SERVICE_DIR = ./service

BIN = ./bin
SRC_OBJ_DIR = $(BIN)/src
EMLIB_OBJ_DIR = $(BIN)/emlib
CMSIS_OBJ_DIR = $(BIN)/CMSIS/EFM32GG
BSP_OBJ_DIR = $(BIN)/BSP
DRIVERS_OBJ_DIR = $(BIN)/Drivers
SERVICE_OBJ_DIR = $(BIN)/service

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
EMLIB_FILES = $(wildcard $(EMLIB_DIR)/*.c)
CMSIS_FILES = $(wildcard $(CMSIS_DIR)/*.c)
BSP_FILES = $(wildcard $(BSP_DIR)/*.c)
DRIVERS_FILES = $(wildcard $(DRIVERS_DIR)/*.c)
SERVICE_FILES = $(wildcard $(SERVICE_DIR)/*.c)

SRC_OBJ = $(patsubst $(SRC_DIR)/%.c,$(SRC_OBJ_DIR)/%.o,$(SRC_FILES))
EMLIB_OBJ = $(patsubst $(EMLIB_DIR)/%.c,$(EMLIB_OBJ_DIR)/%.o,$(EMLIB_FILES))
CMSIS_OBJ = $(patsubst $(CMSIS_DIR)/%.c,$(CMSIS_OBJ_DIR)/%.o,$(CMSIS_FILES))
BSP_OBJ = $(patsubst $(BSP_DIR)/%.c,$(BSP_OBJ_DIR)/%.o,$(BSP_FILES))
DRIVERS_OBJ = $(patsubst $(DRIVERS_DIR)/%.c,$(DRIVERS_OBJ_DIR)/%.o,$(DRIVERS_FILES))
SERVICE_OBJ = $(patsubst $(SERVICE_DIR)/%.c,$(SERVICE_OBJ_DIR)/%.o,$(SERVICE_FILES))

SRC = STK3700
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
GDB = arm-none-eabi-gdb
OBJ += ./bin/src/*.o
#OBJ += ./bin/BSP/*.o
OBJ += ./bin/CMSIS/EFM32GG/*.o
OBJ += ./bin/emlib/*.o
OBJ += ./bin/Drivers/*.o
#OBJ += ./bin/service/*.o
CFLAGS = -g3 -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 -D $(DEVICE)=1
DEBUGFLAGS = -O0 -Wall -fmessage-length=0 -c -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -D DEBUG_EFM=1

all: dirs emlib cmsis bsp drivers service $(SRC) axf hex bin s37 size

dirs:
	mkdir -p $(BIN)/src
	mkdir -p $(BIN)/emlib 
	mkdir -p $(BIN)/CMSIS/EFM32GG
	mkdir -p $(BIN)/BSP
	mkdir -p $(BIN)/Drivers
	mkdir -p $(BIN)/service

$(SRC): $(SRC_OBJ)
$(SRC_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -D $(DEVICE)=1 $(INCLUDEFLAGS) $(DEBUGFLAGS) -o $@ $^

emlib: $(EMLIB_OBJ)
$(EMLIB_OBJ_DIR)/%.o: $(EMLIB_DIR)/%.c
	$(CC) $(CFLAGS) -D $(DEVICE)=1 $(INCLUDEFLAGS) $(DEBUGFLAGS) -o $@ $^

cmsis:
	$(CC) $(CFLAGS) -c -x assembler-with-cpp -D $(DEVICE)=1 $(INCLUDEFLAGS) -D $(DEVICE)=1 -o $(BIN)/CMSIS/EFM32GG/startup_gcc_efm32gg.o CMSIS/EFM32GG/startup_gcc_efm32gg.s
	$(CC) $(CFLAGS) -D $(DEVICE)=1 $(INCLUDEFLAGS) $(DEBUGFLAGS) -o $(BIN)/CMSIS/EFM32GG/system_efm32gg.o CMSIS/EFM32GG/system_efm32gg.c

bsp: $(BSP_OBJ)
$(BSP_OBJ_DIR)/%.o: $(BSP_DIR)/%.c
	$(CC) $(CFLAGS) -D $(DEVICE)=1 $(INCLUDEFLAGS) $(DEBUGFLAGS) -o $@ $^

drivers: $(DRIVERS_OBJ)
$(DRIVERS_OBJ_DIR)/%.o: $(DRIVERS_DIR)/%.c
	$(CC) $(CFLAGS) -D $(DEVICE)=1 $(INCLUDEFLAGS) $(DEBUGFLAGS) -o $@ $^

service: $(SERVICE_OBJ)
$(SERVICE_OBJ_DIR)/%.o: $(SERVICE_DIR)/%.c
	$(CC) $(CFLAGS) -D $(DEVICE)=1 $(INCLUDEFLAGS) $(DEBUGFLAGS) -o $@ $^

axf:
	$(CC) $(CFLAGS) -T $(SRC).ld -Xlinker --gc-sections -Xlinker -Map=$(BIN)/$(SRC).map --specs=nano.specs -o $(BIN)/$(SRC).axf $(OBJ) -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group

hex:
	$(OBJCOPY) -O ihex $(BIN)/$(SRC).axf $(BIN)/$(SRC).hex

bin:
	$(OBJCOPY) -O binary $(BIN)/$(SRC).axf $(BIN)/$(SRC).bin

s37:
	$(OBJCOPY) -O srec $(BIN)/$(SRC).axf $(BIN)/$(SRC).s37

size:
	$(SIZE) $(BIN)/$(SRC).axf -A

flash:
	JLinkExe -Device $(DEVICE) -If SWD -Speed 4000 -CommanderScript Flash.jlink

gdb:
	JLinkGDBServer -if SWD -device $(DEVICE) -speed 4000 &
	alacritty -e $(GDB) -x debug.gdb $(BIN)/$(SRC).axf

clean:
	rm -rfv *.axf *.hex *.bin *.s37 *.map bin 

.PHONY: all emlib cmsis bsp drivers service axf hex bin s37 size flash gdb clean

