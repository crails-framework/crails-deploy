#pragma once
#include "../deploy_user.hpp"

class LinuxUserDeploy : public UserDeployInterface
{
public:
  template<typename PARENT>
  LinuxUserDeploy(PARENT& parent) : UserDeployInterface(parent) {}

  bool user_exists() override
  {
    return ssh.exec("id -u \"" + app_user + '"', null_output) == 0;
  }

  bool group_exists() override
  {
    return ssh.exec("grep \"^" + app_group + ":\" /etc/group", null_output) == 0;
  }

  int create_user() override
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    command << "useradd --no-create-home \"" << app_user << '"'
            << " -g \"" << app_group << '"';
    return ssh.exec(command.str(), null_output);
  }

  int create_group() override
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    command << "groupadd \"" << app_group << '"';
    return ssh.exec(command.str(), null_output);
  }

  int assign_group() override
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    command << "usermod -g \"" << app_group << '"'
            << " \"" << app_user << '"';
    return ssh.exec(command.str(), null_output);
  }
};
