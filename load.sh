#!/bin/sh

sudo cp -R build/Debug/SEAforth24.kext /tmp
sudo kextload /tmp/SEAforth24.kext
sudo kill -1 `ps -ax|grep kextd|head -1|awk '{print $1'}`
