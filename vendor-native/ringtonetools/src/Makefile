#DEBUG = -DDEBUG
CC = gcc

# static build, we want to run a sandbox later
WARN = -static

# from https://github.com/ossf/wg-best-practices-os-developers/blob/main/docs/Compiler_Hardening_Guides/Compiler-Options-Hardening-Guide-for-C-and-C%2B%2B.md
FLAGS = $(DEBUG) -O2 -Wall -Wformat=2 -Wtrampolines \
-D_FORTIFY_SOURCE=3 \
-D_GLIBCXX_ASSERTIONS \
-fstack-clash-protection -fstack-protector-strong \
-Wl,-z,nodlopen -Wl,-z,nodump -Wl,-z,noexecstack -Wl,-z,noexecheap \
-Wl,-z,relro -Wl,-z,now \
-fPIE -pie -fPIC

#DSP = -DDSP
#WINDOWS = -DWINDOWS -lwinmm

default:
	$(CC) -c 3210.c $(FLAGS) $(WARN)
	$(CC) -c bmp.c $(FLAGS) $(WARN)
	$(CC) -c emelody.c $(FLAGS) $(WARN)
	$(CC) -c ems.c $(FLAGS) $(WARN)
	$(CC) -c fileoutput.c $(FLAGS) $(WARN) $(DSP)
	$(CC) -c general.c $(FLAGS) $(WARN)
	$(CC) -c gif.c $(FLAGS) $(WARN)
	$(CC) -c imelody.c $(FLAGS) $(WARN)
	$(CC) -c kyocera.c $(FLAGS) $(WARN)
	$(CC) -c kyocera6035.c $(FLAGS) $(WARN)
	$(CC) -c midi.c $(FLAGS) $(WARN)
	$(CC) -c motorola.c $(FLAGS) $(WARN)
	$(CC) -c nokia.c $(FLAGS) $(WARN)
	$(CC) -c parse_3210.c $(FLAGS) $(WARN)
	$(CC) -c parse_bmp.c $(FLAGS) $(WARN)
	$(CC) -c parse_emelody.c $(FLAGS) $(WARN)
	$(CC) -c parse_ems.c $(FLAGS) $(WARN)
	$(CC) -c parse_gif.c $(FLAGS) $(WARN)
	$(CC) -c parse_imelody.c $(FLAGS) $(WARN)
	$(CC) -c parse_kws.c $(FLAGS) $(WARN)
	$(CC) -c parse_logo.c $(FLAGS) $(WARN)
	$(CC) -c parse_midi.c $(FLAGS) $(WARN)
	$(CC) -c parse_morsecode.c $(FLAGS) $(WARN)
	$(CC) -c parse_rtttl.c $(FLAGS) $(WARN)
	$(CC) -c parse_siemens.c $(FLAGS) $(WARN)
	$(CC) -c parse_sckl.c $(FLAGS) $(WARN)
	$(CC) -c rtttl.c $(FLAGS) $(WARN)
	$(CC) -c samsung.c $(FLAGS) $(WARN)
	$(CC) -c siemens.c $(FLAGS) $(WARN)
	$(CC) -c seo.c $(FLAGS) $(WARN)
	$(CC) -c treo.c $(FLAGS) $(WARN)
	$(CC) -c wav.c $(FLAGS) $(WARN) $(DSP)
	$(CC) -o ringtonetools ringtonetools.c 3210.o bmp.o emelody.o \
	         ems.o fileoutput.o general.o gif.o imelody.o kyocera.o\
	         kyocera6035.o midi.o motorola.o nokia.o parse_3210.o \
	         parse_bmp.o parse_emelody.o parse_ems.o parse_gif.o \
	         parse_imelody.o parse_kws.o parse_logo.o parse_midi.o \
	         parse_morsecode.o parse_rtttl.o parse_siemens.o \
	         parse_sckl.o rtttl.o samsung.o siemens.o seo.o treo.o \
	         wav.o $(FLAGS) $(WARN) $(DSP) $(LIBS)

