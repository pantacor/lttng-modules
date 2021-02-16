# SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)

ifneq ($(KERNELRELEASE),)

# This part of the Makefile is used when called by the kernel build system
# and defines the modules to be built.

obj-$(CONFIG_LTTNG) += src/

else # KERNELRELEASE

# This part of the Makefile is used when the 'make' command is runned in the
# base directory of the lttng-modules sources. It sets some environment and
# calls the kernel build system to build the actual modules.

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CFLAGS = $(EXTCFLAGS)

default: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m modules_install

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src clean

%.i: %.c
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src CONFIG_LTTNG=m CONFIG_LTTNG_CLOCK_PLUGIN_TEST=m $@

endif # KERNELRELEASE
