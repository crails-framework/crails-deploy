#pragma once
#include "settings.hpp"
#include "ssh.hpp"

class PermissionDeployInterface;

class ApplicationPermissionDeploy :
  protected virtual ApplicationDeploySettings,
  protected virtual ApplicationDeploySsh
{
  friend class PermissionDeployInterface;
  PermissionDeployInterface* interface;
public:
  bool initialize_target();
  void set_permissions();

private:

};

class PermissionDeployInterface :
  protected ApplicationDeploySettings,
  protected ApplicationDeployProxySsh
{
public:
  PermissionDeployInterface(ApplicationPermissionDeploy& parent) : ApplicationDeploySettings(parent), ApplicationDeployProxySsh(parent) {}
  virtual ~PermissionDeployInterface() {}

  virtual void set_permissions_on(const std::string& target, const std::string permissions, std::string command_prefix = "");
  virtual std::string command_chown(const std::string& target);
  virtual std::string command_chgrp(const std::string& target);
  virtual std::string command_chmod(const std::string& target, const std::string& permissions);
};
