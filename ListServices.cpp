#include "ListServices.h"
#include "ServiceHandle.h"

std::vector<ServiceStatusProcess> listServices(  
                                                      ServiceType const type, 
                                                      ServiceState const state, 
                                                      std::string const * machine,
                                                      std::string const * dbname,
                                                      std::string const * groupName) {
      std::vector<ServiceStatusProcess> ssps;

      auto scHandle = ServiceHandle
      { 
         ::OpenSCManager(
            machine == nullptr ? nullptr : machine->c_str(), 
            dbname == nullptr ? nullptr : dbname->c_str(), 
            SC_MANAGER_ENUMERATE_SERVICE) 
      };
      auto bytesNeeded = DWORD{ 0 };
      auto servicesReturnedCount = DWORD{ 0 };
      auto resumeHandle = DWORD{ 0 };

      do
      {
         if (!EnumServicesStatusEx(
            scHandle,
            SC_ENUM_PROCESS_INFO,
            static_cast<DWORD>(type),
            static_cast<DWORD>(state),
            nullptr,
            0,
            &bytesNeeded,
            &servicesReturnedCount,
            &resumeHandle,
            groupName == nullptr ? nullptr : groupName->c_str()))
         {
            if (ERROR_MORE_DATA == ::GetLastError())
            {
               std::vector<unsigned char> buffer(bytesNeeded, 0);

               if (EnumServicesStatusEx(
                  scHandle,
                  SC_ENUM_PROCESS_INFO,
                  static_cast<DWORD>(type),
                  static_cast<DWORD>(state),
                  reinterpret_cast<LPBYTE>(buffer.data()),
                  bytesNeeded,
                  &bytesNeeded,
                  &servicesReturnedCount,
                  nullptr,
                  groupName == nullptr ? nullptr : groupName->c_str()))
               {
                  auto essp = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(buffer.data());

                  for (auto i = DWORD{ 0 }; i < servicesReturnedCount; ++i)
                  {
                     auto ssp = ServiceStatusProcess{};
                     ssp.ServiceName = essp[i].lpServiceName;
                     ssp.DisplayName = essp[i].lpDisplayName;
                     ssp.Status = essp[i].ServiceStatusProcess;

                     ssps.push_back(ssp);
                  }
               }
               else break;
            }
            else break;
         }
      } while (resumeHandle != 0);

      return ssps;
   }