#include <crails/logger.hpp>
#include <crails/cli/command.hpp>
#include <crails/cli/process.hpp>
#include <crails/cli/notifier.hpp>
#include <cstdlib>
#include <filesystem>
#include "deploy_application.hpp"
#include "deploy_permissions.hpp"
#include "deploy_user.hpp"
#include "deploy_service.hpp"
#include <crails/read_file.hpp>

using namespace std;
using namespace Crails;

namespace Crails
{
  const Logger::Symbol Logger::log_level = Debug;
}

class DeployCommand : public Crails::Command,
  private ApplicationUserDeploy,
  private ApplicationPackageDeploy,
  private ApplicationPermissionDeploy,
  private ApplicationServiceDeploy
{
public:
  DeployCommand()
  {
  }

  void options_description(boost::program_options::options_description& options) const override
  {
    options.add_options()
      ("verbose,v",     "enable verbose mode")
      ("sudo",          "administrative tasks will require root permissions")
      ("package,p",     boost::program_options::value<string>(), "application package (.tar.gz)")
      ("app-name,a",    boost::program_options::value<string>(), "application name")
      ("app-port,p",    boost::program_options::value<unsigned short>(), "network port the application will listen to")
      ("app-host",      boost::program_options::value<string>(), "network host the application will listen to")
      ("hostname,o",    boost::program_options::value<string>(), "deployment target")
      ("deploy-user,d", boost::program_options::value<string>(), "user performing the deployment")
      ("root,r",        boost::program_options::value<string>(), "remote install directory")
      ("user,u",        boost::program_options::value<string>(), "user which will run the application")
      ("group,g",       boost::program_options::value<string>(), "user group which will run the application")
      ("runtime-path",  boost::program_options::value<string>(), "runtime path (defaults to /var/lib/application-name)")
      ("start",         boost::program_options::value<string>(), "command to start the service")
      ("stop",          boost::program_options::value<string>(), "command to stop the service")
      ("env,e",         boost::program_options::value<vector<string>>()->multitoken(), "list of environment variables or files (such as: -e VAR1=value1 VAR2=value2 /tmp/env)")
      ("pubkey", "ssh authentication using rsa public key")
      ("password",      boost::program_options::value<string>(), "password used for ssh authentication (using the SSH_PASSWORD environment variable will be more secure than this CLI option)")
      ("jail-path",     boost::program_options::value<string>(), "freebsd jail path")
      ;
  }

  void initialize_environment()
  {
    for (const string& param : options["env"].as<vector<string>>())
    {
      string contents;

      if (param.find('=') != string::npos)
        contents = param;
      else if (!Crails::read_file(param, contents))
        cerr << "/!\\ Could not read environment file `" << param << '`' << endl;
      environment_variables.push_back(contents);
    }
  }

  void initialize_options()
  {
    const char* env_password = std::getenv("SSH_PASSWORD");

    if (!options.count("package"))
      throw std::runtime_error("missing required option --package");
    sudo      = options.count("sudo") > 0;
    if (options.count("app-name"))
      app_name = options["app-name"].as<string>();
    else
    {
      project_variables.initialize();
      app_name = project_variables.variable("name");
    }
    if (app_name.length() == 0)
      throw std::runtime_error("missing app-name option");
    package   = options["package"].as<string>();
    app_user  = options.count("user") ? options["user"].as<string>() : app_name;
    app_group = options.count("group") ? options["group"].as<string>() : app_user;
    runtime_directory = "/var/lib/" + app_name;
    log_directory = "/var/log/" + app_name;
    environment_file_path = "/usr/share/crails-deploy/" + app_name;
    if (env_password)
      password = env_password;
    if (options.count("hostname"))
      hostname = options["hostname"].as<string>();
    if (options.count("deploy-user"))
      deploy_user = options["deploy-user"].as<string>();
    if (options.count("password"))
      password = options["password"].as<string>();
    if (options.count("root"))
      root = options["root"].as<string>();
    if (options.count("runtime-path"))
      runtime_directory = options["runtime-path"].as<string>();
    if (options.count("start"))
      start_command = options["start"].as<string>();
    if (options.count("stop"))
      stop_command = options["stop"].as<string>();
    if (options.count("env"))
      initialize_environment();
    if (options.count("app-port"))
      app_port = options["app-port"].as<unsigned short>();
    if (options.count("app-host"))
      app_host = options["app-host"].as<std::string>();
    tmp_filename = "crails." + app_name + ".tar.gz";
    tmp_filepath = "/tmp/" + tmp_filename;
    if (!std::filesystem::is_regular_file(package))
      throw std::runtime_error("invalid package path");
  }

  int run() override
  {
    try
    {
      Ssh::Session ssh;
      bool initialize_properly;

      initialize_options();
      connect();
      probe_target_type();

      initialize_properly = ApplicationUserDeploy::initialize_target()
                         && ApplicationPackageDeploy::initialize_target()
                         && ApplicationPermissionDeploy::initialize_target()
                         && ApplicationServiceDeploy::initialize_target();
      if (!initialize_properly)
        throw std::runtime_error("could not properly initialize deployer for target");

      prepare_application_user();
      install_application();
      deploy_service();
      set_permissions();
      restart_service();
      Crails::cli_notification("crails-deploy", "Deployed successfully", "success");
    }
    catch (const std::exception& err)
    {
      Crails::cli_notification("crails-deploy", string("failed to deploy: ") + err.what(), "failure");
      std::cerr << "Error occured: " << err.what() << std::endl;
      return -1;
    }
    return 0;
  }

  void connect()
  {
    ssh.connect(deploy_user, hostname);
    if (options.count("pubkey"))
      ssh.authentify_with_pubkey(password);
    else if (password.length() > 0)
      ssh.authentify_with_password(password);
  }

  void probe_target_type()
  {
    std::stringstream result;

    if (ssh.exec("uname -a | cut -d' ' -f1", result) == 0)
    {
      std::string system_name = result.str();
      if (system_name.find("Linux") != std::string::npos)
        target_type = LinuxTarget;
      else if (system_name.find("FreeBSD") != std::string::npos)
        target_type = FreeBSDTarget;
      else
        throw std::runtime_error("unsupported target system `" + system_name + '`');
    }
    else
      throw std::runtime_error("failed to probe system on the remote target");
  }
};

int main(int argc, const char** argv)
{
  DeployCommand command;

  if (command.initialize(argc, argv))
    return command.run();
  return -1;
}
