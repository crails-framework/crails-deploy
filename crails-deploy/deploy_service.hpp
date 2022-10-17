#pragma once
#include "settings.hpp"
#include "ssh.hpp"
#include <filesystem>

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
  void restart_service();
};

class ServiceDeployInterface :
  protected ApplicationDeploySettings,
  protected ApplicationDeployProxySsh
{
public:
  template<typename PARENT>
  ServiceDeployInterface(PARENT& source) : ApplicationDeploySettings(source), ApplicationDeployProxySsh(source)
  {
    deploy_shared_directory = std::filesystem::path(environment_file_path).parent_path().string();
  }
  virtual ~ServiceDeployInterface() {}

  virtual void push_environment_file()
  {
    std::filesystem::path path(deploy_shared_directory + '/' + app_name);

    create_file(path, generate_environment_file());
  }

  virtual void push_service_file() = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void enable() = 0;
  virtual void disable() = 0;
  virtual void reload_service_files() {};

  void create_shared_directory();
  void create_file(std::filesystem::path path, const std::string& content);
  std::string generate_environment_file() const;

  std::string deploy_shared_directory;
};
