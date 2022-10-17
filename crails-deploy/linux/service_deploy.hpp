#pragma once
#include "../deploy_service.hpp"
#include <sstream>
#include <filesystem>

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

  void push_environment_file() override
  {
    std::filesystem::path path(deploy_shared_directory + '/' + app_name);

    create_file(path, generate_environment_file());
  }

  void push_service_file() override
  {
    std::filesystem::path path("/etc/systemd/system/" + app_name + ".service");

    create_file(path, generate_service_file());
  }

private:
  int systemctl_operation(const std::string& operation)
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    command << "systemctl " << operation << ' ' << app_name << ".service";
    return ssh.exec(command.str(), std_out);
  }

  std::string generate_service_file()
  {
    using namespace std;
    stringstream stream;

    stream
      << "[Unit]\n"
      << "Description=" << app_name << " Web Application\n"
      << "After=network.target\n"
      << '\n'
      << "[Service]\n"
      << "WorkingDirectory=" << runtime_directory << '\n'
      << "EnvironmentFile=" << deploy_shared_directory << '/' << app_name << '\n';
    if (app_user.length() > 0)
      stream << "User=" << app_user << '\n';
    if (app_group.length() > 0)
      stream << "Group=" << app_group << '\n';
    stream << "ExecStart=" << start_command << '\n';
    if (stop_command.length() > 0)
      stream << "ExecStop=" << stop_command << '\n';
    stream
      << "PIDFile=/tmp/" << app_name << ".pid\n"
      << "Restart=always\n\n"
      << "[Install]\n"
      << "WantedBy=multi-user.target";
    return stream.str();
  }
};
