#include <crails/logger.hpp>
#include <crails/cli/command.hpp>
#include <crails/cli/process.hpp>
#include <crails/cli/notifier.hpp>
#include <cstdlib>
#include <filesystem>
#include "deploy_application.hpp"
#include "deploy_permissions.hpp"
#include "deploy_user.hpp"

using namespace std;
using namespace Crails;

namespace Crails
{
  const Logger::Symbol Logger::log_level = Debug;
}

class DeployCommand : public Crails::Command,
  private ApplicationUserDeploy,
  private ApplicationPackageDeploy,
  private ApplicationPermissionDeploy
{
public:
  DeployCommand()
  {
  }

  void options_description(boost::program_options::options_description& options) const override
  {
    options.add_options()
      ("verbose,v",     "enable verbose mode")
      ("sudo",          "administrative task will require root permissions")
      ("package,p",     boost::program_options::value<string>(), "application package (.tar.gz)")
      ("hostname,h",    boost::program_options::value<string>(), "deployment target")
      ("deploy-user,d", boost::program_options::value<string>(), "user performing the deployment")
      ("root,r",        boost::program_options::value<string>(), "remote install directory")
      ("user,u",        boost::program_options::value<string>(), "user which will run the application")
      ("group,g",       boost::program_options::value<string>(), "user group which will run the application")
      ("runtime-path",  boost::program_options::value<string>(), "runtime path (defaults to /var/application-name)")
      ("pubkey", "ssh authentication using rsa public key")
      ("password",      boost::program_options::value<string>(), "password used for ssh authentication (using the SSH_PASSWORD environment variable will be more secure than this CLI option)");
  }

  void initialize_options()
  {
    const char* env_password = std::getenv("SSH_PASSWORD");

    project_variables.initialize();
    if (!options.count("package"))
      throw std::runtime_error("missing required option --package");
    sudo      = options.count("sudo") > 0;
    app_name  = project_variables.variable("name");
    package   = options["package"].as<string>();
    app_user  = options.count("user") ? options["user"].as<string>() : app_name;
    app_group = options.count("group") ? options["group"].as<string>() : app_user;
    runtime_directory = "/var/" + app_name;
    log_directory = "/var/log/" + app_name;
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

      initialize_options();
      connect();
      prepare_application_user();
      install_application();
      set_permissions();
      setup_systemctl();
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

  void setup_systemctl()
  {
    systemctl_operation("restart");
    systemctl_operation("enable");
  }

  int systemctl_operation(const std::string& operation)
  {
    stringstream command;
    if (sudo) command << "sudo ";
    command << " systemctl " << operation << ' ' << app_name << ".service";
    return ssh.exec(command.str(), std_out);
  }
};

int main(int argc, const char** argv)
{
  DeployCommand command;

  if (command.initialize(argc, argv))
    return command.run();
  return -1;
}
