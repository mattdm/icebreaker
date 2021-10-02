CC=gcc

PKG_CONFIG ?= pkg-config

ifndef prefix
  prefix=/usr/local
  #prefix=/usr
endif

ifndef datadir
  datadir=$(prefix)/share
endif

ifndef mandir
  mandir=$(prefix)/share/man
endif

ifndef bindir
  bindir=$(prefix)/bin
endif


CFLAGS+=-Wall -Werror $(OPTIMIZE) $(SDL_CFLAGS) -DDATAPREFIX=\"$(datadir)/icebreaker\"

SRC=icebreaker.c cursor.c grid.c laundry.c line.c penguin.c sound.c \
    level.c intro.c text.c status.c transition.c hiscore.c dialog.c \
    menu.c options.c fullscreen.c themes.c event.c titlebar.c benchmark.c \
    misc.c lock.c delay.c

DISTFILES=$(wildcard *.c *.h *.bmp *.png *.ibt *.wav *.sh *.rc *.ico *.desktop *.man.in *.nsi.in *.spec README* TODO LICENSE INSTALL ChangeLog Makefile*)

SDL_MIXER := $(shell $(PKG_CONFIG) SDL_mixer --libs)
SDL_LIB=$(SDL_MIXER) $(SDL_LDFLAGS)
SDL_CFLAGS := $(shell $(PKG_CONFIG) sdl --cflags)
SDL_LDFLAGS := $(shell $(PKG_CONFIG) sdl --libs)
VERSION := $(shell awk '/^#define VERSION/ { print $$3 }' icebreaker.h)
VERDATE := $(shell LC_ALL=C date -u -r icebreaker.h +"%d %B %Y")

WINARCH=i686
CROSSTOOLSPATH=/usr/$(WINARCH)-w64-mingw32
UNIX2DOS=unix2dos
MAKENSIS=makensis
WINDLLS=SDL.dll SDL_mixer.dll libgcc_s_dw2-1.dll libvorbisfile-3.dll libvorbis-0.dll libogg-0.dll libssp-0.dll libwinpthread-1.dll
export WINARCH

RPMARCH := $(shell  rpm --eval %{_arch} )
RPMOPTS=
RPMCRAZYDEFINES := --define "_topdir %(pwd)/rpmbuild" --define "_sourcedir %(pwd)" --define "_specdir %(pwd)"  --define "_tmppath %(pwd)/rpmbuild/tmp/"  --define "_builddir %(pwd)/rpmbuild/build/" --define "_rpmdir %(pwd)" --define "_srcrpmdir %(pwd)" --define "_rpmfilename %%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm"

