MAKEFLAGS += -rR
.SUFFIXES:

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard ,$d, $2) $(filter $(subst *, %, $2),$d))

override OUTPUT := init

CC ?= cc
LD ?= cc
CFLAGS := -O2 -pipe
CPPFLAGS :=
LDFLAGS :=

# Internal C flags that should not be changed by the user.
override CFLAGS += \
	-I include/ \
    -Wall \
    -Wextra \
    -std=gnu11 \
	-lutil

override CPPFLAGS := \
    $(CPPFLAGS) \
    -MMD \
    -MP

override CFILES = $(call rwildcard, src, *c) $(call rwildcard, src/platform/$(PLATFORM), *.c)
override OBJ = $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS = $(addprefix obj/,$(CFILES:.c=.c.d))

.PHONY: all
all: bin/$(OUTPUT)

bin/$(OUTPUT): GNUmakefile $(OBJ)
	@echo [LD] Linking...
	@mkdir -p "$$(dirname $@)"
	@$(LD) $(OBJ) $(LDFLAGS) -o $@

# Include header dependencies.
-include $(HEADER_DEPS)

# Compilation rules for *.c files.
obj/src/%.c.o: src/%.c GNUmakefile
	@echo [CC] $<
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf bin obj
