// (C) 2009 Wager Labs, SA
// Author: Joel Reymont
// Web: http://tinyco.de

#include <AvailabilityMacros.h>
#include <IOKit/IOKitLib.h>
#include "SEAforth24Interface.h"

kern_return_t openUserClient(io_connect_t connect)
{
    return IOConnectCallScalarMethod(connect, kS24UserClientOpen, NULL, 0, NULL, NULL);
}

kern_return_t closeUserClient(io_connect_t connect)
{
    return IOConnectCallScalarMethod(connect, kS24UserClientClose, NULL, 0, NULL, NULL);
}


kern_return_t read(io_connect_t connect, const void *buffer, const size_t size)
{
	kern_return_t kernResult;
	
    uint64_t scalarI_64[2];
		
    scalarI_64[0] = (uint32_t)buffer;
    scalarI_64[1] = size;
    
    kernResult = IOConnectCallScalarMethod(
        connect,		// an io_connect_t returned from IOServiceOpen().
        kS24ReadMethod, // selector of the function to be called via the user client.
        scalarI_64,		// array of scalar (64-bit) input values.
        2,				// the number of scalar input values.
        NULL,			// array of scalar (64-bit) output values.
        NULL			// pointer to the number of scalar output values.
    );
    
    return kernResult;
}

kern_return_t write(io_connect_t connect, const void *buffer, const size_t size)
{
	kern_return_t kernResult;	
    uint64_t scalarI_64[2];
    
    scalarI_64[0] = (uint32_t)buffer;
    scalarI_64[1] = size;
    
    kernResult = IOConnectCallScalarMethod(
        connect,			// an io_connect_t returned from IOServiceOpen().
        kS24WriteMethod,    // selector of the function to be called via the user client.
        scalarI_64,			// array of scalar (64-bit) input values.
        2,					// the number of scalar input values.
        NULL,				// array of scalar (64-bit) output values.
        NULL				// pointer to the number of scalar output values.
    );
    
    return kernResult;
}


