#pragma once
#include "../deploy_application.hpp"

class FreeBSDPackageDeploy : public PackageDeployInterface
{
public:
  template<typename PARENT>
  FreeBSDPackageDeploy(PARENT& parent) : PackageDeployInterface(parent) {}

  int extract(const std::string & tarball, const std::string & target) override
  {
    if (freebsd_jail_root.length() > 0)
      return PackageDeployInterface::extract(tarball, freebsd_jail_root + target);
    return PackageDeployInterface::extract(tarball, target);
  }

  int mkdir(const std::string & path) override
  {
    if (freebsd_jail_root.length() > 0)
      return PackageDeployInterface::mkdir(freebsd_jail_root + path);
    return PackageDeployInterface::mkdir(path);
  }

  int push_text(const std::string& text, const std::string& target) override
  {
    return PackageDeployInterface::push_text(text, freebsd_jail_root + target);
  }
};
