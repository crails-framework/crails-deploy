#include "deploy_application.hpp"
#include "freebsd/package_deploy.hpp"

using namespace Crails;
using namespace std;

bool ApplicationPackageDeploy::initialize_target()
{
  if (target_type == FreeBSDTarget)
    interface = new FreeBSDPackageDeploy(*this);
  else
    interface = new PackageDeployInterface(*this);
  return true;
}

void ApplicationPackageDeploy::install_application()
{
  deploy_package();
  create_runtime_directory();
  create_log_directory();
}

void ApplicationPackageDeploy::deploy_package()
{
  int rc = interface->extract(package, "/");

  if (rc != 0)
    throw std::runtime_error("could not extract package");
}

void ApplicationPackageDeploy::create_runtime_directory()
{
  if (interface->mkdir(runtime_directory) != 0)
    throw std::runtime_error("could not create runtime directory");
}

void ApplicationPackageDeploy::create_log_directory()
{
  if (interface->mkdir(log_directory) != 0)
    throw std::runtime_error("could not create log directory");
}

int PackageDeployInterface::extract(const std::string& tarball, const std::string& target)
{
  stringstream command;
  int rc;

  if (sudo) command << "sudo ";
  command << "tar xf \"" << tmp_filepath << "\" --directory \"" << target << '"';
  ssh.make_scp_session("/tmp", Ssh::WriteMode)->push_file(tarball, tmp_filename);
  rc = ssh.exec(command.str(), std_out);
  ssh.exec("rm \"" + tmp_filepath + '"', null_output);
  return rc;
}

int PackageDeployInterface::mkdir(const std::string& path)
{
  stringstream command;

  if (sudo) command << "sudo ";
  command << "mkdir -p \"" << path << '"';
  return ssh.exec(command.str(), null_output);
}
