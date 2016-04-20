dist-hook:
	mkdir -p $(distdir)/src
	cp -f $(srcdir)/src/*.sh $(distdir)/src

install-exec-hook:
	cp -f ./src/sort-trace.grep.sh $(bindir) 
	cp -f ./src/sort-trace.awk.sh $(bindir) 
	cp -f ./src/is-trace-sorted.grep.sh $(bindir) 
	cp -f ./src/is-trace-sorted.awk.sh $(bindir) 
	cd $(bindir)
	ln -sf sort-trace.grep.sh      $(bindir)/sort-trace.sh
	ln -sf is-trace-sorted.grep.sh $(bindir)/is-trace-sorted.sh

# Erases everything which should not be uploaded to CVS
mrproper: distclean
	rm -fr Makefile.in

