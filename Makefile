WORKING_DIRECTORY="$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))"

IMAGE_NAME=SalernOS

ARCH=x86_64
CROSS_COMPILER_SUITE=$(ARCH)-linux-gnu-

MAKE=make
GIT=git
DOCKER=docker
DOCKER_INPUT=Docker
DOCKER_OUTPUT=salernos-buildenv
DOCKER_PLATFORM=linux/$(ARCH)
DOCKER_PLATFORM_ARGS=--platform $(DOCKER_PLATFORM)
DOCKER_GLOBAL_ARGS=--rm -it -v $(WORKING_DIRECTORY):/root/env

COMPONENT_REPOSITORIES=https://github.com/Alessandro-Salerno/SalernOS-Kernel

# Toolchain for building the 'limine' executable for the host.
override DEFAULT_HOST_CC := cc
$(eval $(call DEFAULT_VAR,HOST_CC,$(DEFAULT_HOST_CC)))
override DEFAULT_HOST_CFLAGS := -g -O2 -pipe
$(eval $(call DEFAULT_VAR,HOST_CFLAGS,$(DEFAULT_HOST_CFLAGS)))
override DEFAULT_HOST_CPPFLAGS :=
$(eval $(call DEFAULT_VAR,HOST_CPPFLAGS,$(DEFAULT_HOST_CPPFLAGS)))
override DEFAULT_HOST_LDFLAGS :=
$(eval $(call DEFAULT_VAR,HOST_LDFLAGS,$(DEFAULT_HOST_LDFLAGS)))
override DEFAULT_HOST_LIBS :=
$(eval $(call DEFAULT_VAR,HOST_LIBS,$(DEFAULT_HOST_LIBS)))

buildall: build-each build iso

setup: limine
	mkdir -p SalernOS
	echo $(COMPONENT_REPOSITORIES) | xargs -n1 | xargs -I{} git clone {}

build-each: setup
	for dir in ./*; do (cd "$$dir" && $(MAKE) && cp -r ./bin ../../iso_root/$$dir); done

run: $(IMAGE_NAME).iso ovmf
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf/OVMF.fd -cdrom $(IMAGE_NAME).iso -boot d

ovmf:
	mkdir -p ovmf
	cd ovmf && curl -Lo OVMF.fd https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd

limine:
	$(GIT) clone https://github.com/limine-bootloader/limine.git --branch=v5.x-branch-binary --depth=1 && \
	$(MAKE) -C limine \
		CC="$(HOST_CC)" \
		CFLAGS="$(HOST_CFLAGS)" \
		CPPFLAGS="$(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		LIBS="$(HOST_LIBS)" && \
	mv ./limine/limine.exe ./limine/limine

$(IMAGE_NAME).iso: iso
iso:
	rm -rf iso_root
	mkdir -p iso_root
	cp -v SalernOS-kernel/bin/kern \
		limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/
	mkdir -p iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso
	./limine/limine bios-install $(IMAGE_NAME).iso
	rm -rf iso_root

mkenv:
	$(DOCKER) build $(DOCKER_INPUT) -t $(DOCKER_OUTPUT)

env:
	$(DOCKER) run $(DOCKER_GLOBAL_ARGS) $(DOCKER_OUTPUT)
 
clean:
	rm $(IMAGE_NAME).iso
	for dir in ./SalernOS/*; do (cd "$$dir" && $(MAKE) clean); done

purge:
	rm $(IMAGE_NAME).iso
	rm -rf SalernOS
