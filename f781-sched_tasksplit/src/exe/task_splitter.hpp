/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 *
 * This class is not thread-safe and relies on the calling class to
 * protect all access to with a mutex.
 */
#ifndef EXE__TASK_SPLITTER_HPP_
#define EXE__TASK_SPLITTER_HPP_
#include <vector>
#include "boost/thread/mutex.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

class TaskSplitter {
  public:
    typedef std::vector<Task::Handle> TaskVector;
    TaskSplitter(Task::id_type id, const TaskVector& tasks);

    //! Returns true of no more pending or running tasks
    bool IsComplete(void) const;

    //! Returns true if all tasks have been assigned (may be still running tasks)
    bool NonePending(void) const;

    //! Decrements pending_ and returns true if NonePending()
    bool OneTaskAssigned(void);

    //! Decrements running_ and returns true if IsComplete()
    bool OneTaskDone(void);

    //! Returns reference to the next task and decrements next_
    Task& GetTask();

  protected:
  private:
    Task::id_type id_;
    size_t pending_;
    size_t running_;
    size_t next_;
    TaskVector tasks_;
    boost::mutex mutex_;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_SPLITTER_HPP_
