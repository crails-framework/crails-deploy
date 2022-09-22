#pragma once
#include "../deploy_user.hpp"

class FreeBSDUserDeploy : public UserDeployInterface
{
  void freebsd_jail_pw(std::stringstream& stream)
  {
    if (freebsd_jail_root.length() > 0)
      stream << " -R \"" << freebsd_jail_root << '"';
  }
public:
  template<typename PARENT>
  FreeBSDUserDeploy(PARENT& parent) : UserDeployInterface(parent) {};

  bool user_exists() override
  {
    std::stringstream command;

    command << "pw user show";
    freebsd_jail_pw(command);
    command << " \"" << app_user << '"';
    return ssh.exec(command.str(), null_output) == 0;
  }

  bool group_exists() override
  {
    std::stringstream command;

    command << "pw group show";
    freebsd_jail_pw(command);
    command << " \"" << app_group << '"';
    return ssh.exec(command.str(), null_output) == 0;
  }

  int create_user() override
  {
    std::stringstream command;

    command << "pw user add";
    freebsd_jail_pw(command);
    command << " -n \"" << app_user << "\" -g \"" << app_group << "\" -s /bin/sh";
    return ssh.exec(command.str(), null_output);
  }

  int create_group() override
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    command << "pw group add";
    freebsd_jail_pw(command);
    command << " -n \"" << app_group << '"';
    return ssh.exec(command.str(), null_output);
  }

  int assign_group() override
  {
    std::stringstream command;

    if (sudo) command << "sudo ";
    command << "pw user mod";
    freebsd_jail_pw(command);
    command << " -n \"" << app_user << '"' << " -g \"" << app_group << '"';
    return ssh.exec(command.str(), null_output);
  }
};
