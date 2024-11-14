initiate-builder:
	docker build -t rmasmoke_builder buildenv
	sudo rm -rvf build
	docker run -it -v $(PWD):/rmasmoke --rm rmasmoke_builder:latest  bash -c "cd /rmasmoke && bash ./buildenv/build.sh" 
	docker build -t rmasmoke_root dockerroot
	docker run -it --name rmasmoke_root -v $(PWD):/rmasmoke rmasmoke_root:latest bash -c "cd /rmasmoke && bash ./dockerroot/init.sh"
	sudo docker export rmasmoke_root -o build/rmasmoke_root.tar.xz
	docker rm rmasmoke_root
	sudo chown --recursive 1000 build
deploy-cros:
	sh chromiumos_vm_deploy.sh