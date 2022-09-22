#pragma once
#include "settings.hpp"
#include "ssh.hpp"

class UserDeployInterface;

class ApplicationUserDeploy :
  protected virtual ApplicationDeploySettings,
  protected virtual ApplicationDeploySsh
{
  friend class UserDeployInterface;
  UserDeployInterface* interface = nullptr;
public:
  virtual ~ApplicationUserDeploy();
  bool initialize_target();
  void prepare_application_user();
};

class UserDeployInterface :
  protected ApplicationDeploySettings,
  protected ApplicationDeployProxySsh
{
public:
  UserDeployInterface(ApplicationUserDeploy& source) : ApplicationDeploySettings(source), ApplicationDeployProxySsh(source) {}
  virtual ~UserDeployInterface() {}

  virtual bool user_exists()  = 0;
  virtual bool group_exists() = 0;
  virtual int  create_user()  = 0;
  virtual int  create_group() = 0;
  virtual int  assign_group() = 0;
};
