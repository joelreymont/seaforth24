// (C) 2009 Wager Labs, SA
// Author: Joel Reymont
// Web: http://tinyco.de

#define DriverClassName   com_wagerlabs_driver_SEAforth24
#define kDriverClassName  "com_wagerlabs_driver_SEAforth24"

// User client method dispatch selectors.
enum 
{
    kS24UserClientOpen,
    kS24UserClientClose,
    kS24InitMethod,
    kS24ReadMethod,
    kS24WriteMethod,
    kNumberOfMethods // Must be last 
};

