#!/bin/sh
arg1=$1
echo "--------------start ${arg1}-----------------"
if [ "$arg1" = "bios" ]; then
# ---------------------------------------------------- bios startup  perfect  -------------------------------------------------------
#qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -m 4G -smp 8,sockets=1,cores=8 --accel tcg,thread=multi  -bios /home/liroding/workspace/app/OVMF_enoch.fd -machine q35 -device edu -drive file=fat:rw:/home/liroding/workspace/app -serial file:/home/liroding/workspace/app/app.log -d guest_errors -debugcon file:debug.log -global isa-debugcon.iobase=0x402 -s
#qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -m 4G -smp 8,sockets=1,cores=8 -bios /home/liroding/workspace/app/OVMF_enoch.fd -machine q35 -cpu Broadwell -device edu -drive file=fat:rw:/home/liroding/workspace/app -serial file:/home/liroding/workspace/app/app.log -d guest_errors -debugcon file:debug.log -global isa-debugcon.iobase=0x402 -s
#    gdb --args qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -m 4G -smp 1,sockets=1,cores=1 --accel tcg,thread=multi  -bios /home/liroding/workspace/app/OVMF/OVMF_enoch.fd -machine q35 -device edu -hda fat:rw:/home/liroding/workspace/app -serial file:/home/liroding/workspace/app/app.log  -debugcon file:debug.log -monitor stdio

#---------------------------------- debug vga add --------------------------
   gdb --args /home/liroding/workspace/git/RemoteGit/qemu_4.0.0/qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64  -m 4G -smp 1,sockets=1,cores=1 --accel tcg,thread=multi  -bios image/OVMF/OVMF_9.fd -machine q35 -hda fat:rw:/home/liroding/workspace/app -serial file:/home/liroding/workspace/app/app.log  -debugcon file:debug.log -monitor stdio -device cirrus-vga  

fi






# ---------------------------------------------------- ubuntu  -------------------------------------------------------

if [ "$arg1" = "ubuntu" ]; then
#----------------------------------------------------- ubuntu install ------------------------------------------------------------------------------------
#    ../work/qemu/build/x86_64-softmmu/qemu-system-x86_64 -enable-kvm -hda block.raw -cdrom ubuntu-14.04.5-server-amd64.iso -m 2048

#    ../qemu-2.10.0/build/x86_64-softmmu/qemu-system-x86_64 -enable-kvm -hda ../project/dpdk_item/uefi_ubuntu_os.img -boot d -cdrom ../zip/ubuntu-14.04.5-server-amd64.iso -m 2048

#    ../qemu-2.10.0/build/x86_64-softmmu/qemu-system-x86_64 -bios ../app/OVMF_1.fd -drive if=none,file=../zip/ubuntu-14.04.5-server-amd64.iso,id=cdrom,media=cdrom -drive if=none,file=../project/dpdk_item/uefi_ubuntu_os.img,id=hd0 -m 2048 -enable-kvm
#-----------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------- ubuntu startup  perfect [dpdk QEMU_1] -------------------------------------------------------

#    qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -enable-kvm -smp 4,sockets=1,cores=4 -net tap,ifname=tap0,script=no,downscript=on -net nic,model=e1000,macaddr=52:54:00:12:34:56 -cpu qemu64,+ssse3,+sse4.1,+sse4.2 -hda /home/liroding/workspace/project/dpdk_item/ubuntu14.04.raw -m 2048 -drive file=fat:rw:/home/liroding/workspace/project/dpdk_item/node -machine q35 
    qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64  -smp 4,sockets=1,cores=4  -cpu qemu64,+ssse3,+sse4.1,+sse4.2 -hda /home/liroding/workspace/project/dpdk_item/ubuntu14.04.raw -m 2048 -drive file=fat:rw:/home/liroding/workspace/project/dpdk_item/node -machine q35  -device cirrus-vga   
fi


if [ "$arg1" = "ubuntu_1" ]; then

# ---------------------------------------------------- ubutun stattup  perfect [dpdk QEMU_2] -------------------------------------------------------
  qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64  -smp 4,sockets=1,cores=4 -net tap,ifname=tap1,script=no,downscript=on -net nic,model=e1000,macaddr=52:54:00:12:34:60 -cpu qemu64,+ssse3,+sse4.1,+sse4.2 -hda /home/liroding/workspace/project/dpdk_item/ubuntu14.04_brother.raw -m 2048 -drive file=fat:rw:/home/liroding/workspace/project/dpdk_item/node -machine q35 
fi





#-------------------------------------------------------------- win7/10 -----------------------------------------------------

if [ "$arg1" = "winos" ]; then

# ------------------------------------------------------ win install ----------------------------------------------------------------------------------
 #   ../qemu-2.10.0/build/x86_64-softmmu/qemu-system-x86_64 -enable-kvm -hda ./dpdk_item/winos.img -cdrom /home/liroding/workspace/zip/en_windows_7.iso -m 2048
