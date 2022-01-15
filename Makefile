.POSIX:

SUBDIRS = src

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

install uninstall clean:
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

.PHONY: all clean install $(SUBDIRS)
