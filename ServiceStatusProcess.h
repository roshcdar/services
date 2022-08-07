#pragma once
#include <string>
#include "ServiceConstants.h"

struct ServiceStatusProcess
{
   std::string           ServiceName;
   std::string           DisplayName;
   SERVICE_STATUS_PROCESS  Status;
};
