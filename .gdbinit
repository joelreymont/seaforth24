file /Volumes/KernelDebugKit/mach_kernel
add-symbol-file /tmp/SEAforth24.sym

break 'com_wagerlabs_driver_SEAforth24UserClient::S24IO'
break 'com_wagerlabs_driver_SEAforth24::S24SyncIO'

target remote-kdp
kdp-reattach localhost

