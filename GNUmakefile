PACKAGE := vmfs-tools

define PATH_LOOKUP
$(wildcard $(foreach path,$(subst :, ,$(PATH)),$(path)/$(1)))
endef

CC := gcc
OPTIMFLAGS := -O2
CFLAGS := -Wall $(OPTIMFLAGS) -g -D_FILE_OFFSET_BITS=64 $(EXTRA_CFLAGS)
ifneq (,$(call PATH_LOOKUP,pkg-config))
UUID_LDFLAGS := $(shell pkg-config --libs uuid)
FUSE_LDFLAGS := $(shell pkg-config --libs fuse)
FUSE_CFLAGS := $(shell pkg-config --cflags fuse)
else
UUID_LDFLAGS := -luuid
endif
LDFLAGS := $(UUID_LDFLAGS)
SRC := $(wildcard *.c)
HEADERS := $(wildcard *.h)
OBJS := $(SRC:%.c=%.o)
PROGRAMS := debugvmfs vmfs-fuse
buildPROGRAMS := $(PROGRAMS)
ifeq (,$(FUSE_LDFLAGS))
buildPROGRAMS := $(filter-out vmfs-fuse,$(buildPROGRAMS))
endif
MANSRCS := $(wildcard $(buildPROGRAMS:%=%.txt))
DOCBOOK_XSL :=	http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl

ifneq (,$(call PATH_LOOKUP,asciidoc))
ifneq (,$(call PATH_LOOKUP,xsltproc))
ifneq (,$(shell xsltproc --nonet --noout $(DOCBOOK_XSL) && echo ok))
MANDOCBOOK := $(MANSRCS:%.txt=%.xml)
MANPAGES := $(foreach man,$(MANSRCS),$(shell sed '1{s/(/./;s/)//;q}' $(man)))
endif
endif
endif

EXTRA_DIST := LICENSE README TODO AUTHORS

prefix := /usr/local
exec_prefix := $(prefix)
sbindir := $(exec_prefix)/sbin
datarootdir := $(prefix)/share
mandir := $(datarootdir)/man

all: $(buildPROGRAMS) $(wildcard .gitignore)

version: $(MAKEFILE_LIST) $(SRC) $(HEADERS) $(wildcard .git/logs/HEAD .git/refs/tags)
	(if [ -d .git ]; then \
		VER=$$(git describe --match "v[0-9].*" --abbrev=0 HEAD); \
		git update-index -q --refresh; \
		if git diff-index --name-status $${VER} -- | grep -q -v '^A'; then \
			VER=$$(git describe --match "v[0-9].*" --abbrev=4 HEAD | sed s/-/./g); \
		fi ; \
		if [ -z "$${VER}" ]; then \
			VER="v0.0.0.$$(git rev-list HEAD | wc -l).$$(git rev-parse --short=4 HEAD)"; \
		fi; \
		test -z "$$(git diff-index --name-only HEAD --)" || VER=$${VER}-dirty; \
        else \
		VER=$(patsubst %,%-patched,$(or $(VERSION:%-patched=%),v0.0.0)); \
	fi; \
	echo VERSION := $${VER}) > $@ 2> /dev/null
-include version

vmfs-fuse: LDFLAGS+=$(FUSE_LDFLAGS)
vmfs-fuse.o: CFLAGS+=$(FUSE_CFLAGS)

debugvmfs_EXTRA_SRCS := readcmd.c
debugvmfs: LDFLAGS+=-lreadline
debugvmfs.o: CFLAGS+=-DVERSION=\"$(VERSION)\"

define program_template
$(strip $(1))_EXTRA_OBJS := $$($(strip $(1))_EXTRA_SRCS:%.c=%.o)
LIBVMFS_EXCLUDE_OBJS += $(1).o $$($(strip $(1))_EXTRA_OBJS)
$(1): $(1).o $$($(strip $(1))_EXTRA_OBJS) libvmfs.a
endef
$(foreach program, $(buildPROGRAMS), $(eval $(call program_template,$(program))))

libvmfs.a: $(filter-out $(LIBVMFS_EXCLUDE_OBJS),$(OBJS))
	ar -r $@ $^
	ranlib $@

$(OBJS): %.o: %.c $(HEADERS)

$(buildPROGRAMS):
	$(CC) -o $@ $^ $(LDFLAGS)

clean: CLEAN := $(wildcard libvmfs.a $(PROGRAMS) $(OBJS) $(PACKAGE)-*.tar.gz $(MANPAGES) $(MANDOCBOOK))
clean:
	$(if $(CLEAN),rm $(CLEAN))

ALL_DIST := $(SRC) $(HEADERS) $(MAKEFILE_LIST) $(MANSRCS) $(EXTRA_DIST)
DIST_DIR := $(PACKAGE)-$(VERSION:v%=%)
dist: $(ALL_DIST)
	@rm -rf "$(DIST_DIR)"
	@mkdir "$(DIST_DIR)"
	cp -p $(ALL_DIST) $(DIST_DIR)
	tar -zcf "$(DIST_DIR).tar.gz" "$(DIST_DIR)"
	@rm -rf "$(DIST_DIR)"

$(MANDOCBOOK): %.xml: %.txt
	asciidoc -a manversion=$(VERSION:v%=%) -a manmanual=$(PACKAGE) -b docbook -d manpage -o $@ $<

$(MANPAGES): %.8: %.xml
	xsltproc -o $@ --nonet --novalid $(DOCBOOK_XSL) $<

doc: $(MANPAGES)

$(DESTDIR)/%:
	install -d -m 0755 $@

installPROGRAMS := $(buildPROGRAMS:%=$(DESTDIR)$(sbindir)/%)
installMANPAGES := $(MANPAGES:%=$(DESTDIR)$(mandir)/man8/%)

$(installPROGRAMS): $(DESTDIR)$(sbindir)/%: % $(DESTDIR)$(sbindir)
	install $(if $(NO_STRIP),,-s )-m 0755 $< $(dir $@)

$(installMANPAGES): $(DESTDIR)$(mandir)/man8/%: % $(DESTDIR)$(mandir)/man8
	install -m 0755 $< $(dir $@)

install: $(installPROGRAMS) $(installMANPAGES)

.PHONY: all clean dist install doc

.gitignore: $(MAKEFILE_LIST)
	(echo "*.tar.gz"; \
	 echo "*.[ao]"; \
	 echo "*.xml"; \
	 echo "*.8"; \
	 echo "version"; \
	 $(foreach program, $(PROGRAMS),echo $(program);) \
	) > $@
