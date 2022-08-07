#pragma once
#include <chrono>
#include <windows.h>
#include <Winsvc.h>
#include <string>
#include "ServiceConstants.h"
#include "ServiceHandle.h"

using namespace std::literals::chrono_literals;

class ServiceManager {
public:
   ServiceManager(std::string name, DWORD access = SERVICE_ALL_ACCESS);  
   ServiceStatus GetStatus();
   bool Start();
   bool Stop();
   bool Pause();
   bool Continue();
   bool Delete();
   static bool ChangeServiceStatus(SC_HANDLE const handle, DWORD const controlCode, SERVICE_STATUS_PROCESS& ssp);
   bool WaitForStatus(ServiceStatus desiredStatus, std::chrono::milliseconds const timeout = 30000ms);
   bool GetSrvHandle() const;
private:
   bool StopDependentServices();
   static std::chrono::milliseconds GetWaitTime(DWORD const waitHint);
   static bool WaitForStatus(SC_HANDLE const handle, SERVICE_STATUS_PROCESS& ssp,ServiceStatus const desireStatus, 
                             std::chrono::milliseconds const timeout = 30000ms);
   ServiceHandle scHandle;
   ServiceHandle srvHandle;
   std::string srvName;
};