/*!
 * \file flame2/model/task_list.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief TaskList: used to hold tasks
 */
#ifndef MODEL__TASK_LIST_HPP_
#define MODEL__TASK_LIST_HPP_
#include <boost/shared_ptr.hpp>
#include "task.hpp"

namespace flame { namespace model {

//! Use a shared pointer to automatically handle Task pointers
typedef boost::shared_ptr<Task> TaskPtr;

typedef std::set< std::pair<std::string, std::string> > StringPairSet;
typedef std::set<std::string> StringSet;
typedef size_t TaskId;
typedef std::set<TaskId> TaskIdSet;
typedef std::map<TaskId, TaskId> TaskIdMap;

class TaskList {
  public:
    TaskList();
    ~TaskList();
    void addTask(TaskPtr ptr);
    void removeTask(size_t index);
    size_t getIndex(Task * t) const;
    Task * getTask(size_t index) const;

    TaskIdSet getAgentTasks() const;
    TaskIdSet getAgentIOTasks() const;
    TaskId getInitIOTask() const;
    TaskId getFinIOTask() const;
    TaskIdSet getMessageBoardSyncTasks() const;
    TaskIdSet getMessageBoardClearTasks() const;
    std::string getTaskName(TaskId id) const;
    std::string getTaskAgentName(TaskId id) const;
    std::string getTaskFunctionName(TaskId id) const;
    StringSet getTaskReadOnlyVariables(TaskId id) const;
    StringSet getTaskWriteVariables(TaskId id) const;
    StringSet getTaskOutputMessages(TaskId id) const;
    StringSet getTaskInputMessages(TaskId id) const;

  //private:
    /*! \brief Ptr to vertex task so that mappings can be swapped */
    std::vector<TaskPtr> * vertex2task_;
};

}}  // namespace flame::model
#endif  // MODEL__TASKLIST_HPP_
