#include "SEAforth24.h"
#include <IOKit/storage/IOBlockStorageDriver.h>

#ifdef DEBUG
#define DEBUG_LOG IOLog
#else
#define DEBUG_LOG(...)
#endif

#define super IOSCSIPeripheralDeviceType00
OSDefineMetaClassAndStructors(com_wagerlabs_driver_SEAforth24, IOSCSIPeripheralDeviceType00)

bool com_wagerlabs_driver_SEAforth24::InitializeDeviceSupport( void )
{
    bool result = false;

    result = super::InitializeDeviceSupport();

    if ( result == true ) 
        result = (S24Init() == kIOReturnSuccess);

    return result;
}

IOReturn com_wagerlabs_driver_SEAforth24::S24Init(void)
{
    IOReturn err;

    SCSITaskIdentifier req = GetSCSITask();
    SetCommandDescriptorBlock(req, 0x20, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    SetTimeoutDuration(req, 10000);
    SetDataTransferDirection(req, kSCSIDataTransfer_NoDataTransfer);
	SCSIServiceResponse serviceResponse = SendCommand(req, 10000);

    SCSITaskStatus taskStatus = GetTaskStatus( req );
	
	DEBUG_LOG("%s[%p]::%s(): service response: %lu, task status: %lu\n", 
	    getName(), this, __FUNCTION__, 
	    (serviceResponse == kSCSIServiceResponse_TASK_COMPLETE), 
	    (taskStatus == kSCSITaskStatus_GOOD));

	if ((serviceResponse == kSCSIServiceResponse_TASK_COMPLETE) 
	    && taskStatus == kSCSITaskStatus_GOOD)
	    err = kIOReturnSuccess;
	else
        err = kIOReturnError;
	
	ReleaseSCSITask(req);
    req = NULL;

	return err;  
}

// Padding for future binary compatibility.
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 0);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 1);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 2);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 3);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 4);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 5);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 6);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 7);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 8);
OSMetaClassDefineReservedUnused(com_wagerlabs_driver_SEAforth24, 9);
