FORTH

: CSTR ( -- n ) BL WORD COUNT TUCK 0 DO COUNT C, LOOP DROP 0 C, ;
: ZSTR ( -- ) CSTR DROP ;

LIBRARY /usr/lib/libc.dylib

GLOBAL: mach_task_self_

LIBRARY /System/Library/Frameworks/CoreFoundation.framework/CoreFoundation

FUNCTION: CFRelease ( ref -- )

LIBRARY /System/Library/Frameworks/IOKit.framework/IOKit

FUNCTION: IOServiceOpen ( svc port type handle-addr -- kr )
FUNCTION: IOServiceClose ( handle -- kr )
FUNCTION: IOServiceMatching ( cstr -- dict )
FUNCTION: IOServiceGetMatchingServices ( port dict iter-addr -- kr )
FUNCTION: IORegistryEntryGetParentEntry ( svc plane reg-entry-addr -- kr )
FUNCTION: IOIteratorNext ( iter -- obj ) 
FUNCTION: IOObjectRelease ( iter -- kr )
FUNCTION: IOObjectConformsTo ( obj cst -- bool )
FUNCTION: IOObjectGetClass ( obj buf -- kr )
FUNCTION: IOConnectCallScalarMethod ( port u addr u addr u -- kr )

\ Dispatch selectors

0  ENUM kS24UserClientOpen
   ENUM kS24UserClientClose
   ENUM kS24InitMethod
   ENUM kS24ReadMethod
   ENUM kS24WriteMethod
   ENUM kNumberOfMethods
DROP

0 CONSTANT kIOMasterPortDefault
mach_task_self_ @ CONSTANT OUR-MACH-TASK

CREATE DRIVER-CLASS-NAME ZSTR com_wagerlabs_driver_SEAforth24

VARIABLE ITERATOR

: LOOKUP-DRIVER ( -- svc )   
   DRIVER-CLASS-NAME IOServiceMatching ( dictRef | 0 *)
   ?DUP 0= ABORT" IOServiceMatching did not return a dictionary"
   \ consumes dictRef on success
   kIOMasterPortDefault OVER ITERATOR IOServiceGetMatchingServices
   ( dictRef kr *) ?DUP IF 
      CFRelease ABORT" IOServiceGetMatchingServices failed" 
   THEN DROP
   ITERATOR @ DUP IOIteratorNext ( iter svc *)
   ?DUP 0= ABORT" No driver found!"
   SWAP IOObjectRelease DROP ;
  
\ This call will cause the user client to be instantiated. 
\ It returns an io_connect_t handle that is used for all 
\ subsequent calls to the user client.

VARIABLE USER-CLIENT

: OPEN-USER-CLIENT ( svc -- port )
   OUR-MACH-TASK 0 USER-CLIENT IOServiceOpen
   ABORT" Could not open user client" 
   USER-CLIENT @ DUP
   \ connect to the driver
   kS24UserClientOpen 0 0 0 0 IOConnectCallScalarMethod
   ABORT" Could not connect to the driver" ;
  
: CLOSE-USER-CLIENT ( port -- )
   DUP kS24UserClientClose 0 0 0 0 IOConnectCallScalarMethod
   ABORT" Could not disconnect from the driver"
   ( port *) IOServiceClose
   ABORT" Could not close user client" ;

CREATE SCALAR 16 ALLOT  
CREATE BUFFER 256 1024 * ALLOT

SCALAR CONSTANT SCALAR0
SCALAR 2 CELLS + CONSTANT SCALAR1

: DRIVER-INIT ( port -- )
   kS24InitMethod 0 0 0 0 IOConnectCallScalarMethod
   ABORT" Driver init failed" ;

: DRIVER-READ ( port size -- )
   0 BUFFER SCALAR0 2!  \ scalarI_64[0] = (uint32_t)buffer;
   0 SWAP SCALAR1 2!    \ scalarI_64[1] = size;
   kS24ReadMethod SCALAR0 2 0 0 IOConnectCallScalarMethod
   ABORT" Driver read failed" ;
   
: DRIVER-WRITE ( port size -- )
   0 BUFFER SCALAR0 2!  \ scalarI_64[0] = (uint32_t)buffer;
   0 SWAP SCALAR1 2!    \ scalarI_64[1] = size;
   kS24WriteMethod SCALAR0 2 0 0 IOConnectCallScalarMethod
   ABORT" Driver write failed" ;
   


    
