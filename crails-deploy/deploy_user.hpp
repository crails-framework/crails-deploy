#pragma once
#include "settings.hpp"
#include "ssh.hpp"

class ApplicationUserDeploy :
  protected virtual ApplicationDeploySettings,
  protected virtual ApplicationDeploySsh
{
public:
  void prepare_application_user();

private:
  bool user_exists();
  bool group_exists();
  void create_user();
  void create_group();
  void assign_group();
};
