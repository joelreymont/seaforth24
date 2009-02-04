// (C) 2009 Wager Labs, SA
// Author: Joel Reymont
// Web: http://tinyco.de

#include <AssertMacros.h>
#include <IOKit/storage/IOBlockStorageDriver.h>
#include "SEAforth24.h"

#ifdef DEBUG
#define DEBUG_LOG IOLog
#else
#define DEBUG_LOG(...)
#endif

#define super IOSCSIPeripheralDeviceType00
OSDefineMetaClassAndStructors(com_wagerlabs_driver_SEAforth24, IOSCSIPeripheralDeviceType00)

bool com_wagerlabs_driver_SEAforth24::start(IOService *provider)
{
    bool  result = false;

    require ( super::start ( provider ), ErrorExit );
    result = true;

    registerService();
  
ErrorExit:
    
    return result;

}

bool com_wagerlabs_driver_SEAforth24::InitializeDeviceSupport(void)
{
    bool result = false;

    result = super::InitializeDeviceSupport();

    if ( result == true ) 
        result = (S24Init() == kIOReturnSuccess);

    return result;
}

IOReturn com_wagerlabs_driver_SEAforth24::S24Read(IOMemoryDescriptor *buffer, UInt16 bits)
{
    return S24SyncIO(kS24Read, buffer, bits);
}

IOReturn com_wagerlabs_driver_SEAforth24::S24Write(IOMemoryDescriptor *buffer, UInt16 bits)
{
    return S24SyncIO(kS24Write, buffer, bits);
}

IOReturn com_wagerlabs_driver_SEAforth24::S24WriteLast(IOMemoryDescriptor *buffer, UInt16 bits)
{
    return S24SyncIO(kS24WriteLast, buffer, bits);
}

IOReturn com_wagerlabs_driver_SEAforth24::S24Init(void)
{
    return S24SyncIO(kS24Init, NULL, 0);
}
 
IOReturn com_wagerlabs_driver_SEAforth24::S24SyncIO(UInt8 kind, IOMemoryDescriptor *buffer, UInt16 bits)
{
    IOReturn err = kIOReturnBadArgument;
    UInt8 direction, b1, b2, hi = bits >> 8, lo = bits & 0xff;
    UInt64 count = 0;
    SCSITaskIdentifier req = NULL;
    SCSITaskStatus taskStatus = kSCSITaskStatus_No_Status;
    SCSIServiceResponse serviceResponse = kSCSIServiceResponse_SERVICE_DELIVERY_OR_TARGET_FAILURE;
    
    DEBUG_LOG("%s[%p]::%s(%d, %p)\n", getName(), this, __FUNCTION__, direction, buffer);

    if (direction != kSCSIDataTransfer_NoDataTransfer)
    {
        require(buffer != NULL, ErrorExit);
    }
    
    req = GetSCSITask();
    
    require(req != NULL, ErrorExit);

    switch (direction)
    {
        case kS24Write:
            direction = kSCSIDataTransfer_FromInitiatorToTarget;
            b1 = 0xFB;
            b2 = 0x00;
            break;
        case kS24WriteLast:
            direction = kSCSIDataTransfer_FromInitiatorToTarget;
            b1 = 0xFB;
            b2 = 0x02;
            break;
        case kS24Read:
            direction = kSCSIDataTransfer_FromTargetToInitiator;
            b1 = 0xFB;
            b2 = 0x01;
            break;
        default: // kS24Init
            direction = kSCSIDataTransfer_NoDataTransfer;
            b1 = 0xFA;
            b2 = 0x00;
    }
    
    SetCommandDescriptorBlock(req, 0x20, b1, b2, 0x00, 0x00, 0x00, 0x00, hi, lo, 0x00);
    SetTimeoutDuration(req, 10000);
    SetDataTransferDirection(req, direction);
    
    if (buffer != NULL)
    {   
        buffer->prepare();
        SetDataBuffer(req, buffer);
        SetRequestedDataTransferCount(req, buffer->getLength());
    }
	
    serviceResponse = SendCommand(req, 10000);
  
    if (buffer != NULL)
    {
        buffer->complete();
    }
  
    taskStatus = GetTaskStatus(req);
    count = GetRealizedDataTransferCount(req);
    
	DEBUG_LOG("%s[%p]::%s(): service response: %lu, task status: %lu, transfer count: %lu\n", 
	    getName(), this, __FUNCTION__, serviceResponse, taskStatus, count);

	if ((serviceResponse == kSCSIServiceResponse_TASK_COMPLETE) 
        && taskStatus == kSCSITaskStatus_GOOD)
        err = kIOReturnSuccess;
	else
        err = kIOReturnError;

	ReleaseSCSITask(req);
    req = NULL;

ErrorExit:

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
