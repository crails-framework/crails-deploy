#include "deploy_user.hpp"

using namespace Crails;
using namespace std;

void ApplicationUserDeploy::prepare_application_user()
{
  if (!group_exists())
    create_group();
  if (!user_exists())
    create_user();
  else
    assign_group();
}

bool ApplicationUserDeploy::user_exists()
{
  return ssh.exec("id -u \"" + app_user + '"', null_output) == 0;
}

bool ApplicationUserDeploy::group_exists()
{
  return ssh.exec("grep \"^" + app_group + ":\" /etc/group", null_output) == 0;
}

void ApplicationUserDeploy::create_user()
{
  stringstream command;
  int rc;

  if (sudo) command << "sudo ";
  command << "useradd --no-create-home \"" << app_user << '"'
          << " -g \"" << app_group << '"';
  rc = ssh.exec(command.str(), null_output);
}

void ApplicationUserDeploy::create_group()
{
  stringstream command;
  int rc;

  if (sudo) command << "sudo ";
  command << "groupadd \"" << app_group << '"';
  rc = ssh.exec(command.str(), null_output);
  if (rc != 0)
    throw std::runtime_error("could not create app group");
}

void ApplicationUserDeploy::assign_group()
{
  stringstream command;
  int rc;

  if (sudo) command << "sudo ";
  command << "usermod -g \"" << app_group << '"'
          << " \"" << app_user << '"';
  rc = ssh.exec(command.str(), null_output);
  if (rc != 0)
    throw std::runtime_error("could not assign group to user");
}
