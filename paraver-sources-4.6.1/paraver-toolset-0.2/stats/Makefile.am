bin_PROGRAMS = stats.bin 

stats_bin_SOURCES = \
 src/stats.c \
 src/stats.h


stats_bin_LDADD = -lm

AM_CFLAGS = -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 

install-exec-hook:
	strip $(bindir)/stats.bin
	cd $(bindir)
	ln -sf stats.bin $(bindir)/stats

# Erases everything which should not be uploaded to CVS
mrproper: distclean
	rm -fr Makefile.in

