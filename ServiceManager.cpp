#include "ServiceManager.h"
#include <vector>
#include <thread>

ServiceManager::ServiceManager(std::string name, DWORD access) {
      srvName = name;
      scHandle = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
      if (scHandle)
      {
         srvHandle = ::OpenService(scHandle, name.c_str(), access);
         if (!srvHandle)
            scHandle = nullptr;
      }
 }

ServiceStatus ServiceManager::GetStatus() {
  auto status = ServiceStatus::Unknown;
  if (srvHandle) {
     auto bytesNeeded = DWORD{ 0 };
     auto ssp = SERVICE_STATUS_PROCESS {0};
     auto result = ::QueryServiceStatusEx(
        srvHandle,
        SC_STATUS_PROCESS_INFO,
        reinterpret_cast<LPBYTE>(&ssp),
        sizeof(ssp),
        &bytesNeeded);

     if (result != 0)
     {
         status = static_cast<ServiceStatus>(ssp.dwCurrentState);
     }
  }
  return status;
}

bool ServiceManager::Start() {
  bool success = false;
  if (srvHandle) 
     success = ::StartService(srvHandle, 0, nullptr) != 0;
  return success;
}

bool ServiceManager::Stop() {
  bool success = false;
  if (srvHandle) {
     success = StopDependentServices();
     if (success) {
        auto ssp = SERVICE_STATUS_PROCESS{ 0 };
        success = ChangeServiceStatus(srvHandle, SERVICE_CONTROL_STOP, ssp);
     }
  }
  return success;
}

bool ServiceManager::Pause() {
  bool success = false;
  if (srvHandle) {
     auto ssp = SERVICE_STATUS_PROCESS{ 0 };
     success = ChangeServiceStatus(srvHandle, SERVICE_CONTROL_PAUSE, ssp);
  }
  return success;
}

bool ServiceManager::Continue() {
  bool success = false;
  if (srvHandle) {
     auto ssp = SERVICE_STATUS_PROCESS{ 0 };
     success = ChangeServiceStatus(srvHandle, SERVICE_CONTROL_CONTINUE, ssp);
  }
  return success;
}

bool ServiceManager::Delete() {
  bool success = false;
  if (srvHandle) {
     success = ::DeleteService(srvHandle) != 0;
     if (success)
        srvHandle = nullptr;
  }
  return success;
}

bool ServiceManager::ChangeServiceStatus(SC_HANDLE const handle, DWORD const controlCode, SERVICE_STATUS_PROCESS& ssp) {
  bool success = false;
  if (handle) 
     success = ::ControlService(handle, controlCode, reinterpret_cast<LPSERVICE_STATUS>(&ssp)) != 0;
  return success;
}

bool ServiceManager::StopDependentServices() {
  auto ess = ENUM_SERVICE_STATUS{ 0 };
  auto bytesNeeded = DWORD{ 0 };
  auto count = DWORD{ 0 };
  if (!::EnumDependentServices( srvHandle, SERVICE_ACTIVE, nullptr, 0, &bytesNeeded, &count)) {
     if (GetLastError() != ERROR_MORE_DATA)
        return false;
     std::vector<unsigned char> buffer(bytesNeeded, 0);
     if (!::EnumDependentServices(srvHandle, SERVICE_ACTIVE, reinterpret_cast<LPENUM_SERVICE_STATUS>(buffer.data()),
                                  bytesNeeded, &bytesNeeded, &count))
        return false;
     for (auto i = DWORD{ 0 }; i < count; ++i) {
        auto ess = static_cast<ENUM_SERVICE_STATUS>
                   (*(reinterpret_cast<LPENUM_SERVICE_STATUS>(buffer.data() + i)));
        ServiceHandle handle = ::OpenService(scHandle, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
        if (!handle)
           return false;
        auto ssp = SERVICE_STATUS_PROCESS{ 0 };
        if (!ChangeServiceStatus(handle, SERVICE_CONTROL_STOP, ssp))
           return false;
        if (!WaitForStatus(handle, ssp, ServiceStatus::Stopped))
           return false;
     }
  }
  return true;
}

bool ServiceManager::WaitForStatus(ServiceStatus desiredStatus, 
                                   std::chrono::milliseconds const timeout) {
  auto success = false;

  if (srvHandle)
  {
     auto ssp = SERVICE_STATUS_PROCESS{ 0 };

     auto bytesNeeded = DWORD{ 0 };

     if (::QueryServiceStatusEx(
        srvHandle,
        SC_STATUS_PROCESS_INFO,
        reinterpret_cast<LPBYTE>(&ssp),
        sizeof(ssp),
        &bytesNeeded))
     {
        success = WaitForStatus(srvHandle, ssp, desiredStatus, timeout);
     }
  }

  return success;
}

std::chrono::milliseconds ServiceManager::GetWaitTime(DWORD const waitHint) {
  auto waitTime = waitHint / 10;

  if (waitTime < 1000)
     waitTime = 1000;
  else if (waitTime > 10000)
     waitTime = 10000;

  return std::chrono::milliseconds(waitTime);
}

bool ServiceManager::WaitForStatus(SC_HANDLE const handle, 
                          SERVICE_STATUS_PROCESS& ssp, 
                          ServiceStatus const desireStatus, 
                          std::chrono::milliseconds const timeout) {
  auto success = ssp.dwCurrentState == static_cast<DWORD>(desireStatus);

  if (!success && handle)
  {
     auto start = std::chrono::high_resolution_clock::now();
     auto waitTime = GetWaitTime(ssp.dwWaitHint);

     while (ssp.dwCurrentState != static_cast<DWORD>(desireStatus))
     {
        std::this_thread::sleep_for(waitTime);

        auto bytesNeeded = DWORD{ 0 };

        if (!::QueryServiceStatusEx(
           handle,
           SC_STATUS_PROCESS_INFO,
           reinterpret_cast<LPBYTE>(&ssp),
           sizeof(ssp),
           &bytesNeeded))
           break;

        if (ssp.dwCurrentState == static_cast<DWORD>(desireStatus))
        {
           success = true;
           break;
        }

        if (std::chrono::high_resolution_clock::now() - start > timeout)
           break;
     }
  }

  return success;
}

bool ServiceManager::GetSrvHandle() const {
    return srvHandle;
}