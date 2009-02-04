// (C) 2009 Wager Labs, SA
// Author: Joel Reymont
// Web: http://tinyco.de

#include <IOKit/IOLib.h>
#include <IOKit/IOKitKeys.h>
#include <libkern/OSByteOrder.h>
#include "SEAforth24UserClient.h"

#define super IOUserClient

// Even though we are defining the convenience macro super 
// for the superclass, you must use the actual class name
// in the OS*MetaClass macros.

OSDefineMetaClassAndStructors(com_wagerlabs_driver_SEAforth24UserClient, IOUserClient)

// This is the technique which supports both 32-bit and 
// 64-bit user processes starting with Mac OS X 10.5.
//
// User client method dispatch table.
//
// The user client mechanism is designed to allow calls 
// from a user process to be dispatched to any IOService-based 
// object in the kernel. Almost always this mechanism 
// is used to dispatch calls to either member functions 
// of the user client itself or of the user client's provider. 
// The provider is the driver which the user client is connecting 
// to the user process.
//
// It is recommended that calls be dispatched to the user client 
// and not directly to the provider driver. This allows the user 
// client to perform error checking on the parameters before 
// passing them to the driver. 

const IOExternalMethodDispatch UserClientClassName::Methods[kNumberOfMethods] = 
{
	{   // kS24ClientOpen
		(IOExternalMethodAction) &UserClientClassName::sOpenUserClient,
		0,														
		0,														
		0,														
		0														
	},
	{   // kS24ClientClose
		(IOExternalMethodAction) &UserClientClassName::sCloseUserClient,
		0,																
		0,																
		0,																
		0																
	},
	{   // kS24InitMethod
		(IOExternalMethodAction) &UserClientClassName::sInit,
		0,													
		0,													
		0,													
		0													
	},
	{   // kS24ReadMethod
		(IOExternalMethodAction) &UserClientClassName::sRead,
		3,													
		0,													
		0,													
		0													
	},
	{   // kS24WriteMethod
		(IOExternalMethodAction) &UserClientClassName::sWrite,
		4,													
		0,													
		0,													
		0													
	}
};

IOReturn UserClientClassName::externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
    IOExternalMethodDispatch* dispatch, OSObject* target, void* reference)

{
    IOLog("%s[%p]::%s(%d, %p, %p, %p, %p)\n", getName(), this, __FUNCTION__,
        selector, arguments, dispatch, target, reference);
        
    if (selector < (uint32_t) kNumberOfMethods) 
    {
        dispatch = (IOExternalMethodDispatch *) &Methods[selector];
        
        if (!target)
		    target = this;
	}

	return super::externalMethod(selector, arguments, dispatch, target, reference);
}

// initWithTask is called as a result of the user process calling IOServiceOpen.

bool UserClientClassName::initWithTask(task_t owningTask, void* securityToken, UInt32 type)
{
    bool success = super::initWithTask(owningTask, securityToken, type);	    
	
	// This IOLog must follow super::initWithTask because getName relies on the superclass initialization.
	IOLog("%s[%p]::%s(%p, %p, %ld)\n", getName(), this, __FUNCTION__, owningTask, securityToken, type);

    fTask = owningTask;
    fProvider = NULL;
        
    return success;
}

// start is called after initWithTask as a result of the user process calling IOServiceOpen.

bool UserClientClassName::start(IOService* provider)
{
    bool success;
	
	IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
    
    // Verify that this user client is being started with a provider 
    // that it knows how to communicate with.
	
	fProvider = OSDynamicCast(DriverClassName, provider);
  
    success = (fProvider != NULL);
    
    if (success) 
    {
		// It's important not to call super::start if some previous condition
		// (like an invalid provider) would cause this function to return false. 
		// I/O Kit won't call stop on an object if its start function returned false.
		success = super::start(provider);
	}
	
    return success;
}


// clientClose is called as a result of the user process calling IOServiceClose.
IOReturn UserClientClassName::clientClose(void)
{
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    // Defensive coding in case the user process called IOServiceClose
	// without calling closeUserClient first.
    (void) closeUserClient();
    
	// Inform the user process that this user client is no longer available. This will also cause the
	// user client instance to be destroyed.
	//
	// terminate would return false if the user process still had this user client open.
	// This should never happen in our case because this code path is only reached if the user process
	// explicitly requests closing the connection to the user client.
	bool success = terminate();
	if (!success) 
	{
		IOLog("%s[%p]::%s(): terminate() failed.\n", getName(), this, __FUNCTION__);
	}

    // DON'T call super::clientClose, which just returns kIOReturnUnsupported.
    
    return kIOReturnSuccess;
}

