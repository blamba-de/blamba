all: build generate

build:
	gcc -lseccomp -o genseccomp-imagemagick genseccomp-imagemagick.c
	gcc -lseccomp -o genseccomp-ringtonetools genseccomp-ringtonetools.c

generate: build
	./genseccomp-imagemagick > seccomp-imagemagick.bpf
	./genseccomp-ringtonetools > seccomp-ringtonetools.bpf
clean:
	rm -f seccomp-ringtonetools.bpf
	rm -f genseccomp-ringtonetools
	rm -f seccomp-imagemagick.bpf
	rm -f genseccomp-imagemagick
