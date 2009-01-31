FORTH

: CSTR ( -- n ) BL WORD COUNT TUCK 0 DO COUNT C, LOOP DROP 0 C, ;
: ZSTR ( -- ) CSTR DROP ;

LIBRARY /usr/lib/libc.dylib

GLOBAL: mach_task_self_

LIBRARY /System/Library/Frameworks/CoreFoundation.framework/CoreFoundation

FUNCTION: CFRelease ( ref -- )

LIBRARY /System/Library/Frameworks/IOKit.framework/IOKit

FUNCTION: IOServiceOpen ( svc task_port type handle-addr -- kr )
FUNCTION: IOServiceClose ( handle -- kr )
FUNCTION: IOServiceMatching ( cstr -- dict )
FUNCTION: IOServiceGetMatchingServices ( port dict iter-addr -- kr )
FUNCTION: IORegistryEntryGetParentEntry ( svc plane reg-entry-addr -- kr )
FUNCTION: IOIteratorNext ( iter -- obj ) 
FUNCTION: IOObjectRelease ( iter -- kr )
FUNCTION: IOObjectConformsTo ( obj cst -- bool )
FUNCTION: IOObjectGetClass ( obj buf -- kr )

\ Dispatch selectors

0  ENUM kS24UserClientOpen
   ENUM kS24UserClientClose
   ENUM kS24ReadMethod
   ENUM kS24WriteMethod
   ENUM kNumberOfMethods
DROP

0 CONSTANT kIOMasterPortDefault
0 CONSTANT KERN_SUCCESS

\ CREATE DRIVER-CLASS-NAME ZSTR com_wagerlabs_driver_SEAforth24
CREATE DRIVER-CLASS-NAME ZSTR IOSCSIPeripheralDeviceType00

CREATE BLOCK-STORAGE-SVC ZSTR IOBlockStorageServices 
\ Registry plane names
CREATE kIOServicePlane ZSTR IOService

VARIABLE ITERATOR
VARIABLE DRIVER
VARIABLE USER-CLIENT

: RELEASE-OBJECT ( -- )
   DUP @ IOObjectRelease DROP 0 SWAP ! ;
   
: RELEASE-ITERATOR ( -- )
   ITERATOR RELEASE-OBJECT ;

: RELEASE-DRIVER ( -- )
   DRIVER RELEASE-OBJECT ;
   
: LOOKUP-DRIVER ( -- )   
   \ IOBlockStorageServices is our child in the I/O registry
   BLOCK-STORAGE-SVC IOServiceMatching ( dictRef | 0 *)
   ?DUP 0= ABORT" IOServiceMatching did not return a dictionary"
   \ create an iterator over all matching IOService nubs
   \ and consume dictRef on success
   kIOMasterPortDefault OVER ITERATOR IOServiceGetMatchingServices
   ( dictRef kr *) ?DUP IF 
      CFRelease ABORT" IOServiceGetMatchingServices failed" 
   THEN DROP
   \ Iterate over all instances of block storage services to find our driver
   BEGIN
      ITERATOR @ IOIteratorNext ( svc *)
      ?DUP 0= ABORT" No block storage service found!"
      ( svc *) DUP kIOServicePlane DRIVER IORegistryEntryGetParentEntry
      ( svc kr *) SWAP IOOBjectRelease DROP
      ABORT" IORegistryEntryGetParentEntry failed"
      DRIVER @ DRIVER-CLASS-NAME IOOBjectConformsTo
   UNTIL
   RELEASE-ITERATOR ;
  
\ This call will cause the user client to be instantiated. It returns 
\ an io_connect_t handle that is used for all subsequent calls 
\ to the user client.

: OPEN-USER-CLIENT ( -- )
   DRIVER-OBJECT @ mach_task_self_ 0 USER-CLIENT IOServiceOpen
   ABORT" Could open user client"
   ;
   
: CLOSE-USER-CLIENT ( -- )
   USER-CLIENT @ IOServiceClose
   ABORT" Could not close user client" 
   0 USER-CLIENT ! ;
   
\\
    
