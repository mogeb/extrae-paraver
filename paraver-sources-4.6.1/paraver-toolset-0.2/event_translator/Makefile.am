bin_PROGRAMS = event_translator.bin 

event_translator_bin_SOURCES = \
 call_renaming.c \
 call_renaming.h \
 hashtab.c \
 hashtab.h


event_translator_bin_LDADD = -lm

AM_CFLAGS = -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 

install-exec-hook:
	strip $(bindir)/event_translator.bin
	cd $(bindir)
	ln -sf event_translator.bin $(bindir)/event_translator.old

# Erases everything which should not be uploaded to CVS
mrproper: distclean
	rm -fr Makefile.in

