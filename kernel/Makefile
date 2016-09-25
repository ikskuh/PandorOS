
ifndef PLATFORM

PLATFORM=x86

endif

HAL_INIT=./hal/$(PLATFORM)/init.lua

all: kernel rmbuild

kernel: initfiles makefile 
	$(MAKE) -B -C build
	mv build/kernel kernel

makefile:
	mkdir -p build
	mcs build.cs
	mono build.exe $(PLATFORM)

initfiles: init.lua
	lua init.lua
ifneq ("$(wildcard $(HAL_INIT))","")
	cd ./hal/$(PLATFORM); lua init.lua
else
	echo "HAL has no custom initialization mechanism."
endif

clean: rmbuild
	$(MAKE) -C build -B clean

rmbuild: build.exe
	rm build.exe
	
.PHONY: kernel cleanup
