#pragma once
#include <windows.h>
#include <Winsvc.h>

class ServiceHandle {     
public:
   ServiceHandle(SC_HANDLE const serviceHandle = nullptr) noexcept;
   ServiceHandle(ServiceHandle&& serviceHandle) noexcept;
   ~ServiceHandle();
   ServiceHandle& operator=(SC_HANDLE const serviceHandle);
   ServiceHandle& operator=(ServiceHandle&& serviceHandle);
   operator SC_HANDLE() const noexcept;
   explicit operator bool() const noexcept;
private:
    void Close();
    SC_HANDLE handle = nullptr;
};