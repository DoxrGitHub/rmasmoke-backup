#!/bin/bash
CHROOT_PATH=/mnt/stateful_partition/rmasmoke_root
tpm_manager_client destroy_space --index=0x80000A
crossystem clear_tpm_owner_request=1
crossystem clear_tpm_owner_request=1 #grunt weirdness
initctl stop trunksd
initctl stop tpm_managerd
initctl status tpm2-simulator # Chk the pid of tpm2 
mount --bind /dev $CHROOT_PATH/dev
mount --bind /proc $CHROOT_PATH/proc
mount --bind /var $CHROOT_PATH/var
chroot $CHROOT_PATH rmasmoke "$@"
umount $CHROOT_PATH/dev
umount $CHROOT_PATH/proc
umount $CHROOT_PATH/var
initctl start trunksd
initctl start tpm_managerd
initctl status tpm2-simulator #Check if the tpm2 simulator has crashed