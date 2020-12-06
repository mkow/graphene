# Use one of these commands to build the manifest for Bash:
#
# - make
# - make DEBUG=1
# - make SGX=1
# - make SGX=1 DEBUG=1
#
# Use `make clean` to remove Graphene-generated files.

# Relative path to Graphene root and key for enclave signing
GRAPHENEDIR ?= ../..
SGX_SIGNER_KEY ?= $(GRAPHENEDIR)/Pal/src/host/Linux-SGX/signer/enclave-key.pem

ifeq ($(DEBUG),1)
GRAPHENEDEBUG = inline
else
GRAPHENEDEBUG = none
endif

.PHONY: all
all: bash.manifest | pal_loader
ifeq ($(SGX),1)
all: bash.manifest.sgx bash.token bash.sig
endif

include ../../Scripts/Makefile.configs

# Program dependencies (generated from ldd):
#
# For SGX, the manifest needs to list all the libraries loaded during the
# execution, so that the signer can include the file checksums.

# We need to replace Glibc dependencies with Graphene-specific Glibc. The Glibc
# binaries are already listed in the manifest template, so we can skip them
# from the ldd results. This list also contains some runtime deps of Bash.
GLIBC_DEPS = linux-vdso ld-linux-x86-64 libc libm librt libdl libutil libpthread

# Generating manifest rules for Bash dependencies.

# We'll duplicate some Glibc libraries (which
# Graphene provides in a customized version), but there's no harm in this. [or maybe not? TODO]
PROGRAMS = bash ls cat rm cp date
.INTERMEDIATE: trusted-libs
trusted-libs: get_deps.sh
	# this grep filter is quite bad, matching too much... TODO: fix if needed.
	# ./get_deps.sh $(foreach exec,$(PROGRAMS),$(shell which $(exec))) \
	# | grep -v $(foreach dep,$(GLIBC_DEPS),-e "$(dep)") > $@
	./get_deps.sh $(foreach exec,$(PROGRAMS),$(shell which $(exec))) > $@

# Listing all Bash dependencies, besides Glibc libraries
.INTERMEDIATE: bash.deps
%.deps:


bash.manifest: manifest.template trusted-libs
	(sed -e 's|$$(GRAPHENEDIR)|'"$(GRAPHENEDIR)"'|g' \
	     -e 's|$$(GRAPHENEDEBUG)|'"$(GRAPHENEDEBUG)"'|g' \
	     -e 's|$$(EXECDIR)|'"$(shell dirname $(shell which bash))"'|g' \
	     -e 's|$$(ARCH_LIBDIR)|'"$(ARCH_LIBDIR)"'|g' \
	     $<; \
	cat trusted-libs) > $@

# Generating the SGX-specific manifest (*.manifest.sgx), the enclave signature,
# and the token for enclave initialization.
bash.manifest.sgx: bash.manifest
	$(GRAPHENEDIR)/Pal/src/host/Linux-SGX/signer/pal-sgx-sign \
		-libpal $(GRAPHENEDIR)/Runtime/libpal-Linux-SGX.so \
		-key $(SGX_SIGNER_KEY) \
		-manifest bash.manifest -output $@

bash.sig: bash.manifest.sgx

bash.token: bash.sig
	$(GRAPHENEDIR)/Pal/src/host/Linux-SGX/signer/pal-sgx-get-token \
		-output bash.token -sig bash.sig

pal_loader:
	ln -s $(GRAPHENEDIR)/Runtime/pal_loader $@

.PHONY: regression
regression: all
	@mkdir -p scripts/testdir

	./pal_loader ./bash -c "ls" > OUTPUT
	@grep -q "Makefile" OUTPUT && echo "[ Success 1/6 ]"
	@rm OUTPUT

	./pal_loader ./bash -c "cd scripts && bash bash_test.sh 1" > OUTPUT
	@grep -q "hello 1" OUTPUT      && echo "[ Success 2/6 ]"
	@grep -q "createdfile" OUTPUT  && echo "[ Success 3/6 ]"
	@grep -q "somefile" OUTPUT     && echo "[ Success 4/6 ]"
	@grep -q "current date" OUTPUT && echo "[ Success 5/6 ]"
	@rm OUTPUT

	./pal_loader ./bash -c "cd scripts && bash bash_test.sh 3" > OUTPUT
	@grep -q "hello 3" OUTPUT      && echo "[ Success 6/6 ]"
	@rm OUTPUT

	@rm -rf scripts/testdir


.PHONY: clean
clean:
	$(RM) *.manifest *.manifest.sgx *.token *.sig trusted-libs pal_loader OUTPUT scripts/testdir/*

.PHONY: distclean
distclean: clean
