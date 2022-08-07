#include "ServiceHandle.h"
#include <utility>

ServiceHandle::ServiceHandle(SC_HANDLE const serviceHandle) noexcept :handle(handle) {}

ServiceHandle::ServiceHandle(ServiceHandle&& serviceHandle) noexcept : handle(std::move(serviceHandle.handle)) {}

ServiceHandle::~ServiceHandle() {
      Close();
}

ServiceHandle& ServiceHandle::operator=(SC_HANDLE const serviceHandle) {
      if (handle != serviceHandle) {
         Close();
         handle = serviceHandle;
      }
      return *this;
}

ServiceHandle& ServiceHandle::operator=(ServiceHandle&& serviceHandle) {
      if (this != &serviceHandle) {
         handle = std::move(serviceHandle.handle);
         serviceHandle.handle = nullptr;
      }
      return *this;
}

ServiceHandle::operator SC_HANDLE() const noexcept { return handle; }

ServiceHandle::operator bool() const noexcept { return handle != nullptr; }
   
void ServiceHandle::Close() {
      if (handle != nullptr)
         ::CloseServiceHandle(handle);
}
   
   