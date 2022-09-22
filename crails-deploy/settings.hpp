#pragma once
#include <string>
#include <crails/cli/project_variables.hpp>

enum DeployTargetType
{
  LinuxTarget,
  FreeBSDTarget
};

struct ApplicationDeploySettings
{
  Crails::ProjectVariables project_variables;
  bool verbose = false;
  bool sudo = false;
  std::string app_name;
  std::string deploy_user = "root";
  std::string hostname = "localhost";
  std::string root = "/usr/local";
  std::string runtime_directory;
  std::string log_directory;
  std::string app_user, app_group;
  std::string password;
  std::string package;
  std::string tmp_filepath, tmp_filename;
  std::string freebsd_jail_root;
  DeployTargetType target_type;

  ApplicationDeploySettings() : project_variables(".crails")
  {
  }
};
