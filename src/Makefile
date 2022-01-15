.POSIX:

NAME = nextcloud_sync_applet
LIB = $(NAME:=.so)
OBJ = $(NAME:=.o)

PKG_FLAGS = $(shell pkg-config gtk+-2.0 hildon-control-panel libosso gconf-2.0 --libs --cflags)
CFLAGS = -shared -Wall -Wextra -Werror
LDFLAGS = -Wl,--as-needed

HILDON_CONTROL_PANEL_LIBDIR = $(shell pkg-config hildon-control-panel --variable=pluginlibdir)
HILDON_CONTROL_PANEL_DATADIR = $(shell pkg-config hildon-control-panel --variable=plugindesktopentrydir)

all: $(LIB)

$(LIB): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(PKG_FLAGS) $(OBJ) -o $@

.c.o:
	$(CC) -fPIC $(CFLAGS) $(PKG_FLAGS) -c $<

install:
	mkdir -p $(DESTDIR)(HILDON_CONTROL_PANEL_LIBDIR)
	cp -f $(LIB) $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)
	chmod 644 $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)
	mkdir -p $(DESTDIR)/usr/share/gconf/schemas
	cp -f nextcloud_sync_applet.schemas $(DESTDIR)/usr/share/gconf/schemas
	mkdir -p $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)
	cp -f nextcloud_sync_applet.desktop $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)
	mkdir -p $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon
	mkdir -p $(DESTDIR)/usr/share/icons/hicolor/18x18/hildon
	cp -f nextcloud_48.png $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon
	cp -f nextcloud_18.png $(DESTDIR)/usr/share/icons/hicolor/18x18/hildon
	mkdir -p $(DESTDIR)/usr/libexec
	cp -f nextcloud-sync-daemon.sh $(DESTDIR)/usr/libexec
	chmod 755 $(DESTDIR)/usr/libexec/nextcloud-sync-daemon.sh

uninstall:
	rm -f $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)$(LIB)
	rm -f $(DESTDIR)/usr/share/gconf/schemas/nextcloud_sync_applet.schemas
	rm -f $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)/nextcloud_sync_applet.desktop
	rm -f $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon/nextcloud_48.png
	rm -f $(DESTDIR)/usr/share/icons/hicolor/18x18/hildon/nextcloud_18.png

clean:
	rm -f $(OBJ) $(LIB)

.PHONY: all install uninstall clean
