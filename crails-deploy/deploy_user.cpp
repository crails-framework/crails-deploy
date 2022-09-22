#include "deploy_user.hpp"
#include "linux/user_deploy.hpp"
#include "freebsd/user_deploy.hpp"

using namespace Crails;
using namespace std;

ApplicationUserDeploy::~ApplicationUserDeploy()
{
  if (interface) delete interface;
}


bool ApplicationUserDeploy::initialize_target()
{
  if (target_type == LinuxTarget)
    interface = new LinuxUserDeploy(*this);
  else if (target_type == FreeBSDTarget)
    interface = new FreeBSDUserDeploy(*this);
  else
    return false;
  return true;
}

void ApplicationUserDeploy::prepare_application_user()
{
  int rc;

  if (!interface->group_exists())
  {
    rc = interface->create_group();
    if (rc != 0)
      throw runtime_error("could not create app group");
  }
  if (!interface->user_exists())
  {
    rc = interface->create_user();
    if (rc != 0)
      throw runtime_error("could not create app user");
  }
  else
  {
    rc = interface->assign_group();
    if (rc != 0)
      throw runtime_error("could not update app user");
  }
}
