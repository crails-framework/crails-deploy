#pragma once
#include "../deploy_permissions.hpp"
#include "helpers.hpp"

class FreeBSDPermissionDeploy : public PermissionDeployInterface
{
public:
  template<typename PARENT>
  FreeBSDPermissionDeploy(PARENT& parent) : PermissionDeployInterface(parent) {}

  virtual void set_permissions_on(const std::string& target, const std::string permissions, std::string command_prefix)
  {
    if (freebsd_jail_root.length() > 0)
      command_prefix += "jexec " + FreeBSDHelpers::get_jail_id(ssh, *this) + ' ';
    PermissionDeployInterface::set_permissions_on(target, permissions, command_prefix);
  }
};
