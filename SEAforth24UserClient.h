#include <IOKit/IOUserClient.h>
#include "SEAforth24.h"

#define UserClientClassName com_wagerlabs_driver_SEAforth24UserClient

class UserClientClassName : public IOUserClient
{
    OSDeclareDefaultStructors(com_wagerlabs_driver_SEAforth24UserClient)
    
protected:

    DriverClassName* fProvider;
    task_t fTask;
    
    static const IOExternalMethodDispatch Methods[kNumberOfMethods];
    
public:

    virtual void stop(IOService* provider);
    virtual bool start(IOService* provider);
    
    virtual bool initWithTask(task_t owningTask, void* securityToken, UInt32 type);
    
    virtual IOReturn clientClose(void);
    virtual IOReturn clientDied(void);
    
    virtual bool willTerminate(IOService* provider, IOOptionBits options);
    virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer);
    
    virtual bool terminate(IOOptionBits options = 0);
    virtual bool finalize(IOOptionBits options);
    
protected:	
    
    virtual IOReturn closeUserClient(void);
    virtual IOReturn openUserClient(void);
    
    // KPI for supporting access from both 32-bit and 64-bit user processes beginning with Mac OS X 10.5.
    virtual IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
                                    IOExternalMethodDispatch* dispatch, OSObject* target, void* reference);
    
    static IOReturn sOpenUserClient(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments);    
    static IOReturn sCloseUserClient(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
    static IOReturn sInit(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments);

    static IOReturn sRead(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
    static IOReturn sWrite(UserClientClassName* target, void* reference, IOExternalMethodArguments* arguments);    

    virtual IOReturn S24IO(vm_address_t buffer, UInt32 size, UInt16 bits, IODirection direction);
  };