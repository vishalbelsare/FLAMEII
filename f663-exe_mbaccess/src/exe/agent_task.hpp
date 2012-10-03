/*!
 * \file src/exe/agent_task.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task that runs agent functions
 */
#ifndef EXE__AGENT_TASK_HPP_
#define EXE__AGENT_TASK_HPP_
#include <string>
#include <utility>
#include <map>
#include <set>
#include "mem/memory_manager.hpp"
#include "mem/memory_iterator.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

class AgentTask : public Task {
  friend class TaskManager;

  public:
    //! Enable access to a specific agent var
    void AllowAccess(const std::string& var_name, bool writeable = false);

    //! Define read access to message board
    void ReadsMessage(const std::string& msg_name);

    //! Define post access to message board
    void PostsMessage(const std::string& msg_name);

    //! Returns a new instance of a MemoryIterator
    flame::mem::MemoryIteratorPtr GetMemoryIterator() const;

    //! Returns the name of the task
    std::string get_task_name() const;

    //! Returns the the task type
    TaskType get_task_type() const { return Task::AGENT_FUNCTION; }

    //! Runs the task
    void Run();

  protected:
    // Tasks should only be created via Task Manager
    AgentTask(std::string task_name, std::string agent_name,
              TaskFunction func_ptr);

  private:
    std::string agent_name_;  //! Name of associated agent
    TaskFunction func_;  //! Function associated with task
    flame::mem::AgentShadowPtr shadow_ptr_;  //! Pointer to AgentShadow
};

}}  // namespace flame::exe
#endif  // EXE__AGENT_TASK_HPP_
