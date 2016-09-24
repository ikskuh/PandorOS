
ifndef PLATFORM

PLATFORM=x86

endif

all: kernel rmbuild

kernel: makefile 
	$(MAKE) -B -C build
	mv build/kernel kernel

makefile:
	mkdir -p build
	mcs build.cs
	mono build.exe $(PLATFORM)

clean: rmbuild
	$(MAKE) -C build -B clean

rmbuild: build.exe
	rm build.exe
	
.PHONY: kernel cleanup
