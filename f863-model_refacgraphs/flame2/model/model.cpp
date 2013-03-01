/*!
 * \file flame2/model/model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Model: model object for users to use
 */
#include <cstdio>
#include <string>
#include <map>
#include <set>
#include "flame2/config.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/exceptions/model.hpp"
#include "model.hpp"

namespace exe = flame::exe;

namespace flame {
namespace model {

Model::Model()
  : validated_(false) {}

Model::Model(std::string path_to_model) {
  // Load model
  flame::io::IOManager::GetInstance().loadModel(path_to_model, &model_);
  // Validate model
  validate();
}

void Model::registerAgentFunction(std::string name, exe::TaskFunction f_ptr) {
  funcMap_.insert(std::make_pair(name, f_ptr));
}

void Model::validate() {
  if (model_.validate() != 0) throw flame::exceptions::flame_model_exception(
      "Model could not be validated");
  validated_ = true;
}

void Model::addAgent(std::string name) {
  model_.addAgent(name);
  // model changed so not validated
  validated_ = false;
}

XMachine * Model::getAgent(std::string name) {
  // try and get named agent
  XMachine * agent = model_.getAgent(name);
  // if agent exists return
  if (agent) return agent;
  // if agent does not exist
  throw flame::exceptions::flame_model_exception("Agent does not exist");
}

void Model::addAgentVariable(std::string agent_name,
    std::string type, std::string name) {
  // get named agent
  XMachine * agent = getAgent(agent_name);
  // add variable
  agent->addVariable(type, name);
  // model changed so not validated
  validated_ = false;
}

void Model::addAgentFunction(std::string agent_name,
    const AgentFunction& agentFunction) {
  std::set<std::string>::iterator it;
  // get named agent
  XMachine * agent = getAgent(agent_name);
  // add function
  XFunction * func = agent->addFunction(agentFunction.getName(),
      agentFunction.getCurrentState(), agentFunction.getNextState());
  // for each input
  std::set<std::string> inputs = agentFunction.getInputs();
  for (it = inputs.begin(); it != inputs.end(); ++it) func->addInput(*it);
  // for each output
  std::set<std::string> outputs = agentFunction.getOutputs();
  for (it = outputs.begin(); it != outputs.end(); ++it) func->addOutput(*it);
  // for each read write variable
  std::set<std::string> rw = agentFunction.getReadWriteVariables();
  for (it = rw.begin(); it != rw.end(); ++it) func->addReadWriteVariable(*it);
  // for each read write variable
  std::set<std::string> ro = agentFunction.getReadOnlyVariables();
  for (it = ro.begin(); it != ro.end(); ++it) func->addReadOnlyVariable(*it);

  // model changed so not validated
  validated_ = false;
}

void Model::addMessage(std::string name) {
  model_.addMessage(name);
  // model changed so not validated
  validated_ = false;
}

void Model::addMessageVariable(std::string message_name,
    std::string type, std::string name) {
  // get named message
  XMessage * message = model_.getMessage(message_name);
  // add variable
  message->addVariable(type, name);
  // model changed so not validated
  validated_ = false;
}

exe::TaskFunction Model::getAgentFunctionPointer(std::string name) const {
  std::map<std::string, exe::TaskFunction>::const_iterator it;

  // Try and find function pointer from map
  it = funcMap_.find(name);
  if (it == funcMap_.end()) throw flame::exceptions::flame_model_exception(
      std::string("Agent function has not be registered: ").append(name));

  return (*it).second;
}

bool Model::isValidated() const {
  return validated_;
}

AgentMemory Model::getAgentMemoryInfo() const {
  return model_.getAgentMemoryInfo();
}

TaskList Model::getTasks() const {
  return model_.getTasks();
}

TaskIdMap Model::getTaskDependencies() const {
  return model_.getTaskDependencies();
}

/*TaskIdSet Model::getAgentTasks() const {
  return model_.getAgentTasks();
}

TaskIdSet Model::getAgentIOTasks() const {
  return model_.getAgentIOTasks();
}

TaskId Model::getInitIOTask() const {
  return model_.getInitIOTask();
}

TaskId Model::getFinIOTask() const {
  return model_.getFinIOTask();
}

TaskIdSet Model::getMessageBoardSyncTasks() const {
  return model_.getMessageBoardSyncTasks();
}
TaskIdSet Model::getMessageBoardClearTasks() const {
  return model_.getMessageBoardClearTasks();
}

std::string Model::getTaskName(TaskId id) const {
  return model_.getTaskName(id);
}

std::string Model::getTaskAgentName(TaskId id) const {
  return model_.getTaskAgentName(id);
}

std::string Model::getTaskFunctionName(TaskId id) const {
  return model_.getTaskFunctionName(id);
}

StringSet Model::getTaskReadOnlyVariables(TaskId id) const {
  return model_.getTaskReadOnlyVariables(id);
}

StringSet Model::getTaskWriteVariables(TaskId id) const {
  return model_.getTaskWriteVariables(id);
}

StringSet Model::getTaskOutputMessages(TaskId id) const {
  return model_.getTaskOutputMessages(id);
}

StringSet Model::getTaskInputMessages(TaskId id) const {
  return model_.getTaskInputMessages(id);
}*/

}}  // namespace flame::model
