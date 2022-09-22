#pragma once
#include "settings.hpp"
#include "ssh.hpp"

class ServiceDeployInterface;

class ApplicationServiceDeploy :
  protected virtual ApplicationDeploySettings,
  protected virtual ApplicationDeploySsh
{
  friend class ServiceDeployInterface;
  ServiceDeployInterface* interface;
public:
  bool initialize_target();
  void deploy_service();
};

class ServiceDeployInterface :
  protected ApplicationDeploySettings,
  protected ApplicationDeployProxySsh
{
public:
  template<typename PARENT>
  ServiceDeployInterface(PARENT& source) : ApplicationDeploySettings(source), ApplicationDeployProxySsh(source) {}
  virtual ~ServiceDeployInterface() {}

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void enable() = 0;
  virtual void disable() = 0;
};
