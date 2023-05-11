all: build generate

build:
	gcc -lseccomp -o genseccomp genseccomp.c
generate: build
	./genseccomp > seccomp.bpf
clean:
	rm -f seccomp.bpf
	rm -f genseccomp
