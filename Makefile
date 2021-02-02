# Makefile for cbmplugs
# Copyright © 2002-2007, 2009 David Weinehall

PRGNAME := cbmplugs
VERSION := 1.2.2

CC := gcc
GIMPTOOL := gimptool-2.0
PALETTE_DIR := /usr/share/gimp/2.0/palettes

ifdef LOCAL_PLUGINS
GIMP_INSTALL_BIN := --install-bin
GIMP_UNINSTALL_BIN := --uninstall-bin
# Use Makefile as plugin to install, to be sure the file exists
PALETTE_DIR := \
	$(shell pkg-config --variable=gimpdatadir gimp-2.0)/palettes
else
GIMP_INSTALL_BIN := --install-admin-bin
GIMP_UNINSTALL_BIN := --uninstall-admin-bin
endif

# Use Makefile as plugin to install, to be sure the file exists
PLUGIN_DIR := \
	$(shell pkg-config --variable=gimplibdir gimp-2.0)/plug-ins

AM_CFLAGS += `$(GIMPTOOL) --cflags-noui`
# AM_CFLAGS += -Werror
# AM_CFLAGS += -W -Wall -Wpointer-arith -Wcast-align
# AM_CFLAGS += -Wbad-function-cast -Wsign-compare
# AM_CFLAGS += -Waggregate-return -Wmissing-noreturn -Wnested-externs
# AM_CFLAGS += -Wchar-subscripts -Wformat-security -Wmissing-prototypes

AM_LDFLAGS += `$(GIMPTOOL) --libs-noui` -lm -Wl,--allow-multiple-definition

RM := rm -f
INSTALL := install --mode=755
INSTALL_DATA := install --mode=644
INSTALL_DIR := install -d

ARCHIVE := $(PRGNAME)-$(VERSION)
MKDIR := mkdir -p
TAR_EXCLUDES := --exclude="*.swp" --exclude="*.bak" --exclude=".svnignore"
TAR_EXCLUDES += --exclude="*~" --exclude="*.o" --exclude=".svn"
TAR := tar cz $(TAR_EXCLUDES) --file

PLUGINS	= \
	advartstudio afli artist64 artstudio blazingpaddles drazpaint	\
	eafli fli64 imagesystem interpainthires interpaintlores koala	\
	rain shfli shflixl sprite suxx twilight vidcom64 hws

PALETTES = \
	Commodore64.gpl Commodore64-c64wiki.gpl DTV.gpl

INCLUDES = cbmplugs.c
HEADERS = cbmplugs.h

.PHONY: default
default: $(PLUGINS)

$(PLUGINS): %: %.c $(INCLUDES) $(HEADERS)
	$(CC) $(CFLAGS) $(AM_CFLAGS) -o $@ $< $(INCLUDES) $(AM_LDFLAGS)

.PHONY: install-plugins
install-plugins:
	$(INSTALL_DIR) $(DESTDIR)$(PLUGIN_DIR)				&&\
	for plugin in $(PLUGINS); do					\
		$(INSTALL) $$plugin $(DESTDIR)$(PLUGIN_DIR);		\
	done

.PHONY: install-palettes
install-palettes:
	$(INSTALL_DIR) $(DESTDIR)$(PALETTE_DIR)				&&\
	for palette in $(PALETTES); do					\
		$(INSTALL_DATA) $$palette $(DESTDIR)$(PALETTE_DIR);	\
	done

.PHONY: install
install: install-plugins install-palettes

.PHONY: uninstall-plugins
uninstall-plugins:
	for plugin in $(PLUGINS); do					\
		$(RM) $(PLUGIN_DIR)/$$plugin;				\
	done

.PHONY: uninstall-palettes
uninstall-palettes:
	for palette in $(PALETTES); do					\
		$(RM) $(PALETTE_DIR)/$$palette;				\
	done

.PHONY: uninstall
uninstall: uninstall-plugins uninstall-palettes

.PHONY: clean
clean:
	for plugin in $(PLUGINS); do					\
		$(RM) $$plugin;						\
	done

.PHONY: archive
archive: clean
	$(RM) ../$(ARCHIVE).tar.gz					&&\
	$(RM) -r $(ARCHIVE)						&&\
	$(MKDIR) $(ARCHIVE)						&&\
	find . -maxdepth 1 -path '*/$(ARCHIVE)' -prune -o		\
		-not -name '.' -exec cp -a {} $(ARCHIVE) \;		&&\
	$(TAR) ../$(ARCHIVE).tar.gz $(ARCHIVE)				&&\
	$(RM) -r $(ARCHIVE)
