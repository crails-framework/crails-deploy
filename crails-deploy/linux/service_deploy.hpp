#pragma once
#include "../deploy_service.hpp"
#include <sstream>

class LinuxServiceDeploy : public ServiceDeployInterface
{
public:
  template<typename PARENT>
  LinuxServiceDeploy(PARENT& parent) : ServiceDeployInterface(parent) {}

  void start()   override { systemctl_operation("restart"); }
  void stop()    override { systemctl_operation("stop"); }
  void enable()  override { systemctl_operation("enable"); }
  void disable() override { systemctl_operation("disable"); }

  void reload_service_files() override
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    command << "systemctl daemon-reload";
    ssh.exec(command.str(), std_out);
  }

private:
  int systemctl_operation(const std::string& operation)
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    command << "systemctl " << operation << ' ' << app_name << ".service";
    return ssh.exec(command.str(), std_out);
  }
};
