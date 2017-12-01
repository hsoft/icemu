SUBDIRS := $(wildcard */.)

all: $(SUBDIRS)

ifdef LIBFTDI_LIBNAME
export LIBFTDI_LIBNAME
endif
$(SUBDIRS):
	$(MAKE) -C $@ clean sim

.PHONY: all $(SUBDIRS)
