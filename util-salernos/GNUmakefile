MAKEFLAGS += -rR
.SUFFIXES:

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CC ?= cc
CFLAGS := -Wall -Wextra -O2 -pipe

override CFILES = $(call rwildcard,src,*.c)

# Generate matching bin paths
# src/foo/bar.c -> bin/foo/bar
override BINARIES = $(patsubst src/%.c,bin/%,$(CFILES))

.PHONY: all
all: $(BINARIES)

# Build one binary per C file
bin/%: src/%.c GNUmakefile
	@echo [CC] $<
	@mkdir -p "$$(dirname $@)"
	@$(CC) $< $(CFLAGS) -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf bin
