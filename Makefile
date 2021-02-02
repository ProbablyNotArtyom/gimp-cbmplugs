# Makefile for cbmplugs
# Copyright (C) 2002-2006 David Weinehall

PRGNAME := cbmplugs
VERSION := 1.2.0

EXTENSION12 := -1.2
EXTENSION20 := -2.0

ifdef GIMP12
GIMPTOOL := gimptool-1.2
PALETTE_DIR := $(DESTDIR)/usr/share/gimp/1.2/palettes
AM_CFLAGS += -DGIMP12
EXTENSION := $(EXTENSION12)
else
GIMPTOOL := gimptool-2.0
PALETTE_DIR := $(DESTDIR)/usr/share/gimp/2.0/palettes
EXTENSION := $(EXTENSION20)
endif

ifdef LOCAL_PLUGINS
GIMP_INSTALL_BIN := --install-bin
GIMP_UNINSTALL_BIN := --uninstall-bin
PALETTE_DIR := \
	`$(GIMPTOOL) --dry-run $(GIMP_INSTALL_BIN) Makefile | \
	sed -e 's/.* \(.*\)\/Makefile/\1/;s/plug-ins/palettes/'`
else
GIMP_INSTALL_BIN := --install-admin-bin
GIMP_UNINSTALL_BIN := --uninstall-admin-bin
endif

# Use Makefile as plugin to install, to be sure the file exists
PLUGIN_DIR := \
	`$(GIMPTOOL) --dry-run $(GIMP_INSTALL_BIN) Makefile | \
	tail -n 1 | sed -e 's/.* \(.*\)\/Makefile/\1/'`

AM_CFLAGS += `$(GIMPTOOL) --cflags-noui`
# AM_CFLAGS += -Werror
# AM_CFLAGS += -W -Wall -Wpointer-arith -Wcast-align
# AM_CFLAGS += -Wbad-function-cast -Wsign-compare
# AM_CFLAGS += -Waggregate-return -Wmissing-noreturn -Wnested-externs
# AM_CFLAGS += -Wchar-subscripts -Wformat-security -Wmissing-prototypes

AM_LDFLAGS += `$(GIMPTOOL) --libs-noui`

RM := rm -f
INSTALL := install --mode=755
INSTALL_DATA := install --mode=644
INSTALL_DIR := install -d

ARCHIVE := $(PRGNAME)-$(VERSION)
MKDIR := mkdir -p
TAR_EXCLUDES := --exclude="*.swp" --exclude="*.bak"
TAR_EXCLUDES += --exclude="*~" --exclude="*.o" --exclude="CVS"
TAR := tar cz $(TAR_EXCLUDES) --file

PLUGINS	= \
	advartstudio afli artist64 artstudio blazingpaddles drazpaint	\
	eafli fli64 imagesystem interpainthires interpaintlores koala	\
	rain shfli shflixl sprite suxx twilight vidcom64 hws

PALETTES = \
	Commodore64.gpl

INCLUDES = cbmplugs.c
HEADERS = cbmplugs.h

.PHONY: default
default: $(PLUGINS)

$(PLUGINS): %: %.c $(INCLUDES) $(HEADERS)
	$(CC) $(CFLAGS) $(AM_CFLAGS) -o $@$(EXTENSION) $< $(INCLUDES) $(AM_LDFLAGS)
	
.PHONY: uninstall-plugins
uninstall-plugins:
	for plugin in $(PLUGINS); do					\
		$(RM) $(PLUGIN_DIR)/$$plugin$(EXTENSION);		\
		$(RM) $(PLUGIN_DIR)/$$plugin$(EXTENSION);		\
	done

.PHONY: uninstall-palettes
uninstall-palettes:
	$(RM) $(PALETTE_DIR)/$(PALETTES)

.PHONY: uninstall
uninstall: uninstall-plugins uninstall-palettes

.PHONY: install-plugins
install-plugins:
	$(INSTALL_DIR) $(PLUGIN_DIR)					&&\
	for plugin in $(PLUGINS); do					\
		$(INSTALL) $$plugin$(EXTENSION) $(PLUGIN_DIR);		\
	done

.PHONY: install-palettes
install-palettes:
	$(INSTALL_DIR) $(PALETTE_DIR)					&&\
	$(INSTALL_DATA) $(PALETTES) $(PALETTE_DIR)

.PHONY: install
install: install-plugins install-palettes

.PHONY: clean
clean:
	for plugin in $(PLUGINS); do					\
		$(RM) $$plugin$(EXTENSION12);				\
		$(RM) $$plugin$(EXTENSION20);				\
	done

.PHONY: archive
archive: clean
	$(RM) ../$(ARCHIVE).tar.gz					&&\
	$(RM) -r $(ARCHIVE) 						&&\
	$(MKDIR) $(ARCHIVE)						&&\
	find . -maxdepth 1 -path '*/$(ARCHIVE)' -prune -o		\
		-not -name '.' -exec cp -a {} $(ARCHIVE) \;		&&\
	$(TAR) ../$(ARCHIVE).tar.gz $(ARCHIVE)				&&\
	$(RM) -r $(ARCHIVE)
