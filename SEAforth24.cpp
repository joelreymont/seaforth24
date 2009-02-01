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

IOReturn com_wagerlabs_driver_SEAforth24::S24Read(IOMemoryDescriptor *buffer)
{
    return S24SyncIO(kSCSIDataTransfer_FromTargetToInitiator, buffer);
}

IOReturn com_wagerlabs_driver_SEAforth24::S24Write(IOMemoryDescriptor *buffer)
{
    return S24SyncIO(kSCSIDataTransfer_FromInitiatorToTarget, buffer);
}

IOReturn com_wagerlabs_driver_SEAforth24::S24Init(void)
{
    return S24SyncIO(kSCSIDataTransfer_NoDataTransfer, NULL);
}
 
IOReturn com_wagerlabs_driver_SEAforth24::S24SyncIO(UInt8 direction, IOMemoryDescriptor *buffer)
{
    IOReturn err = kIOReturnBadArgument;
    UInt8 b1, b2;
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
        kSCSIDataTransfer_FromInitiatorToTarget:
            b1 = 0xFB;
            b2 = 0x00;
            break;
        kSCSIDataTransfer_FromTargetToInitiator:
            b1 = 0xFB;
            b2 = 0x01;
            break;
        default:
            b1 = 0xFA;
            b2 = 0x00;
    }
    SetCommandDescriptorBlock(req, 0x20, b1, b2, 0x00, 0x00, 0x00, 0x00, 0x00, /*0x90*/0x00, 0x00);
    SetTimeoutDuration(req, 10000);
    SetDataTransferDirection(req, direction);
    
    if (buffer != NULL)
    {
      SetDataBuffer(req, buffer);
	    SetRequestedDataTransferCount(req, buffer->getLength());
      buffer->prepare();
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