#    ../qemu-2.10.0/build/x86_64-softmmu/qemu-system-x86_64 -bios ../app/OVMF.fd -enable-kvm -hda ./dpdk_item/uefi_winos.raw -cdrom /home/liroding/workspace/zip/en_windows_7.iso -m 2048
#     ../qemu-2.10.0/build/x86_64-softmmu/qemu-system-x86_64 -bios ../app/OVMF_1.fd -enable-kvm -hda ./dpdk_item/uefi_win10_20G.raw -cdrom /home/liroding/workspace/zip/en_windows_10.ISO -m 2048
# ---------------------------------------------------------------------------------------------------------------------------------------------------------


# ------------------------------------------------------ win7 startup perfect [win7]  --------------------------------------------------------------------------- 
   qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -bios image/OVMF/OVMF_enoch.fd -enable-kvm -hda /home/liroding/workspace/project/dpdk_item/uefi_winos.raw  -m 2048

# ------------------------------------------------------ win10 startup perfect [win10]  --------------------------------------------------------------------------- 
#   qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64  -bios image/OVMF/OVMF_enoch.fd  -enable-kvm -hda /home/liroding/workspace/project/dpdk_item/uefi_win10_20G.raw -m 2048

fi









# ---------------------------------------------------- small simple linux os  -------------------------------------------------------

if [ "$arg1" = "smalllinuxos" ]; then

# ------------------------------------------------------ small linuxos startup  --------------------------------------------------------------------------- 
#  qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64  -bios ../app/OVMF/OVMF.fd -m 2G -smp 4,sockets=1,cores=4 -append "root=/dev/sda init=/linuxrc " -drive format=raw,file=./dpdk_item/linuxos.raw -kernel ./dpdk_item/bzImage -machine q35 -drive file=fat:rw:../app/
#   ../qemu-2.10.0/build/x86_64-softmmu/qemu-system-x86_64 -m 2G -smp 4,sockets=1,cores=4 -enable-kvm -append "root=/dev/sda init=/linuxrc noapic nolock" -drive format=raw,file=./dpdk_item/linuxos.raw -kernel ./dpdk_item/bzImage -device dramc -machine q35 -drive file=fat:rw:../app/

# qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -m 2G -smp 4,sockets=1,cores=4 --accel tcg,thread=multi -append "root=/dev/sda init=/linuxrc noapic nolock" -drive format=raw,file=./disk_30M.raw -kernel ./bzImage -debugcon file:debug.log -global isa-debugcon.iobase=0x402 -device dramc -machine q35 -drive file=fat:rw:bin/os 
# ----------------------------------------------------------------------------------------------------------------------------------------------------------
#-------------------------  add usb xhci and usb storage device     
#gdb --args qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -m 4G -machine q35 -smp 1,sockets=1,cores=1 -bios /home/liroding/workspace/app/OVMF/OVMF_enoch.fd  -kernel /home/liroding/workspace/app/kernel/usb_item/cosim_bzImage_2021.4.19 -append "root=/dev/sda init=/linuxrc " -drive format=raw,file=/home/liroding/workspace/app/kernel/usb_item/linuxos.raw  -drive file=fat:rw:/home/liroding/workspace/app -device qemu-xhci,id=zx_usb1 -device usb-storage,id=usbdisk1,drive=disk1,port=1 -drive if=none,format=raw,id=disk1,file=/home/liroding/workspace/app/kernel/usb_item/disk_usb.img  -monitor stdio 
#gdb --args qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -m 4G -machine q35 -smp 1,sockets=1,cores=1 -bios image/OVMF/OVMF_enoch.fd  -kernel image/kernel/usb_item/cosim_bzImage_2021.4.19 -debugcon file:kernel_debug.log -global isa-debugcon.iobase=0x402 -append "root=/dev/sda init=/linuxrc "  -drive format=raw,file=image/kernel/usb_item/linuxos.raw  -drive file=fat:rw:image/app -device qemu-xhci,id=zx_usb1 -device usb-storage,id=usbdisk1,drive=disk1,port=1 -drive if=none,format=raw,id=disk1,file=image/kernel/usb_item/disk_usb.img  -monitor stdio #-serial pty -nographic  

#----------------------------------------------vga add -------    
gdb --args qemu-4.0.0/build/x86_64-softmmu/qemu-system-x86_64 -m 4G -machine q35 -smp 1,sockets=1,cores=1 -bios image/OVMF/OVMF_enoch.fd  -kernel image/kernel/usb_item/cosim_bzImage_2021.4.19  -append "root=/dev/sda init=/linuxrc " -drive format=raw,file=/home/liroding/workspace/app/kernel/usb_item/linuxos.raw  -drive file=fat:rw:image/app/ -monitor stdio 
# -device cirrus-vga 
    
fi