ifneq ($(VERSION),$(shell awk '/^Version:/ { print $$2 }' icebreaker.spec))
  $(error Version in spec file doesn't match version in icebreaker.h! )
endif

RPMRELEASE := $(shell awk '/^Release:/ { print $$2 }' icebreaker.spec)
PRERELEASE := $(shell awk '/^Release:/ { print $$2 }' icebreaker.spec | grep pre)

ifneq ($(strip $(PRERELEASE)),)
  ifeq ($(shell awk '/^%define.*isprerelease/ { print $$3 }' icebreaker.spec),0)
    $(error .spec file has -pre release tag, but sets "isprerelease" to 0.  Fix that)
  endif
  ifndef OPTIMIZE
    OPTIMIZE=-g
  endif
  VERSIONSTRING := $(VERSION)-$(PRERELEASE)
  $(warning Building prerelease version.)
else
  ifeq ($(shell awk '/^%define.*isprerelease/ { print $$3 }' icebreaker.spec),1)
    $(error .spec file has final release tag, but sets "isprerelease" to 1.  Fix that)
  endif
  ifndef OPTIMIZE
    OPTIMIZE=-O2
  endif
  VERSIONSTRING := $(VERSION)
endif


all:	icebreaker man

.PHONY: clean
clean:
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	[ -d rpmbuild ] && rm -rf rpmbuild || true
	[ -d win32.build ] && rm -rf win32.build || true
	-rm -f icebreaker
	-rm -f icebreaker.6
	-rm -f icebreaker.nsi
	-rm -f *.o
	-rm -f *.d
	-rm -f *.tar.xz
	-rm -f *.zip
	-rm -f *.exe
	-rm -f *.rpm

dist: tgz win32 rpm
	ls -lh --color=yes icebreaker-$(VERSION)*

tgz: icebreaker-$(VERSIONSTRING).tar.xz

icebreaker-$(VERSIONSTRING).tar.xz: $(DISTFILES)
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	mkdir icebreaker-$(VERSIONSTRING)
	cp -p * icebreaker-$(VERSIONSTRING) || true
	(cd icebreaker-$(VERSIONSTRING); make clean)
	tar cJf icebreaker-$(VERSIONSTRING).tar.xz icebreaker-$(VERSIONSTRING)
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	tar tf icebreaker-$(VERSIONSTRING).tar.xz

rpm: srcrpm binrpm

srcrpm: icebreaker-$(VERSION)-$(RPMRELEASE).src.rpm 
binrpm: $(RPMARCH)rpm
$(RPMARCH)rpm: icebreaker-$(VERSION)-$(RPMRELEASE).$(RPMARCH).rpm

icebreaker-$(VERSION)-$(RPMRELEASE).src.rpm: icebreaker-$(VERSIONSTRING).tar.xz icebreaker.spec
	[ -d rpmbuild ] && rm -rf rpmbuild || true
	mkdir -p rpmbuild/tmp
	rpmbuild $(RPMCRAZYDEFINES) -bs icebreaker.spec
	[ -d rpmbuild ] && rm -rf rpmbuild || true

icebreaker-$(VERSION)-$(RPMRELEASE).$(RPMARCH).rpm: icebreaker-$(VERSIONSTRING).tar.xz icebreaker.spec
	[ -d rpmbuild ] && rm -rf rpmbuild || true
	mkdir -p rpmbuild/tmp; mkdir -p rpmbuild/build
	rpmbuild $(RPMCRAZYDEFINES) -bb icebreaker.spec
	[ -d rpmbuild ] && rm -rf rpmbuild || true

win32: icebreaker-$(VERSIONSTRING).exe

icebreaker-$(VERSIONSTRING).exe: icebreaker.nsi icebreaker-$(VERSIONSTRING).zip
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	unzip -b icebreaker-$(VERSIONSTRING).zip
	touch icebreaker-$(VERSIONSTRING)/lockhelper.lck
	(cd icebreaker-$(VERSIONSTRING); \
	   $(MAKENSIS) -NOCD -INPUTCHARSET UTF8 ../icebreaker.nsi; \
	   test -f icebreaker-$(VERSIONSTRING).exe)
	mv -f icebreaker-$(VERSIONSTRING)/icebreaker-$(VERSIONSTRING).exe icebreaker-$(VERSIONSTRING).exe
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true	
	ls -l icebreaker-$(VERSIONSTRING).exe

zip: icebreaker-$(VERSIONSTRING).zip

icebreaker-$(VERSIONSTRING).zip: icebreaker.exe icebreaker-$(VERSIONSTRING).tar.xz
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	mkdir icebreaker-$(VERSIONSTRING)
	cp icebreaker.exe icebreaker-$(VERSIONSTRING)
	cp icebreaker-$(VERSIONSTRING).tar.xz icebreaker-$(VERSIONSTRING)/icebreaker-$(VERSIONSTRING)-src.tar.xz
	for dll in $(WINDLLS); do cp $(CROSSTOOLSPATH)/sys-root/mingw/bin/$$dll icebreaker-$(VERSIONSTRING); done
	cp *.wav icebreaker-$(VERSIONSTRING)
	cp *.bmp icebreaker-$(VERSIONSTRING)
	cp *.png icebreaker-$(VERSIONSTRING)
	for i in *.ibt; do $(UNIX2DOS) -n $$i icebreaker-$(VERSIONSTRING)/$$i; done
	$(UNIX2DOS) -n ChangeLog icebreaker-$(VERSIONSTRING)/ChangeLog.txt
	$(UNIX2DOS) -n LICENSE icebreaker-$(VERSIONSTRING)/LICENSE.txt
	$(UNIX2DOS) -n README icebreaker-$(VERSIONSTRING)/README.txt
	$(UNIX2DOS) -n README.SDL icebreaker-$(VERSIONSTRING)/README-SDL.txt
	$(UNIX2DOS) -n README.themes icebreaker-$(VERSIONSTRING)/README-themes.txt
	$(UNIX2DOS) -n TODO icebreaker-$(VERSIONSTRING)/TODO.txt
	zip -r icebreaker-$(VERSIONSTRING).zip icebreaker-$(VERSIONSTRING)
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	unzip -t icebreaker-$(VERSIONSTRING).zip

beos:
	make -f Makefile.beos

osx:
	make -f Makefile.osx

icebreaker.exe: $(DISTFILES)
	[ -f $(CROSSTOOLSPATH)/sys-root/mingw/bin/SDL.dll ]
	[ -d win32.build ] && rm -rf win32.build || true
	mkdir win32.build
	cp -p * win32.build || true
	(cd win32.build; make clean; make -f Makefile.w32; mv icebreaker.exe ..)
	[ -d win32.build ] && rm -rf win32.build || true

icebreaker:	$(SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o icebreaker $(SDL_LIB) $(LDFLAGS)

man: icebreaker.6

%.nsi: %.nsi.in icebreaker.spec icebreaker.h
	sed 's/\%ICEBREAKERVERSION/$(VERSIONSTRING)/' $< > $@

%.6: %.man.in
	sed 's/\$$VERSION/$(VERSION)/' $< | \
	   sed 's/\$$VERDATE/$(VERDATE)/' > $@

install-mkdirs:
	install -m 755 -d $(datadir)/icebreaker
	install -m 755 -d $(bindir)
	install -m 755 -d $(mandir)/man6

themes-install: install-themes

install-themes: install-mkdirs
	install -m 644 *.ibt $(datadir)/icebreaker
	install -m 644 *.wav *.bmp *.png $(datadir)/icebreaker

install-bin: install-mkdirs icebreaker
	install -m 755 icebreaker $(bindir)

install-man: install-mkdirs icebreaker.6
	install -m 644 icebreaker.6 $(mandir)/man6

install: all install-mkdirs install-themes install-bin install-man

%.d: %.c
	set -e; $(CC) -M $(CFLAGS) $< \
           | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
           [ -s $@ ] || rm -f $@

include $(SRC:.c=.d)
