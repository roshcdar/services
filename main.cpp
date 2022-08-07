#include <iostream>
#include <string>
#include "ServiceManager.h"
#include "ListServices.h"

std::string serviceStatusToString(ServiceStatus status) {
    switch (status) {
    case ServiceStatus::Unknown: return "Unknown";
    case ServiceStatus::Stopped: return "Stopped";
    case ServiceStatus::Starting: return "Starting";
    case ServiceStatus::Stopping: return "Stopping";
    case ServiceStatus::Running: return "Running";
    case ServiceStatus::Continuing: return "Continuing";
    case ServiceStatus::Pausing: return "Pausing";
    case ServiceStatus::Paused: return "Paused";
    default: return "Unknown status type";
    }
}

void printStatus(ServiceStatus status) {
    std::cout << "Status:         " << serviceStatusToString(status) << std::endl;
}

void list() {
    auto services = listServices();
    if (services.size() > 0)
        std::cout << "--------------------------" << std::endl;
    for (auto const& s : services)
    {
        std::wstring serviceName(s.ServiceName.begin(), s.ServiceName.end());
        std::wstring displayName(s.DisplayName.begin(), s.DisplayName.end());
        std::cout << "Service name:  " << s.ServiceName << std::endl;
        std::cout << "Display name:  " << s.DisplayName << std::endl;
        printStatus(static_cast<ServiceStatus>(s.Status.dwCurrentState));
        std::cout << "--------------------------" << std::endl;
    }
}

void select(const std::string serviceName) {
    std::string command;
    auto service = ServiceManager{ serviceName };
    if (!service.GetSrvHandle()) {
        std::cout << "Service can't be managed" << std::endl;
        return;
    }
    while (getline(std::cin, command)) {
        if (command == "status")
            printStatus(service.GetStatus());
        else if (command == "start") {
            if (!service.Start())
                std::cout << "Service can't be started" << std::endl;
            else {
                service.WaitForStatus(ServiceStatus::Running);
                printStatus(service.GetStatus());
            }
        }
        else if (command == "stop") {
            if (!service.Stop())
                std::cout << "Service can't be stopped" << std::endl;
            else {
                service.WaitForStatus(ServiceStatus::Stopped);
                printStatus(service.GetStatus());
            }
        }
        else if (command == "pause") {
            if (!service.Pause())
                std::cout << "Service can't be paused" << std::endl;
            else {
                service.WaitForStatus(ServiceStatus::Paused);
                printStatus(service.GetStatus());
            }
        }
        else if (command == "continue") {
            if (!service.Continue())
                std::cout << "Service can't be continued" << std::endl;
            else {
                service.WaitForStatus(ServiceStatus::Running);
                printStatus(service.GetStatus());
            }
        }
        else if (command == "delete") {
            if (!service.Delete())
                std::cout << "Service can't be deleted" << std::endl;
        }
        else if (command == "unselect")
            break;
    }
}

void readInput() {
    std::string command;
    while (getline(std::cin, command)) {
        if (command == "list") 
            list();
        else if (command.size() >= 7 && command.substr(0, 6) == "select") 
            select(command.substr(7));
    }
    if (!std::cin.eof())
        std::cout << "Smth went wrong." << std::endl;
}

int main() {
    readInput();
    return 0;
}