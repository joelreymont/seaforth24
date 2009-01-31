#!/bin/sh

sudo cp -R build/Debug/SEAforth24.kext /tmp
sudo kextload -n -k /Volumes/KernelDebugKit/mach_kernel -s /tmp/syms/ /tmp/SEAforth24.kext
