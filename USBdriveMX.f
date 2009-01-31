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

mach_task_self_ @ CONSTANT OUR-MACH-TASK

CREATE DRIVER-CLASS-NAME ZSTR com_wagerlabs_driver_SEAforth24

\ Registry plane names
CREATE kIOServicePlane ZSTR IOService

VARIABLE ITERATOR
VARIABLE USER-CLIENT

: RELEASE-OBJECT ( -- )
   DUP @ IOObjectRelease DROP 0 SWAP ! ;
   
: RELEASE-ITERATOR ( -- )
   ITERATOR RELEASE-OBJECT ;

: LOOKUP-DRIVER ( -- svc )   
   DRIVER-CLASS-NAME IOServiceMatching ( dictRef | 0 *)
   ?DUP 0= ABORT" IOServiceMatching did not return a dictionary"
   \ consumes dictRef on success
   kIOMasterPortDefault OVER ITERATOR IOServiceGetMatchingServices
   ( dictRef kr *) ?DUP IF 
      CFRelease ABORT" IOServiceGetMatchingServices failed" 
   THEN DROP
   ITERATOR @ IOIteratorNext ( svc *)
   ?DUP 0= ABORT" No driver found!"
   RELEASE-ITERATOR ;
  
\ This call will cause the user client to be instantiated. It returns 
\ an io_connect_t handle that is used for all subsequent calls 
\ to the user client.

: OPEN-USER-CLIENT ( svc -- handle )
   OUR-MACH-TASK 0 USER-CLIENT IOServiceOpen
   ABORT" Could not open user client" 
   USER-CLIENT @ ;
   
: CLOSE-USER-CLIENT ( -- )
   USER-CLIENT @ IOServiceClose
   ABORT" Could not close user client" 
   0 USER-CLIENT ! ;
   
\\
    
