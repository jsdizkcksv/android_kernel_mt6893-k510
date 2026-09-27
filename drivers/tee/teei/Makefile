# Copyright (c) 2015-2019, MICROTRUST Incorporated
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

ifneq ($(CONFIG_MICROTRUST_TEE_SUPPORT),)
# AGATE(5.10): CONFIG_MICROTRUST_TEE_VERSION is empty in this defconfig,
# which made the whole subdir name empty so teei/<ver>/ (and therefore
# tee_client_api.c providing TEEC_*) was never built.  The source tree
# only ships 400/, so default to it.
MICROTRUST_TEE_VERSION := $(subst ",,$(CONFIG_MICROTRUST_TEE_VERSION))
ifeq ($(MICROTRUST_TEE_VERSION),)
MICROTRUST_TEE_VERSION := 400
endif
obj-$(CONFIG_MICROTRUST_TEE_SUPPORT) += $(MICROTRUST_TEE_VERSION)/
endif
