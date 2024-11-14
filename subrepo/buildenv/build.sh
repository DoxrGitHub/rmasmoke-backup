echo "Generating PROTOC Files"
sh ./generate_protoc.sh
echo "Building project"
cmake -S . -B build -G "Unix Makefiles"
cd build && make