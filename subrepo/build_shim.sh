SHIM_PATH=$1
MOUNT_PATH=${PWD}/mnt
enable_rw_mount() {
  local rootfs="$1"
  local offset="${2-0}"
  # Make sure we're checking an ext2 image
  # shellcheck disable=SC2086
  if ! is_ext2 "$rootfs" $offset; then
    echo "enable_rw_mount called on non-ext2 filesystem: $rootfs $offset" 1>&2
    return 1
  fi
  local ro_compat_offset=$((0x464 + 3))  # Set 'highest' byte
  # Dash can't do echo -ne, but it can do printf "\NNN"
  # We could use /dev/zero here, but this matches what would be
  # needed for disable_rw_mount (printf '\377').
  printf '\000' |
    sudo dd of="$rootfs" seek=$((offset + ro_compat_offset)) \
            conv=notrunc count=1 bs=1 2>/dev/null
}
# For details, see crosutils.git/common.sh
is_ext2() {
  local rootfs="$1"
  local offset="${2-0}"
  # Make sure we're checking an ext2 image
  local sb_magic_offset=$((0x438))
  local sb_value=$(sudo dd if="$rootfs" skip=$((offset + sb_magic_offset)) \
                   count=2 bs=1 2>/dev/null)
  local expected_sb_value=$(printf '\123\357')
  if [ "$sb_value" = "$expected_sb_value" ]; then
    return 0
  fi
  return 1
}
bb() {
    local font_blue="\033[94m"
    local font_bold="\033[1m"
    local font_end="\033[0m"

    echo -e "\n${font_blue}${font_bold}${1}${font_end}"
}
breg() {
    local font_blue="\033[94m"
    local font_end="\033[0m"
    echo -e "${font_blue}${1}${font_end}"
}
rb() {
    local font_red="\x1b[1;31m"
    local font_end="\033[0m"
    echo -e "${font_red}${1}${font_end}"
}
rreg() {
    local font_red="\x1b[31m"
    local font_end="\033[0m"

    echo -e "${font_red}${1}${font_end}"
}
print_usage() {
    rb "INVALID USAGE ARGUMENTS"
    rreg "${1} <path to shim>"
}
cleanup() {
    bb "[CLEANING UP]"
    losetup -D
    umount $MOUNT_PATH &> /dev/null # we don't care if it unmounts properly we just want it to unmount
    exit -1;
}
if [ $(id -u) -gt 0 ]
then
    rb "Not running as root"
    exit -1
fi
if [ $# -lt 1 ];
then
    print_usage $0
    exit -1
fi
bb "[BUILDING PROJECT]"
make # this may take a while, do not use multiprocess

bb "[WORKING ON SHIM]"
mkdir ${MOUNT_PATH}
bb "[Mounting shim at \"${MOUNT_PATH}\"]"
LOOP_PATH=$(losetup -f)

breg "Erasing stateful ${LOOP_PATH}p1"

breg "Setting loop up to ${LOOP_PATH}"
losetup -fP ${SHIM_PATH}
mkfs.ext4 -F ${LOOP_PATH}p1 # very risky lmk if this breaks 
mount -o loop,rw ${LOOP_PATH}p1 ${MOUNT_PATH}

breg "Setting up shim stateful in ${MOUNT_PATH}"
mkdir -p ${MOUNT_PATH}/dev_image/etc
touch ${MOUNT_PATH}/dev_image/etc/lsb-release

mkdir -p ${MOUNT_PATH}/rmasmoke_root
tar -xvf build/rmasmoke_root.tar.xz -C ${MOUNT_PATH}/rmasmoke_root
mkdir ${MOUNT_PATH}/usrlocal/ -p
cp rmasmoke_shim.sh ${MOUNT_PATH}/usrlocal/rmasmoke


bb "[Working on partition 3 of shim (rootfs)]"
breg "Disabling EXT4 FS Write-Protect"
enable_rw_mount ${LOOP_PATH}p3
mount -o loop,rw ${LOOP_PATH}p3 ${MOUNT_PATH}
cp -v factory_install.sh ${MOUNT_PATH}/usr/bin/factory_install.sh
umount ${MOUNT_PATH}
umount ${MOUNT_PATH}
cleanup