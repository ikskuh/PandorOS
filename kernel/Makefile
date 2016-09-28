
ifndef PLATFORM

PLATFORM=x86

endif

HAL_ROOT=./hal/$(PLATFORM)/
HAL_INIT=$(HAL_ROOT)init.lua
HAL_CONFIG=$(HAL_ROOT)config.h

LEX=flex

all: kernel rmbuild

kernel: initfiles makefile 
	$(MAKE) -B -C build
	mv build/kernel kernel
	
makefile:
	mkdir -p build
	mcs build.cs
	mono build.exe $(PLATFORM)

initfiles: config.h init.lua
	lua init.lua $(PLATFORM)
ifneq ("$(wildcard $(HAL_INIT))","")
	cd ./hal/$(PLATFORM); lua init.lua
else
	echo "HAL has no custom initialization mechanism."
endif

config.h:
	echo "#pragma once" > include/config.h
	echo "#define HAL" $(PLATFORM) >> include/config.h
ifneq ("$(wildcard $(HAL_CONFIG))","")
	echo "#include \"../hal/"$(PLATFORM)"/config.h\"" >> include/config.h
endif
	echo "#if !defined(PMM_MEMSIZE)" >> include/config.h
	echo "#error \"PMM_MEMSIZE must be defined!\"" >> include/config.h
	echo "#endif" >> include/config.h
	echo "#if !defined(PMM_PAGESIZE)" >> include/config.h
	echo "#error \"PMM_PAGESIZE must be defined!\"" >> include/config.h
	echo "#endif" >> include/config.h

clean: rmbuild
	$(MAKE) -C build -B clean

rmbuild: build.exe
	rm build.exe
	
.PHONY: kernel cleanup
