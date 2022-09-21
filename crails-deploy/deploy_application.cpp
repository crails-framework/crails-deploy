#include "deploy_application.hpp"

using namespace Crails;
using namespace std;

void ApplicationPackageDeploy::install_application()
{
  deploy_package();
  create_runtime_directory();
  create_log_directory();
}

void ApplicationPackageDeploy::deploy_package()
{
  stringstream command;
  int rc;

  if (sudo) command << "sudo ";
  command << "tar xf \"" << tmp_filepath << "\" --directory /";
  ssh.make_scp_session("/tmp", Ssh::WriteMode)->push_file(package, tmp_filename);
  rc = ssh.exec(command.str(), std_out);
  if (rc != 0)
    throw std::runtime_error("could not extract package");
  ssh.exec("rm \"" + tmp_filepath + '"', null_output);
}

void ApplicationPackageDeploy::create_runtime_directory()
{
  if (mkdir(runtime_directory) != 0)
    throw std::runtime_error("could not create runtime directory");
}

void ApplicationPackageDeploy::create_log_directory()
{
  if (mkdir(log_directory) != 0)
    throw std::runtime_error("could not create log directory");
}

int ApplicationPackageDeploy::mkdir(const std::string& path)
{
  stringstream command;

  if (sudo) command << "sudo ";
  command << "mkdir -p \"" << path << '"';
  return ssh.exec(command.str(), null_output);
}
