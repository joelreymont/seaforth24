#!/bin/sh

cp -R build/Debug/SEAforth24.kext /tmp
kextload -ls /tmp /tmp/SEAforth24.kext
mv /tmp/com.wagerlabs.driver.SEAforth24.sym /tmp/SEAforth24.sym
echo "Copying symbol file to debugging host..."
scp /tmp/SEAforth24.sym joelr@mothership.local:/tmp/
echo "Pausing, hit RETURN to continue..."
read
echo "Starting driver and re-enumerating..."
kextload -m /tmp/SEAforth24.kext
killall -HUP kextd
