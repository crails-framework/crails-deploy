#include "deploy_permissions.hpp"
#include "freebsd/permission_deploy.hpp"

using namespace std;
using namespace Crails;

bool ApplicationPermissionDeploy::initialize_target()
{
  if (target_type == FreeBSDTarget)
    interface = new FreeBSDPermissionDeploy(*this);
  else
    interface = new PermissionDeployInterface(*this);
  return true;
}

void ApplicationPermissionDeploy::set_permissions()
{
  interface->set_permissions_on(
    runtime_directory,
    "gu+rw,o-rwx"
  );
  interface->set_permissions_on(
    log_directory,
    "gu+rw,o-rwx"
  );
  interface->set_permissions_on(
    root + "/share/" + app_name,
    "gu+r-w,o-rwx"
  );
  interface->set_permissions_on(
    root + "/bin/" + app_name,
    "u+rx-w,g+r-wx,o-rwx"
  );
}

void PermissionDeployInterface::set_permissions_on(const std::string& target, const std::string permissions, std::string command_prefix)
{
  if (sudo)
    command_prefix = "sudo " + command_prefix;
  int rc =
    ssh.exec(command_prefix + command_chown(target), null_output) +
    ssh.exec(command_prefix + command_chgrp(target), null_output) +
    ssh.exec(command_prefix + command_chmod(target, permissions), null_output);
  if (rc != 0)
    throw std::runtime_error("could not set permissions");
}

std::string PermissionDeployInterface::command_chown(const std::string& target)
{
  stringstream command;

  command << "chown -R \"" << app_user << "\" \"" << target << '"';
  return command.str();
}

std::string PermissionDeployInterface::command_chgrp(const std::string& target)
{
  stringstream command;

  command << "chgrp -R \"" << app_group << "\" \"" << target << '"';
  return command.str();
}

std::string PermissionDeployInterface::command_chmod(const std::string& target, const std::string& permissions)
{
  stringstream command;

  command << "chmod -R " << permissions << " \"" << target << '"';
  return command.str();
}