clean:
	@if [ -e 3210.o ]; then rm 3210.o; fi;
	@if [ -e api.o ]; then rm api.o; fi;
	@if [ -e bmp.o ]; then rm bmp.o; fi;
	@if [ -e emelody.o ]; then rm emelody.o; fi;
	@if [ -e ems.o ]; then rm ems.o; fi;
	@if [ -e fileoutput.o ]; then rm fileoutput.o; fi;
	@if [ -e general.o ]; then rm general.o; fi;
	@if [ -e gif.o ]; then rm gif.o; fi;
	@if [ -e imelody.o ]; then rm imelody.o; fi;
	@if [ -e kyocera.o ]; then rm kyocera.o; fi;
	@if [ -e kyocera6035.o ]; then rm kyocera6035.o; fi;
	@if [ -e midi.o ]; then rm midi.o; fi;
	@if [ -e motorola.o ]; then rm motorola.o; fi;
	@if [ -e nokia.o ]; then rm nokia.o; fi;
	@if [ -e parse_3210.o ]; then rm parse_3210.o; fi;
	@if [ -e parse_bmp.o ]; then rm parse_bmp.o; fi;
	@if [ -e parse_emelody.o ]; then rm parse_emelody.o; fi;
	@if [ -e parse_ems.o ]; then rm parse_ems.o; fi;
	@if [ -e parse_gif.o ]; then rm parse_gif.o; fi;
	@if [ -e parse_imelody.o ]; then rm parse_imelody.o; fi;
	@if [ -e parse_kws.o ]; then rm parse_kws.o; fi;
	@if [ -e parse_logo.o ]; then rm parse_logo.o; fi;
	@if [ -e parse_midi.o ]; then rm parse_midi.o; fi;
	@if [ -e parse_morsecode.o ]; then rm parse_morsecode.o; fi;
	@if [ -e parse_rtttl.o ]; then rm parse_rtttl.o; fi;
	@if [ -e parse_siemens.o ]; then rm parse_siemens.o; fi;
	@if [ -e parse_sckl.o ]; then rm parse_sckl.o; fi;
	@if [ -e rtttl.o ]; then rm rtttl.o; fi;
	@if [ -e samsung.o ]; then rm samsung.o; fi;
	@if [ -e siemens.o ]; then rm siemens.o; fi;
	@if [ -e seo.o ]; then rm seo.o; fi;
	@if [ -e treo.o ]; then rm treo.o; fi;
	@if [ -e wav.o ]; then rm wav.o; fi;
	@if [ -e ringtonetools ]; then rm ringtonetools; fi;
	@if [ -e ringtonetools.exe ]; then rm ringtonetools.exe; fi;
	@if [ -e ringtonetools.dll ]; then rm ringtonetools.dll; fi;
	@if [ -e ringtonetools.so ]; then rm ringtonetools.so; fi;

dll: default
	$(CC) -c api.c $(FLAGS) $(WARN)
	dllwrap --def ringtonetools.def --dllname ringtonetools -o \
	        ringtonetools.dll \
	         3210.o api.o bmp.o emelody.o \
	         ems.o fileoutput.o general.o gif.o imelody.o kyocera.o\
	         kyocera6035.o midi.o motorola.o nokia.o parse_3210.o \
	         parse_bmp.o parse_emelody.o parse_ems.o parse_gif.o \
	         parse_imelody.o parse_kws.o parse_logo.o parse_midi.o \
	         parse_morsecode.o parse_rtttl.o parse_siemens.o \
	         parse_sckl.o rtttl.o samsung.o siemens.o seo.o treo.o \
	         wav.o

lib: default
	$(CC) -c api.c $(FLAGS) $(WARN)
	gcc -o ringtonetools.so -shared\
	         3210.o api.o bmp.o emelody.o \
	         ems.o fileoutput.o general.o gif.o imelody.o kyocera.o\
	         kyocera6035.o midi.o motorola.o nokia.o parse_3210.o \
	         parse_bmp.o parse_emelody.o parse_ems.o parse_gif.o \
	         parse_imelody.o parse_kws.o parse_logo.o parse_midi.o \
	         parse_morsecode.o parse_rtttl.o parse_siemens.o \
	         parse_sckl.o rtttl.o samsung.o siemens.o seo.o treo.o \
	         wav.o



