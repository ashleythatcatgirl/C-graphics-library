#!/usr/bin/env bash

readonly INCLUDE_DIR="/usr/local/include"
readonly CGLM_DIR="$INCLUDE_DIR/cglm"
readonly STB_IMAGE_DIR="$INCLUDE_DIR/stb_image"

copyCglm() {
	sudo mkdir "$CGLM_DIR"
	sudo cp -rp "include/cglm/" "$INCLUDE_DIR"
	echo "-> Copied cglm"
}

copyStbImage() {
	sudo mkdir "$STB_IMAGE_DIR"
	sudo cp -p "include/stb_image.h" "$STB_IMAGE_DIR"
	echo "-> Copied stb_image"
}

main() {
	if [[ -d "$CGLM_DIR" ]]; then
		echo "!! cglm directory already exists !!"
		echo "reinstall?"
		read -rn 1 -p "y/n" shouldDelete

		if [[ "$shouldDelete" == "y" ]]; then
			sudo rm -rdf "$CGLM_DIR"
			echo "-> Removed cglm"
			copyCglm
		fi
	else
		copyCglm
	fi


	if [[ -d "$STB_IMAGE_DIR" ]]; then
		echo "!! stb_image directory already exists !!"
		echo "reinstall?"
		read -rn 1 -p "y/n" shouldDelete

		if [[ "$shouldDelete" == "y" ]]; then
			sudo rm -rdf "$STB_IMAGE_DIR"
			echo "-> Removed stb_image"
			copyStbImage
		fi
	else
		copyStbImage
	fi

	cmake -B build
	cmake --build build
	sudo cmake --install build
	echo "-> Built and installed library"
}

main


