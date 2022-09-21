#include "deploy_permissions.hpp"

using namespace std;
using namespace Crails;

void ApplicationPermissionDeploy::set_permissions()
{
  set_permissions_on(
    runtime_directory,
    "gu+rw,o-rwx"
  );
  set_permissions_on(
    log_directory,
    "gu+rw,o-rwx"
  );
  set_permissions_on(
    root + "/share/" + app_name,
    "gu+r,o-rwx"
  );
  set_permissions_on(
    root + "/bin/" + app_name,
    "gu+rx,o-rwx"
  );
}

void ApplicationPermissionDeploy::set_permissions_on(const std::string& target, const std::string permissions)
{
  int rc =
    chown(target) +
    chgrp(target) +
    chmod(target, permissions);
  if (rc != 0)
    throw std::runtime_error("could not set permissions");
}

int ApplicationPermissionDeploy::chown(const std::string& target)
{
  stringstream command;

  if (sudo) command << "sudo ";
  command << "chown -R \"" << app_user << "\" \"" << target << '"';
  return ssh.exec(command.str(), null_output);
}

int ApplicationPermissionDeploy::chgrp(const std::string& target)
{
  stringstream command;

  if (sudo) command << "sudo ";
  command << "chgrp -R \"" << app_group << "\" \"" << target << '"';
  return ssh.exec(command.str(), null_output);
}

int ApplicationPermissionDeploy::chmod(const std::string& target, const std::string& permissions)
{
  stringstream command;

  if (sudo) command << "sudo ";
  command << "chmod -R " << permissions << " \"" << target << '"';
  return ssh.exec(command.str(), null_output);
}
