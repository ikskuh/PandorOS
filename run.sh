#!/bin/bash

# qemu-system-arm -M versatilepb -hda hdd.img -m 256 -kernel kernel.bin -append "root=/dev/sda" -localtime -usb -usbdevice wacom-tablet -show-cursor
qemu-system-arm \
	-M versatilepb \
	-m 256 \
	-kernel kernel.bin \
	-append "root=/dev/sda" \
	-localtime \
	-usb \
	-usbdevice wacom-tablet \
	-show-cursor
