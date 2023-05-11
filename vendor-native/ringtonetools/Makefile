
PREFIX=/usr/local

all:
	@cd src && make
	@if [ -e src/ringtonetools.exe ]; then mv src/ringtonetools.exe ringtonetools.exe; fi
	@if [ -e src/ringtonetools ]; then mv src/ringtonetools .; fi

dll:
	@cd src && make dll
	@if [ -e src/ringtonetools.dll ]; then mv src/ringtonetools.dll .; fi

lib:
	@cd src && make lib
	@if [ -e src/ringtonetools.so ]; then mv src/ringtonetools.so .; fi


clean:
	@if [ -e ringtonetools ]; then rm ringtonetools; fi
	@if [ -e ringtonetools.exe ]; then rm ringtonetools.exe; fi
	@if [ -e ringtonetools.dll ]; then rm ringtonetools.dll; fi
	@if [ -e ringtonetools.so ]; then rm ringtonetools.so; fi
	@cd src && make clean
	@echo "Clean!"

install:
	@if [ ! -d $(PREFIX)/bin ]; then mkdir -p $(PREFIX)/bin; fi
	@if [ ! -d $(PREFIX)/lib ]; then mkdir -p $(PREFIX)/lib; fi
	@if [ -e ringtonetools ]; then install ringtonetools $(PREFIX)/bin; fi
	@if [ -e ringtonetools.so ]; then install ringtonetools.so $(PREFIX)/lib; fi
	@if [ -e src/ringtonetools.h ]; then install src/ringtonetools.h $(PREFIX)/include; fi

uninstall:
	@if [ -e $(PREFIX)/bin/ringtonetools ]; then rm $(PREFIX)/bin/ringtonetools; fi
	@if [ -e $(PREFIX)/lib/ringtonetools.so ]; then rm $(PREFIX)/lib/ringtonetools.so; fi
	@if [ -e $(PREFIX)/include/ringtonetools.h ]; then rm $(PREFIX)/include/ringtonetools.h; fi

solaris:
	pkgmk -d /tmp
	pkgtrans -s /tmp ringtonetools-2.19-sol9-sparc-local KOHNmobl


