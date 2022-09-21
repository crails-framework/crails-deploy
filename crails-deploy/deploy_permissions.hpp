#pragma once
#include "settings.hpp"
#include "ssh.hpp"

class ApplicationPermissionDeploy :
  protected virtual ApplicationDeploySettings,
  protected virtual ApplicationDeploySsh
{
public:
  void set_permissions();

private:
  void set_permissions_on(const std::string& target, const std::string permissions);

  int chown(const std::string& target);
  int chgrp(const std::string& target);
  int chmod(const std::string& target, const std::string& permissions);
};
