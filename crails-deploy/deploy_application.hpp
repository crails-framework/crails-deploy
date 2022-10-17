#pragma once
#include "settings.hpp"
#include "ssh.hpp"

class PackageDeployInterface;

class ApplicationPackageDeploy :
  protected virtual ApplicationDeploySettings,
  public virtual ApplicationDeploySsh
{
  friend class PackageDeployInterface;
  PackageDeployInterface* interface = nullptr;
public:
  bool initialize_target();
  void install_application();

private:
  void deploy_package();
  void create_runtime_directory();
  void create_log_directory();
  std::string generate_environment_file() const;
};

class PackageDeployInterface :
  protected ApplicationDeploySettings,
  protected ApplicationDeployProxySsh
{
public:
  PackageDeployInterface(ApplicationPackageDeploy& source) : ApplicationDeploySettings(source), ApplicationDeployProxySsh(source) {}
  virtual ~PackageDeployInterface() {}

  virtual int extract(const std::string& tarball, const std::string& target);
  virtual int mkdir(const std::string& path);
  virtual int push_text(const std::string& text, const std::string& target);
};
