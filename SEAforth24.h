// (C) 2009 Wager Labs, SA
// Author: Joel Reymont
// Web: http://tinyco.de

#include <IOKit/scsi/IOSCSIPeripheralDeviceType00.h>
#include <IOKit/scsi/IOBlockStorageServices.h>
#include "UserKernelShared.h"

class com_wagerlabs_driver_SEAforth24 : public IOSCSIPeripheralDeviceType00
{
	OSDeclareDefaultStructors(com_wagerlabs_driver_SEAforth24)

public:

    IOReturn S24Init(void);
    IOReturn S24Read(IOMemoryDescriptor *buffer, UInt16 bits);
    IOReturn S24Write(IOMemoryDescriptor *buffer, UInt16 bits);
    IOReturn S24WriteLast(IOMemoryDescriptor *buffer, UInt16 bits);
    
protected:
    
    bool start(IOService *provider);
    bool InitializeDeviceSupport (void);
    IOReturn S24SyncIO(S24Kind kind, IOMemoryDescriptor *buffer, UInt16 bits);
    
    // Reserve space for future expansion.
    struct ExpansionData { };
    ExpansionData *reserved;

private:
    
    // Padding for future binary compatibility.
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 0);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 1);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 2);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 3);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 4);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 5);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 6);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 7);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 8);
    OSMetaClassDeclareReservedUnused(com_wagerlabs_driver_SEAforth24, 9);
};


