CC=gcc

SDLCONFIG=sdl-config

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


ifndef highscoredir
  highscoredir=/var/local/lib/games
  #highscoredir=/var/lib/games
endif


CFLAGS=-Wall $(OPTIMIZE) $(SDL_CFLAGS) -DDATAPREFIX=\"$(datadir)/icebreaker\" -DHISCOREPREFIX=\"$(highscoredir)\"

SRC=icebreaker.c cursor.c grid.c laundry.c line.c penguin.c sound.c \
    level.c intro.c text.c status.c transition.c hiscore.c dialog.c \
    menu.c options.c fullscreen.c themes.c event.c titlebar.c benchmark.c \
    misc.c lock.c delay.c

DISTFILES=$(wildcard *.c *.h *.bmp *.ibt *.wav *.sh *.rc *.ico *.desktop *.man.in *.nsi.in *.spec README* TODO LICENSE INSTALL ChangeLog Makefile*)

SDL_MIXER=-lSDL_mixer
SDL_LIB=$(SDL_MIXER) $(SDL_LDFLAGS)
SDL_CFLAGS := $(shell $(SDLCONFIG) --cflags)
SDL_LDFLAGS := $(shell $(SDLCONFIG) --libs)
VERSION := $(shell awk '/^\#define VERSION/ { print $$3 }' icebreaker.h)
VERDATE := $(shell date -r icebreaker.h +"%d %B %Y")

CROSSTOOLSPATH=/usr/local/cross-tools
# note that you almost certainly want to set wine to use the tty driver instead
# of x11 -- can you do that on the command line?
UNIX2DOS=unix2dos
MAKENSIS=wine /usr/local/NSIS/makensis.exe

RPMARCH := $(shell  rpm --eval %{_arch} )
RPMOPTS=
RPMCRAZYDEFINES := --define "_topdir %(pwd)/rpmbuild" --define "_sourcedir %(pwd)" --define "_specdir %(pwd)"  --define "_tmppath %(pwd)/rpmbuild/tmp/"  --define "_builddir %(pwd)/rpmbuild/build/" --define "_rpmdir %(pwd)" --define "_srcrpmdir %(pwd)" --define "_rpmfilename %%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm"

ifneq ($(VERSION),$(shell awk '/^Version:/ { print $$2 }' icebreaker.spec))
  $(error Version in spec file doesn't match version in icebreaker.h! )
endif

RPMRELEASE := $(shell awk '/^\Release:/ { print $$2 }' icebreaker.spec)
PRERELEASE := $(shell awk '/^\Release:/ { print $$2 }' icebreaker.spec | grep pre)

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
    OPTIMIZE=-O3
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
	-rm -f *.tar.gz
	-rm -f *.zip
	-rm -f *.exe
	-rm -f *.rpm

dist: tgz win32 rpm
	ls -lh --color=yes icebreaker-$(VERSION)*

tgz: icebreaker-$(VERSIONSTRING).tar.gz

icebreaker-$(VERSIONSTRING).tar.gz: $(DISTFILES)
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	mkdir icebreaker-$(VERSIONSTRING)
	cp -p * icebreaker-$(VERSIONSTRING) || true
	(cd icebreaker-$(VERSIONSTRING); make clean)
	tar czf icebreaker-$(VERSIONSTRING).tar.gz icebreaker-$(VERSIONSTRING)
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	tar tzf icebreaker-$(VERSIONSTRING).tar.gz

rpm: srcrpm binrpm

srcrpm: icebreaker-$(VERSION)-$(RPMRELEASE).src.rpm 
binrpm: $(RPMARCH)rpm
$(RPMARCH)rpm: icebreaker-$(VERSION)-$(RPMRELEASE).$(RPMARCH).rpm

icebreaker-$(VERSION)-$(RPMRELEASE).src.rpm: icebreaker-$(VERSIONSTRING).tar.gz icebreaker.spec
	[ -d rpmbuild ] && rm -rf rpmbuild || true
	mkdir -p rpmbuild/tmp
	rpmbuild $(RPMCRAZYDEFINES) -bs icebreaker.spec
	[ -d rpmbuild ] && rm -rf rpmbuild || true

icebreaker-$(VERSION)-$(RPMRELEASE).$(RPMARCH).rpm: icebreaker-$(VERSIONSTRING).tar.gz icebreaker.spec
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
	   $(MAKENSIS) ../icebreaker.nsi; \
	   test -f icebreaker-$(VERSIONSTRING).exe)
	mv -f icebreaker-$(VERSIONSTRING)/icebreaker-$(VERSIONSTRING).exe icebreaker-$(VERSIONSTRING).exe
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true	
	ls -l icebreaker-$(VERSIONSTRING).exe