// didTerminate is called at the end of the termination process. It is a notification
// that a provider has been terminated, sent after recursing up the stack, in leaf-to-root order.
bool UserClientClassName::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
	IOLog("%s[%p]::%s(%p, %ld, %p)\n", getName(), this, __FUNCTION__, provider, options, defer);
	
	// If all pending I/O has been terminated, close our provider. If I/O is still outstanding, set defer to true
	// and the user client will not have stop called on it.
	closeUserClient();
	*defer = false;
	
	return super::didTerminate(provider, options, defer);
}

IOReturn UserClientClassName::sOpenUserClient(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->openUserClient();
}

IOReturn UserClientClassName::openUserClient(void)
{
    IOReturn result = kIOReturnSuccess;
	
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    if (fProvider == NULL || isInactive()) 
    {
		// Return an error if we don't have a provider. This could happen if the user process
		// called openUserClient without calling IOServiceOpen first. Or, the user client could be
		// in the process of being terminated and is thus inactive.
        result = kIOReturnNotAttached;
	}
    else if (!fProvider->open(this)) 
    {
		// The most common reason this open call will fail is because the provider is already open
		// and it doesn't support being opened by more than one client at a time.
		result = kIOReturnExclusiveAccess;
	}
        
    return result;
}

IOReturn UserClientClassName::sCloseUserClient(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->closeUserClient();
}

IOReturn UserClientClassName::closeUserClient(void)
{
    IOReturn result = kIOReturnSuccess;
	
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
            
    if (fProvider == NULL) 
    {
		// Return an error if we don't have a provider. This could happen if the user process
		// called closeUserClient without calling IOServiceOpen first. 
		result = kIOReturnNotAttached;
		IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
	}
	else if (fProvider->isOpen(this)) 
	{
		// Make sure we're the one who opened our provider before we tell it to close.
		fProvider->close(this);
	}
	else 
	{
		result = kIOReturnNotOpen;
		IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
	}
	
    return result;
}

IOReturn UserClientClassName::sInit(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->S24IO(NULL, 0, 0, 0, kIODirectionNone);
}

IOReturn UserClientClassName::sRead(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->S24IO(
        arguments->scalarInput[0], 
        arguments->scalarInput[1], 
        arguments->scalarInput[2],
        0, 
        kIODirectionIn
        );
}

IOReturn UserClientClassName::sWrite(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->S24IO(
        arguments->scalarInput[0], 
        arguments->scalarInput[1],
        arguments->scalarInput[2], 
        arguments->scalarInput[3], 
        kIODirectionOut
        );
}

IOReturn UserClientClassName::S24IO(vm_address_t buffer, UInt32 size, UInt16 bits, UInt8 write_last, IODirection direction)
{
	IOReturn result;

	IOLog("%s[%p]::%s(%p, %d, %d, %d)\n", getName(), this, __FUNCTION__, buffer, size, bits, direction);

	if (fProvider == NULL || isInactive()) 
	{
		// Return an error if we don't have a provider. This could happen if the user process
		// called ScalarIStructI without calling IOServiceOpen first. Or, the user client could be
		// in the process of being terminated and is thus inactive.
		result = kIOReturnNotAttached;
	}
	else if (!fProvider->isOpen(this)) 
	{
		// Return an error if we do not have the driver open. This could happen if the user process
		// did not call openUserClient before calling this function.
		result = kIOReturnNotOpen;
	}
	// We do not support In/Out
	else if (direction == kIODirectionNone || direction == kIODirectionInOut)
	{
        result = fProvider->S24Init();
	}
	else 
	{
        IOMemoryDescriptor *iomd = IOMemoryDescriptor::withAddress(
            (vm_address_t)buffer, 
			size, 
			direction,
			fTask 
		);

        if (iomd == NULL)
        {
            result = kIOReturnError;
        }
        else
        {
            if (direction == kIODirectionIn)
		        result = fProvider->S24Read(iomd, bits);
		    else if (direction == kIODirectionOut && write_last == 0)
                result = fProvider->S24Write(iomd, bits);
            else
                result = fProvider->S24WriteLast(iomd, bits);
                
            iomd->release();
        }
	}
	
	return result;
}
