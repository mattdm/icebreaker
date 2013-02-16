#!/bin/sh
export CFLAGS="-s"
export CXXFLAGS="-s"
cross-configure --disable-debug --disable-joystick --disable-cdrom  --disable-oss --disable-alsa --disable--openbsdaudio --disable-esd --disable-arts --disable-nas  --disable-diskaudio --disable-video-x11 --without-x --disable-video-fbcon  --disable-video-directfb --disable-video-ggi --disable-video-svga
cross-make
find . -name '*.dll' -ls
echo now do \"cross-make install\".
