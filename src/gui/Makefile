QTDIR=/usr/lib
QMAKE4= qmake-qt4
export QTDIR QMAKE4

all: Makefile.qmake
	$(MAKE) -f Makefile.qmake
	strip --strip-unneeded qpxtool
	lrelease -verbose qpxtool.pro

clean: Makefile.qmake
	rm -f Makefile.qmake
	rm -f qpxtool
	rm -f obj/*
	rm -f moc/*
	rm -f *~
	rm -f src/*~
	rm -f include/*~
	rm -f locale/*.qm
	rm -f qrc_qpxtool.cpp

install: Makefile.qmake
	install -m 755 qpxtool $(DESTDIR)$(BINDIR)/qpxtool
	mkdir -p $(DESTDIR)$(PREFIX)/share/qpxtool/locale
	install -m 644 locale/*.qm $(DESTDIR)$(PREFIX)/share/qpxtool/locale
	mkdir -p $(DESTDIR)$(PREFIX)/share/pixmaps
	install -m 644 images/q.png $(DESTDIR)$(PREFIX)/share/pixmaps/qpxtool.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	install -m 644 qpxtool.desktop $(DESTDIR)$(PREFIX)/share/applications/qpxtool.desktop

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/qpxtool
	rm -rf $(DESTDIR)$(PREFIX)/share/qpxtool
	rm -f $(DESTDIR)$(PREFIX)/share/pixmaps/qpxtool.png
	rm -f $(DESTDIR)$(PREFIX)/share/applications/qpxtool.desktop

Makefile.qmake:
	rm -f Makefile.qmake
	$(QMAKE4) LIBS+=-L../lib/lib LIBS+=-lqpxtransport  -o Makefile.qmake

.PHONY: all clean install uninstall
