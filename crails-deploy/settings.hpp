#pragma once
#include <string>
#include <vector>
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
  std::string start_command, stop_command;
  std::vector<std::string> environment_variables;
  std::string environment_file_path;
  std::string    app_host = "0.0.0.0";
  unsigned short app_port = 80;
  std::string password;
  std::string package;
  std::string tmp_filepath, tmp_filename;
  std::string freebsd_jail_root;
  DeployTargetType target_type;

  ApplicationDeploySettings() : project_variables(".crails")
  {
  }
};
