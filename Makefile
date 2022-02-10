#
# Makefile for the glcdprocdriver graphlcd-base wrapper "meta-driver"
#

-include Make.config


MODULE = $(shell grep '\#define GLCDPROC_MODULENAME' glcddriver.h | cut -d' ' -f3)
VERMAJOR = $(shell grep '\#define GLCDPROC_VER_MAJOR' glcddriver.h | cut -d' ' -f3)
VERMINOR = $(shell grep '\#define GLCDPROC_VER_MINOR' glcddriver.h | cut -d' ' -f3)
VERMICRO = $(shell grep '\#define GLCDPROC_VER_MICRO' glcddriver.h | cut -d' ' -f3)
VERSUPPL = $(shell grep '\#define GLCDPROC_VER_SUPPL' glcddriver.h | cut -d' ' -f3)
VERSION = $(VERMAJOR).$(VERMINOR).$(VERMICRO)
ARCHIVE = $(MODULE)-$(VERSION)$(VERSUPPL)
PACKAGE = $(ARCHIVE)
TMPDIR = /tmp

LIBS=-lglcddrivers -lglcdgraphics

BASENAME = libglcdprocdriver.so
LIBNAME  = $(BASENAME).$(VERSION)

OBJS = glcdprocdriver.o glcddriver.o

HEADERS = glcdprocdriver.h glcddriver.h


### Targets:

all: $(LIBNAME)

$(OBJS): %.o: %.c
	$(CXX) -c -fPIC $(CXXFLAGS) $< -o $@

$(LIBNAME): $(OBJS)
	$(CXX) $(LDFLAGS) -shared $(OBJS) $(LIBS) -Wl,-soname="$(BASENAME).$(VERMAJOR)" -o $@

install: all
	install -d $(LIBDIR)
	install -m 644 $(LIBNAME) $(LIBDIR)/
	ln -sf $(LIBNAME) $(LIBDIR)/$(BASENAME)
	ln -sf $(LIBNAME) $(LIBDIR)/$(BASENAME).$(VERMAJOR)
	install -d $(INCDIR)/glcdproclib
	install -m 644 $(HEADERS) $(INCDIR)/glcdproclib/
	@echo \**** Run \"$(LDCONFIG)\" yourself if you ran \"make install\" by hand!!

uninstall:
	rm -f $(LIBDIR)/$(BASENAME).$(VERMAJOR)
	rm -f $(LIBDIR)/$(BASENAME)
	rm -f $(LIBDIR)/$(LIBNAME)
	(for i in $(HEADERS); do rm -f $(INCDIR)/glcdproclib/$$i; done)
	rmdir $(INCDIR)/glcdproclib
	@echo \**** Run \"$(LDCONFIG)\" yourself if you ran \"make uninstall\" by hand!!

clean:
	rm -f $(OBJS) $(DEPFILE) *.so *.so.* *.a *~

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf ../$(PACKAGE).tar.gz -C $(TMPDIR) $(ARCHIVE) --exclude=.git*
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as ../$(PACKAGE).tar.gz

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)
