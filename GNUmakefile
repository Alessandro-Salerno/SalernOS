# Nuke built-in rules and variables.
MAKEFLAGS += -rR
.SUFFIXES:

#RECIPES := $(shell find recipes/ -type f -printf "%f\n" | paste -sd ' ' -)
#HOST_RECIPES := $(shell find host-recipes/ -type f -printf "%f\n" | paste -sd ' ' -)

.PHONY: all
all: iso_root salernos.iso

host:
	./jinx host-build '*'

target: 
	./jinx build '*'

install_sysroot:
	cp -r sysroot/* iso_root/

iso_root: kernel target
	./jinx install iso_root/ '*'

kernel:
	git clone https://github.com/Alessandro-Salerno/SalernOS-Kernel kernel

iso_root/initrd:
	$(MAKE) install_sysroot && \
		cd iso_root/ && \
		tar -cf ../initrd --dereference --hard-dereference . && \
		mv ../initrd ./initrd

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1 && \
	make -C limine > /dev/null 2> /dev/null

iso_root/boot/limine: limine
	mkdir -p iso_root/boot/limine && \
		cp limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin \
   		limine/limine-uefi-cd.bin iso_root/boot/limine/

iso_root/EFI/BOOT: limine
	mkdir -p iso_root/EFI/BOOT && \
		cp limine/BOOTX64.EFI iso_root/EFI/BOOT/ && \
		cp limine/BOOTIA32.EFI iso_root/EFI/BOOT/

salernos.iso: iso_root/boot/limine iso_root/EFI/BOOT iso_root/initrd
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o salernos.iso && \
	./limine/limine bios-install salernos.iso > /dev/null 2> /dev/null

rebuild:
	./jinx regen kernel && \
		./jinx rebuild kernel && \
		./jinx regen init && \
		./jinx rebuild init && \
		rm salernos.iso && \
		rm -rf iso_root/ && \
		./jinx install iso_root/ '*' && \
		$(MAKE) salernos.iso

clean:
	rm -rf *.iso iso_root builds

purge:
	rm -rf builds host-builds sources host-pkgs kernel pkgs iso_root limine *.iso

run: salernos.iso
	qemu-system-x86_64 -M q35 -m 12000m -enable-kvm -smp cpus=8 -no-shutdown -no-reboot -debugcon file:/dev/stdout -serial file:/dev/stdout -netdev user,id=net0 -device virtio-net,netdev=net0 -object filter-dump,id=f1,netdev=net0,file=netdump.dat -cdrom salernos.iso

debug: salernos.iso
	qemu-system-x86_64 -M q35 -m 12000m -enable-kvm -smp cpus=8 -no-shutdown -no-reboot -debugcon file:/dev/stdout -serial file:/dev/stdout -netdev user,id=net0 -device virtio-net,netdev=net0 -object filter-dump,id=f1,netdev=net0,file=netdump.dat -cdrom salernos.iso -S -s

