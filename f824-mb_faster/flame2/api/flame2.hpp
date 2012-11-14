/*!
 * \file flame2/api/cxx/flame2.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief C++ API for FLAME2 Models
 */
#ifndef FLAME2__API__CXX__FLAME2_HPP_
#define FLAME2__API__CXX__FLAME2_HPP_
#include "agent_api.hpp"

#define FLAME_AGENT_ALIVE 0
#define FLAME_AGENT_DEAD  1

// This should match signature of flame::exe::TaskFunction
// define in flame2/exe/task_interface.hpp
#define FLAME_AGENT_FUNCTION(funcName) int funcName(flame::api::AgentAPI* FLAME)
     
namespace flame2 { namespace api {

}}  // namespace flame2::api

#endif  // FLAME2__API__CXX__FLAME2_HPP_
