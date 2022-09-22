#pragma once
#include <crails/ssh/session.hpp>
#include <exception>
#include <sstream>
#include "../settings.hpp"

struct FreeBSDHelpers
{
  static std::string get_jail_id(Crails::Ssh::Session& ssh, ApplicationDeploySettings& settings)
  {
    std::stringstream jail_id;
    int rc;

    rc = ssh.exec("jls | grep \"" + settings.freebsd_jail_root + "\" | awk '{ print $1 }'", jail_id);
    if (rc != 0)
      throw std::runtime_error("could not find jail id for `" + settings.freebsd_jail_root + "`. Make sure the jail already exists, and create it manually otherwise.");
    return jail_id.str();
  }
};
