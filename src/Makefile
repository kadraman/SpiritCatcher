PROJECT_NAME = SpiritDX

# Set platforms to build here, spaced separated. (These are in the separate Makefile.targets)
# They can also be built/cleaned individually: "make gg" and "make gg-clean"
# Possible are: gb gbc pocket megaduck sms gg
TARGETS=gbc

CUSTOM_DATA_SIZE = 8

# Builds all targets sequentially
all: $(TARGETS)

# Number of banks (must be a power of 2): A (Automatic), 2, 4, 8, 16, 32...
N_BANKS = A

# Music player: HUGETRACKER(default) or GBT_PLAYER
MUSIC_PLAYER = HUGETRACKER

# Default hardware sprites size: SPRITES_8x16(default) or SPRITES_8x8
DEFAULT_SPRITES_SIZE = SPRITES_8x8

# Debug builds skip splash/title and start on the overworld.
# Override per-invocation, e.g. make BUILD_TYPE=Debug START_STATE=StateGame gbc
ifndef BUILD_TYPE
	BUILD_TYPE = Release
endif
ifndef START_STATE
	ifeq ($(findstring Debug,$(BUILD_TYPE)),Debug)
		START_STATE = StateOverworld
	endif
endif
ifneq ($(START_STATE),)
	BUILD_DEFS += -DSTART_STATE=$(START_STATE)
endif

#include $(ZGB_PATH)/src/MakefileCommon
include $(subst ',,$(subst \,/,'$(ZGB_PATH)'))/src/MakefileCommon
