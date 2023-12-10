#!/bin/bash
if [ ! -d "build" ]; then
	mkdir build
	cd build
	cmake ..
	cd ..
fi
cd build
make -j16&&[[ -d "$PICO_DRIVE_PATH" ]]&&cp wifi_monitor.uf2 "$PICO_DRIVE_PATH/wifi_monitor.uf2"
cd ..