zip: icebreaker-$(VERSIONSTRING).zip

icebreaker-$(VERSIONSTRING).zip: icebreaker.exe icebreaker-$(VERSIONSTRING).tar.gz
	[ -d icebreaker-$(VERSIONSTRING) ] && rm -rf icebreaker-$(VERSIONSTRING) || true
	mkdir icebreaker-$(VERSIONSTRING)
	cp icebreaker.exe icebreaker-$(VERSIONSTRING)
	cp icebreaker-$(VERSIONSTRING).tar.gz icebreaker-$(VERSIONSTRING)/icebreaker-$(VERSIONSTRING)-src.tar.gz
	cp $(CROSSTOOLSPATH)/i386-mingw32msvc/lib/SDL.dll icebreaker-$(VERSIONSTRING)
	cp $(CROSSTOOLSPATH)/i386-mingw32msvc/lib/SDL_mixer.dll icebreaker-$(VERSIONSTRING)
	cp *.wav icebreaker-$(VERSIONSTRING)
	cp *.bmp icebreaker-$(VERSIONSTRING)
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
	[ -f $(CROSSTOOLSPATH)/i386-mingw32msvc/lib/SDL.dll ]
	[ -d win32.build ] && rm -rf win32.build || true
	mkdir win32.build
	cp -p * win32.build || true
	(cd win32.build; make clean; make -f Makefile.w32; mv icebreaker.exe ..)
	[ -d win32.build ] && rm -rf win32.build || true

icebreaker:	$(SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o icebreaker $(SDL_LIB)

man: icebreaker.6

%.nsi: %.nsi.in icebreaker.spec icebreaker.h
	sed 's/\%ICEBREAKERVERSION/$(VERSIONSTRING)/' $< > $@

%.6: %.man.in
	sed 's/\$$VERSION/$(VERSION)/' $< | \
	   sed 's/\$$VERDATE/$(VERDATE)/' | \
	   sed 's?\$$HIGHSCOREDIR?$(highscoredir)/?' > $@

install-mkdirs:
	mkdir -p $(datadir)/icebreaker
	mkdir -p $(bindir)
	mkdir -p $(highscoredir)
	mkdir -p $(mandir)/man6

themes-install: install-themes

install-themes:
	install -m 644 *.ibt $(datadir)/icebreaker
	install -m 644 *.wav *.bmp $(datadir)/icebreaker

install-bin: icebreaker
	install -g games -m 2755 icebreaker $(bindir) || \
	  install -m 755 icebreaker $(bindir)
	@test -g $(bindir)/icebreaker || \
	  echo -e '*** Note: icebreaker binary not installed set-group-ID "games" -- shared\n*** highscores may be impossible. (Try installing as root.)'

install-man: icebreaker.6
	install -m 644 icebreaker.6 $(mandir)/man6

install-scorefile:
	touch $(highscoredir)/icebreaker.scores
	chown -f games:games $(highscoredir)/icebreaker.scores && \
	chmod 664 $(highscoredir)/icebreaker.scores || \
	chmod 644 $(highscoredir)/icebreaker.scores


install: all install-mkdirs install-themes install-bin install-man install-scorefile

%.d: %.c
	set -e; $(CC) -M $(CFLAGS) $< \
           | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
           [ -s $@ ] || rm -f $@

include $(SRC:.c=.d)
