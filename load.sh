#!/bin/sh

cp -R build/Debug/SEAforth24.kext /tmp
kextload -ls /tmp /tmp/SEAforth24.kext
mv /tmp/com.wagerlabs.driver.SEAforth24.sym /tmp/SEAforth24.sym
scp /tmp/SEAforth24.sym mothership.local:/tmp/
echo "Pausing, hit RETURN to continue...
read
kextload -m SEAforth24.kext
killall -HUP kextd
