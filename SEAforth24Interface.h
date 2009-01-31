#include <stdint.h>
#include <stdlib.h>
#include "UserKernelShared.h"

kern_return_t openUserClient(io_connect_t connect);
kern_return_t closeUserClient(io_connect_t connect);

kern_return_t read(io_connect_t connect, const void *buffer, const size_t size);
kern_return_t write(io_connect_t connect, const void *buffer, const size_t size);
