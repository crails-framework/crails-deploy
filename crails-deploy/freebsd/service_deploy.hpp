#pragma once
#include "../deploy_service.hpp"
#include "helpers.hpp"
#include <sstream>
#include <filesystem>

class FreeBSDServiceDeploy : public ServiceDeployInterface
{
  std::string relative_shared_directory;
public:
  template<typename PARENT>
  FreeBSDServiceDeploy(PARENT& parent) : ServiceDeployInterface(parent)
  {
    relative_shared_directory = deploy_shared_directory;
    deploy_shared_directory = freebsd_jail_root + deploy_shared_directory;
  }

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

  void push_service_file() override
  {
    std::filesystem::path path(freebsd_jail_root + "/etc/rc.d/" + app_name + ".rc");

    create_file(path, generate_service_file());
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

  std::string generate_service_file()
  {
    using namespace std;
    stringstream stream;
    string start_function = app_name + "_start";
    string stop_function = app_name + "_stop";

    stream
      << "#!/bin/sh\n"
      << "#\n\n"
      << "# PROVIDE: " << app_name << "\n\n"
      << ". /etc/rc.subr\n\n"
      << "name=\"" << app_user << "\"\n"
      << "rcvar=\"" << app_name << "_enable\"\n"
      << "command=\"" << start_function << "\"\n";
    if (stop_command.length() > 0)
      stream << "stop_cmd=\"" << stop_function << "\"\n";
    stream << '\n'
      << start_function << "()\n"
      << "{\n"
      << ". " << relative_shared_directory << '/' << app_name << '\n';
    if (app_user.length() > 0)
      stream << "su -c " << start_command << " - " << app_user;
    else
      stream << start_command;
    stream << "}\n\n";
    if (stop_command.length() > 0)
    {
      stream << stop_command << "()\n{\n"
        << ". " << relative_shared_directory << '/' << app_name << '\n';
      if (app_user.length() > 0)
        stream << "su -c " << stop_command << " - " << app_user;
      else
        stream << stop_command;
      stream << "}\n\n";
    }
    stream
      << "load_rc_config $name\n"
      << "run_rc_command \"$1\"";
    return stream.str();
  }
};
