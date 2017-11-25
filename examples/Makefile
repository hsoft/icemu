SUBDIRS := $(wildcard */.)

all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ clean sim

.PHONY: all $(SUBDIRS)
