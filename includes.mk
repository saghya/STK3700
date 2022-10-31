SDK = /opt/simplicitystudio/developer/sdks/gecko_sdk_suite/v2.7

INCLUDEFLAGS = -I $(SDK)/hardware/kit/common/bsp
INCLUDEFLAGS += -I $(SDK)/hardware/kit/common/drivers
INCLUDEFLAGS += -I $(SDK)/hardware/kit/EFM32GG_STK3700/config
INCLUDEFLAGS += -I $(SDK)/platform/CMSIS/Include
INCLUDEFLAGS += -I $(SDK)/platform/Device/SiliconLabs/EFM32GG/Include
INCLUDEFLAGS += -I $(SDK)/platform/emlib/inc
INCLUDEFLAGS += -I ./Drivers

