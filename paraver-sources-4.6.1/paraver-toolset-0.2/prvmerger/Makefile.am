bin_PROGRAMS = prvmerger.bin 

prvmerger_bin_SOURCES = \
 src/prvmerger.c 

prvmerger_bin_LDADD = -lm

AM_CFLAGS = -Wall -O2 

install-exec-hook:
	strip $(bindir)/prvmerger.bin
	cd $(bindir)
	ln -sf prvmerger.bin $(bindir)/prvmerger

# Erases everything which should not be uploaded to CVS
mrproper: distclean
	rm -fr Makefile.in

