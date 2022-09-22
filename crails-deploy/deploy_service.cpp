#include "deploy_service.hpp"
#include "linux/service_deploy.hpp"
#include "freebsd/service_deploy.hpp"

bool ApplicationServiceDeploy::initialize_target()
{
  if (target_type == LinuxTarget)
    interface = new LinuxServiceDeploy(*this);
  else if (target_type == FreeBSDTarget)
    interface = new FreeBSDServiceDeploy(*this);
  else
    return false;
  return true;
}

void ApplicationServiceDeploy::deploy_service()
{
  interface->enable();
  interface->stop();
  interface->start();
}
