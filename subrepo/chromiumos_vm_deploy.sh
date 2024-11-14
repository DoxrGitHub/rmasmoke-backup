make 
scp -P 2222 ./build/rmasmoke_root.tar.xz ./rmasmoke_shim.sh root@localhost:/mnt/stateful_partition/
echo "Run deploy_rmasmoke in the vm to setup rmasmoke in the shim"
