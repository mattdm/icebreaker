#!/bin/sh
export CFLAGS="-s"
export CXXFLAGS="-s"
cross-configure --disable-music-midi --disable-music-timidity-midi --disable-music-native-midi --disable-music-native-midi-gpl --disable-music-mod --disable-music-mp3 --disable-music-ogg
cross-make
find . -name '*.dll' -ls
echo now do \"cross-make install\".
