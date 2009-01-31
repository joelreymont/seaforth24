file /Volumes/KernelDebugKit/mach_kernel
add-symbol-file /tmp/syms/com.wagerlabs.driver.SEAforth24.sym

break 'com_wagerlabs_driver_SEAforth24::S24Read(IOMemoryDescriptor*)'
break 'com_wagerlabs_driver_SEAforth24::S24Write(IOMemoryDescriptor*)'

target remote-kdp
kdp-reattach localhost

