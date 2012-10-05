/*!
 * \file src/model/task.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task: holds task information
 */
#include <cstdio>
#include <string>
#include <set>
#include "./task.hpp"

namespace flame { namespace model {

/*!
 * \brief Initialises Task
 *
 * Initialises Task and sets level to be zero.
 */
Task::Task(std::string parentName, std::string name, TaskType type)
    : parentName_(parentName), name_(name), taskType_(type) {
    level_ = 0;
    priorityLevel_ = 10;
    hasCondition_ = false;
    if (type == Task::sync_start) priorityLevel_ = 10;
    if (type == Task::sync_finish) priorityLevel_ = 1;
    if (type == Task::xfunction) priorityLevel_ = 5;
    if (type == Task::io_pop_write) priorityLevel_ = 0;
}

std::string Task::getTaskName() {
    std::string name;

    if (taskType_ == Task::xfunction) {
        name.append("AF_");
        name.append(parentName_);
    } else if (taskType_ == Task::xcondition) {
        name.append("AC_");
        name.append(parentName_);
    } else if (taskType_ == Task::io_pop_write) {
        name.append("AD_");
        name.append(parentName_);
    } else if (taskType_ == Task::start_model ||
            taskType_ == Task::finish_model) {
        name.append("MD_");
        name.append(parentName_);
    } else if (taskType_ == Task::xmessage) {
        name.append("MS");
    } else if (taskType_ == Task::sync_finish) {
        name.append("MF");
    }
    name.append("_");
    name.append(name_);

    return name;
}

void Task::setParentName(std::string parentName) {
    parentName_ = parentName;
}

std::string Task::getParentName() {
    return parentName_;
}

void Task::setName(std::string name) {
    name_ = name;
}

std::string Task::getName() {
    return name_;
}

void Task::setTaskType(TaskType type) {
    taskType_ = type;
}

Task::TaskType Task::getTaskType() {
    return taskType_;
}

void Task::setLevel(size_t level) {
    level_ = level;
}

size_t Task::getLevel() {
    return level_;
}

void Task::setPriorityLevel(size_t l) {
    priorityLevel_ = l;
}

size_t Task::getPriorityLevel() {
    return priorityLevel_;
}

void Task::setHasCondition(bool hasCondition) {
    hasCondition_ = hasCondition;
}

bool Task::hasCondition() {
    return hasCondition_;
}

void Task::addReadOnlyVariable(std::string name) {
    readOnlyVariables_.insert(name);
}

std::set<std::string>* Task::getReadOnlyVariables() {
    return &readOnlyVariables_;
}

void Task::addReadVariable(std::string name) {
    readVariables_.insert(name);
}

std::set<std::string>* Task::getReadVariables() {
    return &readVariables_;
}

void Task::addWriteVariable(std::string name) {
    writeVariables_.insert(name);
}

std::set<std::string>* Task::getWriteVariables() {
    return &writeVariables_;
}

VarMapToVertices * Task::getLastWrites() {
    return &lastWrites_;
}

std::set<size_t> * Task::getLastConditions() {
    return &lastConditions_;
}

}}  // namespace flame::model
