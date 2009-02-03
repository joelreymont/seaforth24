HOST

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
\ FUNCTION: IORegistryEntryGetParentEntry ( svc plane reg-entry-addr -- kr )
FUNCTION: IOIteratorNext ( iter -- obj ) 
FUNCTION: IOObjectRelease ( iter -- kr )
\ FUNCTION: IOObjectConformsTo ( obj cst -- bool )
\ FUNCTION: IOObjectGetClass ( obj buf -- kr )
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

VARIABLE DRIVER-PORT

: OPEN-DRIVER-PORT ( svc -- )
   OUR-MACH-TASK 0 DRIVER-PORT IOServiceOpen
   ABORT" Could not open driver port" 
   \ connect to the driver
   DRIVER-PORT @ kS24UserClientOpen 0 0 0 0 IOConnectCallScalarMethod
   ABORT" Could not connect to the driver" ;
  
: CLOSE-DRIVER-PORT ( port -- )
   DUP kS24UserClientClose 0 0 0 0 IOConnectCallScalarMethod
   ABORT" Could not disconnect from the driver"
   ( port *) IOServiceClose
   ABORT" Could not close driver port" ;

CREATE SCALAR 8 3 * ALLOT  
SCALAR CONSTANT SCALAR0
SCALAR0 2 CELLS + CONSTANT SCALAR1
SCALAR1 2 CELLS + CONSTANT SCALAR2

CREATE SPT-DBUF 256 1024 * ALLOT
VARIABLE SPT-DataTransferLength

: DRIVER-INIT ( port -- )
   kS24InitMethod 0 0 0 0 IOConnectCallScalarMethod
   ABORT" Driver init failed" ;

: DRIVER-READ ( port size bits -- )
   0 SPT-DBUF SCALAR0 2!   \ scalarI_64[0] = (uint32_t)buffer;
   0 SWAP SCALAR1 2!       \ scalarI_64[1] = size;
   kS24ReadMethod SCALAR0 2 0 0 IOConnectCallScalarMethod
   ABORT" Driver read failed" ;
   
: DRIVER-WRITE ( port size bits -- )
   0 SPT-DBUF SCALAR0 2!   \ scalarI_64[0] = (uint32_t)buffer;
   0 SWAP SCALAR1 2!       \ scalarI_64[1] = size;
   0 SWAP SCALAR2 2!       \ scalarI_64[2] = bits;    
   kS24WriteMethod SCALAR0 2 0 0 IOConnectCallScalarMethod
   ABORT" Driver write failed" ;
   
{ --------------------------------------------------------------------
16>OnStream appends a 16-bit word to the stream in the data buffer.
-------------------------------------------------------------------- }

: 16>OnStream ( x -- )   
   SPT-DBUF SPT-DataTransferLength @ + W!
   2 SPT-DataTransferLength +! ;

{ --------------------------------------------------------------------
SEAforth [x] buffer transfers

/USBdrive initializes the drive (which must already be open!).
[x]>USBdrive sends the [x] buffer to the drive.
USBdrive>[x] reads from the drive, compiling into the [x] memory.
-------------------------------------------------------------------- }

HOST

: FIND-DRIVE ( -- )
   LOOKUP-DRIVER
   OPEN-DRIVER-PORT ;
   
: CLOSE-DRIVE ( -- )
   DRIVER-PORT @ CLOSE-DRIVER-PORT ;
   
: /USBdrive ( -- )
   DRIVER-PORT @ DRIVER-INIT ;

: [x]>USBdrive ( -- )
   0 SPT-DataTransferLength !
   0 18 BEGIN @16<18 16>OnStream OVER HERE = UNTIL
   2 = IF -2 SPT-DataTransferLength +! THEN 18 * ( bits *)
   DRIVER-PORT @ SPT-DataTransferLength @ ROT DRIVER-WRITE ;
   
: USBdrive>[x] ( addr u -- )
   2DUP SCRUB  SWAP ORG
   DUP 18 * DUP ( bits *)
   14 + 16 / 2*  ( size *)
   DRIVER-PORT @ SWAP ROT DRIVER-READ
   SPT-DBUF 16 ROT 0 DO
      @18<16  TARGET ,  HOST
   LOOP 2DROP ;

ONLY FORTH DEFINITIONS


    
