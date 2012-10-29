/*!
 * \file src/mem/memory_manager.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MemoryManager: management and storage class for agent data
 */
#ifndef MEM__MEMORY_MANAGER_HPP_
#define MEM__MEMORY_MANAGER_HPP_
#include <map>
#include <string>
#include <vector>
#include "agent_memory.hpp"

namespace flame { namespace mem {

//! Map to store collection of AgentMemory
typedef std::map<std::string, AgentMemory> AgentMap;

class MemoryManager {
  public:
    void RegisterAgent(std::string agent_name, size_t pop_size_hint);

    template <class T>
    void RegisterAgentVar(std::string agent_name, std::string var_name) {
      AgentMemory &agent = GetAgent(agent_name);
      agent.RegisterVar<T>(var_name);
    }

    template <class T>
    void RegisterAgentVar(std::string agent_name,
                          std::vector<std::string> var_names) {
      AgentMemory &agent = GetAgent(agent_name);

      std::vector<std::string>::iterator it;
      for (it = var_names.begin(); it < var_names.end(); it++) {
        agent.RegisterVar<T>(*it);
      }
    }

    template <class T>
    std::vector<T>& GetMemoryVector(std::string const& agent_name,
                                    std::string const& var_name) {
      return GetAgent(agent_name).GetMemoryVector<T>(var_name);
    }

  private:
    AgentMemory& GetAgent(std::string const& agent_name);
    AgentMap agent_map_;
};
}}  // namespace flame::mem
#endif  // MEM__MEMORY_MANAGER_HPP_