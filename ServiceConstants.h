#pragma once
#include <windows.h>

enum class ServiceStatus 
{
   Unknown     = 0,
   Stopped     = SERVICE_STOPPED,
   Starting    = SERVICE_START_PENDING,
   Stopping    = SERVICE_STOP_PENDING,
   Running     = SERVICE_RUNNING,
   Continuing  = SERVICE_CONTINUE_PENDING,
   Pausing     = SERVICE_PAUSE_PENDING,
   Paused      = SERVICE_PAUSED
};

enum class ServiceType
{
   KernelDriver       = SERVICE_KERNEL_DRIVER,
   FileSystemDriver   = SERVICE_FILE_SYSTEM_DRIVER,
   Adapter            = SERVICE_ADAPTER,
   RecognizerDriver   = SERVICE_RECOGNIZER_DRIVER,
   Win32OwnProcess    = SERVICE_WIN32_OWN_PROCESS,
   Win32ShareProcess  = SERVICE_WIN32_SHARE_PROCESS,
   InteractiveDriver  = SERVICE_INTERACTIVE_PROCESS,
   Driver             = SERVICE_DRIVER,
   Win32              = SERVICE_WIN32,
   All                = SERVICE_TYPE_ALL
};

enum class ServiceState
{
   Active   = SERVICE_ACTIVE,
   Inactive = SERVICE_INACTIVE,
   All      = SERVICE_STATE_ALL
};

