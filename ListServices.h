#pragma once
#include <vector>
#include <string>
#include "ServiceStatusProcess.h"
#include "ServiceConstants.h"

std::vector<ServiceStatusProcess> listServices( ServiceType const type = ServiceType::All,
                                                ServiceState const state = ServiceState::All,
                                                std::string const* machine = nullptr,
                                                std::string const * dbname = nullptr,
                                                std::string const * groupName = nullptr);