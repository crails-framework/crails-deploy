#include "deploy_service.hpp"
#include "linux/service_deploy.hpp"
#include "freebsd/service_deploy.hpp"

using namespace std;

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
  interface->create_shared_directory();
  interface->push_environment_file();
  interface->push_service_file();
}

void ApplicationServiceDeploy::restart_service()
{
  interface->reload_service_files();
  interface->enable();
  interface->stop();
  interface->start();
}

void ServiceDeployInterface::create_shared_directory()
{
  stringstream stream;
  if (sudo)
    stream << "sudo ";
  stream << "mkdir -p \"" << deploy_shared_directory << '"';
  ssh.exec(stream.str(), std_out);
}

void ServiceDeployInterface::create_file(std::filesystem::path path, const std::string& content)
{
  std::stringstream command;

  if (sudo)
    command << "sudo ";
  command << "mv \"/tmp/" << app_name << '.' << path.filename().string() << "\" \"" << path.string() << '"';
  ssh.make_scp_session("/tmp", Crails::Ssh::WriteMode)->push_text(content, app_name + '.' + path.filename().string());
  ssh.exec(command.str(), std_out);
}

std::string ServiceDeployInterface::generate_environment_file() const
{
  stringstream stream;
  stream
    << "#!/bin/sh\n"
    << "APPLICATION_NAME=\"" << app_name << "\"\n"
    << "APPLICATION_HOST=\"" << app_host << "\"\n"
    << "APPLICATION_PORT=\"" << app_port << "\"\n"
    << "VAR_DIRECTORY=\"" << runtime_directory << "\"";
  for (const string& line : environment_variables)
    stream << '\n' << line;
  return stream.str();
}
