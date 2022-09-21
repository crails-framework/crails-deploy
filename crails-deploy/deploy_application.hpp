#pragma once
#include "settings.hpp"
#include "ssh.hpp"

class ApplicationPackageDeploy :
  protected virtual ApplicationDeploySettings,
  public virtual ApplicationDeploySsh
{
public:
  void install_application();

private:
  void deploy_package();
  void create_runtime_directory();
  void create_log_directory();

  int mkdir(const std::string& path);
};
