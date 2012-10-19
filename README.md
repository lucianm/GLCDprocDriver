GLCDProcDriver
==============

GraphLCD adapter layer for the LCDproc "glcdlib" driver

Introduction
============

GLCDprocDriver is the glue between the graphlcd-base library (from the GraphLCD project
at http://projects.vdr-developer.org/projects/show/graphlcd), which is written in C++,
and LCDproc's (http://www.lcdproc.org) meta-driver "glcdlib", which extends supported
displays in LCDproc by those supported through graphlcd-base (but just in text mode, of course).

It is implemented in C++, but provides the necessary wrapper C calls for use by glcdlib.

It is NOT intended to provide general C wrapping for the whole graphlcd-base,
library, as it really only implements the actual meta-driver for LCDproc.

Installation
============
Refer to the INSTALL file in the archive.



Lucian Muresan

http://projects.vdr-developer.org/projects/graphlcd-base/repository/show?rev=touchcol



