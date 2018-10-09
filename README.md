Build Instructions
==================
qmake && make

Apps are styled using stylesheets which are found in the source directory
of the app. To apply the style execute the application in the following manner:
```
./audioscrobbler -stylesheet path/to/audioscrobbler.css
```
(by default the executable path is checked for the css file on Windows and on OSX 
 the bundle Resource directory is checked)

If building didn't work it's because:-


Build Dependencies
==================
* Qt >= 4.8 (built with Phonon if you want sound :P)
* Ruby
* Perl
* liblastfm
* http://lloyd.github.com/yajl/

Qt is readily available as a simple binary installer on all platforms.

You will also need depending on your chosen platform:-

Linux
-----
* pkg-config
* taglib
* libsamplerate
* fftw3
* libmad

Possibly you'll have to play around to get a phonon that works with KDE4 and 
Qt. I can't help you there I'm afraid. An example Ubuntu installation would 
probably go:

	sudo apt-get install qt4-dev pkg-config libtag libsamplerate libfftw3 libmad
	./configure && make -j2 && make install

I wrote that from memory as I'm working on a Mac today. If it's wrong please
write and correct me. KTHXBAI.

Ubuntu install deps line:
```
sudo apt-get install libsqlite3-dev libqt4-sql-sqlite
```
Mac OS X
--------
We suggest you install Qt 4.8 from the Qt SDK from the Qt website. Then, the
easiest way to install the additional dependencies is with MacPorts:-

	sudo port install taglib libsamplerate fftw-3 libmad

Windows
-------
We only build via cygwin. You will also need the latest Windows SDK. We build using Visual Studio 2008. Good luck with building on Windows. It's hard!


Build Support
=============
We support developers trying to build the source on any platform. 

Seeing as we don't provide many varieties of binary package on Linux, we also
support non-developers building from source there. However within reason!
Please check around the net a little first. Ask your friends. Demand help
from people standing at bus-stops.

Maybe check the official forum: http://www.last.fm/forum/34905


Bugs
====
If you find a bug in the software, please let us know about it.

Michael Coffey<michaelc@last.fm>
Desktop App Lead Developer, Last.fm
