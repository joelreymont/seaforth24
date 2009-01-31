#!/bin/sh

sudo cp -R build/Debug/SEAforth24.kext /tmp
sudo kextload /tmp/SEAforth24.kext
sudo killall -HUP kextd
