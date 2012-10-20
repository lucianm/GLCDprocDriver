GLCDprocDriver
==============

GraphLCD adapter layer for the LCDproc "glcdlib" driver

Homepage: http://lucianm.github.com/GLCDprocDriver/


Introduction
------------

GLCDprocDriver is the glue between the graphlcd-base library (from the GraphLCD project
at http://projects.vdr-developer.org/projects/show/graphlcd), which is written in C++,
and LCDproc's (http://www.lcdproc.org) meta-driver "glcdlib", which extends supported
displays in LCDproc by those supported through graphlcd-base (but just in text mode, of course).

It is implemented in C++, but provides the necessary wrapper C calls for use by glcdlib.

It is NOT intended to provide general C wrapping for the whole graphlcd-base,
library, as it really only implements the actual meta-driver for LCDproc.

The image above shows a 21x5 lcdproc client rendered on the graphlcd-base's "image" driver
(FYI, this one creates PBM images in /tmp), which was configured at 256x128 pixel in /etc/graphlcd.conf.


Download
--------

https://github.com/lucianm/GLCDprocDriver/downloads

Please mind the version descriptions with regard to graphlcd-base compatibility.

If you really want to try out GLCDprocDriver >= 0.1.0 you have to use
graphlcd-base greater than 0.1.9 or the "touchcol" branch from their Git repository:

http://projects.vdr-developer.org/projects/graphlcd-base/repository/show?rev=touchcol


Gentoo ebuilds, unless already available via portage:
-----------------------------------------------------

Add the layman overlay located at

https://github.com/lucianm/gen2ovl-googoo2


Installation
------------

Please refer to the INSTALL file in the source archive or here:

https://github.com/lucianm/GLCDprocDriver/raw/master/INSTALL

History
-------

Please consult the ChangeLog file in the source archive or here:

https://github.com/lucianm/GLCDprocDriver/raw/master/ChangeLog




&copy; 2005 - 2012 Lucian Muresan

Last updated: 2012.10.20
