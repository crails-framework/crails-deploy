#pragma once
#include <crails/ssh/session.hpp>
#include <sstream>
#include <iostream>

struct ApplicationDeploySsh
{
  Crails::Ssh::Session ssh;
  std::stringstream null_output;
  std::ostream& std_out;

  ApplicationDeploySsh() : std_out(std::cout)
  {
    null_output.setstate(std::ios_base::badbit);
  }
};
