#!/bin/sh
arm-none-eabi-strip -K _module_cfg_tab -K _module_cfg_entry_num -K _module_pil_version app
