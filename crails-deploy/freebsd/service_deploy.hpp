#pragma once
#include "../deploy_service.hpp"
#include "helpers.hpp"
#include <sstream>

class FreeBSDServiceDeploy : public ServiceDeployInterface
{
public:
  template<typename PARENT>
  FreeBSDServiceDeploy(PARENT& parent) : ServiceDeployInterface(parent) {}

  void start()   override { service_operation("start"); }
  void stop()    override { service_operation("stop"); }

  void enable()  override
  {
    if (!is_enabled())
    {
      std::stringstream append_command;
      append_command << "echo '" << app_name << "_enable=\"YES\"' >> " << rc_path();
      ssh.exec(append_command.str(), null_output);
    }
  }

  void disable() override
  {
    if (is_enabled())
    {
      std::stringstream rm_command, mv_command;
      rm_command << "awk '!/" << app_name << "_enable=\"YES\"/{print}' \"" << rc_path() << '"'
                 << " >> /tmp/awk.tmp";
      mv_command << "mv /tmp/awk.tmp \"" << rc_path() << '"';
      ssh.exec(rm_command.str(), null_output);
      ssh.exec(mv_command.str(), null_output);
    }
  }

  bool is_enabled()
  {
    std::stringstream lookup_command;
    lookup_command << "cat \"" << rc_path() << "\" | grep \"" << app_name << "_enable=\"YES\"";
    return ssh.exec(lookup_command.str(), null_output) == 0;
  }

private:
  std::string rc_path()
  {
    std::string value = "/etc/rc.conf";

    if (freebsd_jail_root.length() > 0) value = freebsd_jail_root + value;
    return value;
  }

  int service_operation(const std::string& operation)
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    if (freebsd_jail_root.length() > 0) command << "jexec " << FreeBSDHelpers::get_jail_id(ssh, *this) << ' ';
    command << "service " << app_name << ' ' << operation;
    return ssh.exec(command.str(), std_out);
  }
};
