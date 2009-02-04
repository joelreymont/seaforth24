#!/bin/sh

cp -R build/Debug/SEAforth24.kext /tmp
kextload /tmp/SEAforth24.kext
killall -HUP kextd

