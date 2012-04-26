/*!
 * \file src/model/generate_task_list.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generates task list
 */
#include <string>
#include <vector>
#include <algorithm>
#include "./model_manager.hpp"
#include "./task.hpp"

namespace flame { namespace model {

int catalog_state_dependencies(XModel * model, std::vector<Task*> * tasks);
int catalog_communication_dependencies(XModel * model,
        std::vector<Task*> * tasks);
int catalog_data_dependencies(XModel * model,
        std::vector<Task*> * tasks);
int check_dependency_loops(XModel * model);
int calculate_dependencies(std::vector<Task*> * tasks);
int calculate_task_list(std::vector<Task*> * tasks);
void write_dependency_graph(std::string filename, std::vector<Task*> * tasks);

/*!
 * \brief Generates task list
 * \return The return code
 * Produces task list with state/data/communication dependencies.
 */
int ModelManager::generate_task_list() {
    int rc;
    /* Catalog state dependencies */
    rc = catalog_state_dependencies(&model_, &tasks_);
    /* Catalog communication dependencies */
    rc = catalog_communication_dependencies(&model_, &tasks_);
    /* Check for dependency loops */
    rc = check_dependency_loops(&model_);
    /* Calculate dependencies */
    rc = calculate_dependencies(&tasks_);
    /* Catalog data dependencies */
    rc = catalog_data_dependencies(&model_, &tasks_);
    /* Calculate task list using dependencies */
    rc = calculate_task_list(&tasks_);

#ifdef TESTBUILD
    /* Output dependency graph to view via graphviz dot */
    write_dependency_graph("dgraph.dot", &tasks_);
#endif

    return 0;
}

int catalog_state_dependencies(XModel * model, std::vector<Task*> * tasks) {
    std::vector<XMachine*>::iterator agent;
    std::vector<XFunction*>::iterator function;
    std::vector<XFunction*>::iterator function2;

    /* For each agent */
    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end(); ++agent) {
        /* For each function */
        for (function = (*agent)->getFunctions()->begin();
                function != (*agent)->getFunctions()->end(); ++function) {
            /* Add function as a task to the task list */
            Task * task = new Task;
            task->setParentName((*agent)->getName());
            task->setName((*function)->getName());
            task->setTaskType(Task::xfunction);
            tasks->push_back(task);
            /* Associate task with function */
            (*function)->setTask(task);
        }
    }

    /* For each agent */
    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end(); ++agent) {
        /* For each function */
        for (function = (*agent)->getFunctions()->begin();
                function != (*agent)->getFunctions()->end(); ++function) {
            /* Add state dependencies to tasks */
            /* For each transition functions start state
             * find transition functions that end in that state */
            for (function2 = (*agent)->getFunctions()->begin();
                    function2 != (*agent)->getFunctions()->end(); ++function2) {
                if ((*function)->getCurrentState() ==
                        (*function2)->getNextState()) {
                    /*(*function)->getTask()->addParent(
                            (*function)->getCurrentState(),
                            Dependency::state,
                            (*function2)->getTask());*/
                    Dependency * d = new Dependency;
                    d->setName((*function)->getCurrentState());
                    d->setDependencyType(Dependency::state);
                    d->setTask((*function2)->getTask());
                    (*function)->getTask()->addDependency(d);
                }
            }
        }
    }

    return 0;
}

int catalog_communication_dependencies(XModel * model,
        std::vector<Task*> * tasks) {
    std::vector<Task*>::iterator task;
    std::vector<XMessage*>::iterator message;
    std::vector<XMachine*>::iterator agent;
    std::vector<XFunction*>::iterator function;
    std::vector<XIOput*>::iterator ioput;

    /* Remove unused messages or
     * messages not read or
     * messages not sent and give warning. */

    /* Add sync_start and sync_finish for each message type */
    for (message = model->getMessages()->begin();
         message != model->getMessages()->end(); ++message) {
        /* Add sync start tasks to the task list */
        Task * syncStartTask = new Task;
        syncStartTask->setParentName((*message)->getName());
        syncStartTask->setName("sync_start");
        syncStartTask->setTaskType(Task::sync_start);
        tasks->push_back(syncStartTask);
        (*message)->setSyncStartTask(syncStartTask);
        /* Add sync finish tasks to the task list */
        Task * syncFinishTask = new Task;
        syncFinishTask->setParentName((*message)->getName());
        syncFinishTask->setName("sync_finish");
        syncFinishTask->setTaskType(Task::sync_finish);
        tasks->push_back(syncFinishTask);
        (*message)->setSyncFinishTask(syncFinishTask);
        /* Add dependency between start and finish sync tasks */
        syncFinishTask->addParent(
                (*message)->getName(),
                Dependency::communication,
                syncStartTask);
    }

    /* Find dependencies */
    /* For each agent */
    for (agent = model->getAgents()->begin();
         agent != model->getAgents()->end(); ++agent) {
        /* For each function */
        for (function = (*agent)->getFunctions()->begin();
             function != (*agent)->getFunctions()->end(); ++function) {
            /* Find outputting functions */
            for (ioput = (*function)->getOutputs()->begin();
                 ioput != (*function)->getOutputs()->end(); ++ioput) {
                /* Find associated messages */
                for (message = model->getMessages()->begin();
                     message != model->getMessages()->end(); ++message) {
                    if ((*message)->getName() == (*ioput)->getMessageName())
                        (*message)->getSyncStartTask()->addParent(
                                (*ioput)->getMessageName(),
                                Dependency::communication,
                                (*function)->getTask());
                }
            }

            /* Find inputting functions */
            for (ioput = (*function)->getInputs()->begin();
                 ioput != (*function)->getInputs()->end(); ++ioput) {
                /* Find associated messages */
                for (message = model->getMessages()->begin();
                        message != model->getMessages()->end(); ++message) {
                    if ((*message)->getName() == (*ioput)->getMessageName())
                        (*function)->getTask()->addParent(
                                (*ioput)->getMessageName(),
                                Dependency::communication,
                                (*message)->getSyncFinishTask());
                }
            }
        }
    }

    return 0;
}

/*!
 * \brief Catalogs data dependencies
 * \ingroup FLAME_MODEL
 * \param[in] model The FLAME model
 * \param[out] tasks The task list
 * \return Return error code
 * \todo .
 * \warning .
 *
 * For each agent memory variable add a task for writing the variable to disk.
 */
int catalog_data_dependencies(XModel * model,
        std::vector<Task*> * tasks) {
    std::vector<XMachine*>::iterator agent;
    std::vector<XVariable*>::iterator variable;
    std::vector<XVariable*>::iterator variableFind;
    std::vector<XFunction*>::iterator function;

    /* For each agent */
    for (agent = model->getAgents()->begin();
         agent != model->getAgents()->end(); ++agent) {
        for (variable = (*agent)->getVariables()->begin();
                variable != (*agent)->getVariables()->end(); ++variable) {
            /* Add variable to disk task */
            Task * dataTask = new Task;
            dataTask->setParentName((*agent)->getName());
            dataTask->setName((*variable)->getName());
            dataTask->setTaskType(Task::io_pop_write);
            tasks->push_back(dataTask);
            /* Add dependency parents to task */
            /* Find the last function that writes each variable */
            XFunction * lastFunction = 0;
            for (function = (*agent)->getFunctions()->begin();
                    function != (*agent)->getFunctions()->end(); ++function) {
                variableFind = std::find(
                        (*function)->getReadWriteVariables()->begin(),
                        (*function)->getReadWriteVariables()->end(),
                        (*variable));
                if (variableFind != (*function)->getReadWriteVariables()->end()
                        || lastFunction == 0) {
                    lastFunction = (*function);
                }
            }
            dataTask->addParent(
                    (*variable)->getName(),
                    Dependency::data,
                    lastFunction->getTask());
            dataTask->setLevel(lastFunction->getTask()->getLevel()+1);
        }
    }

    return 0;
}

int check_dependency_loops(XModel * model) {
    return 0;
}

std::string taskTypeToString(Task::TaskType t) {
    if (t == Task::io_pop_write) return "disk";
    else if (t == Task::sync_finish) return "comm";
    else if (t == Task::sync_start) return "comm";
    else if (t == Task::xfunction) return "func";
    else
        return "";
}

void printTaskList(std::string name, std::vector<Task*> * tasks) {
    std::vector<Task*>::iterator task;

    fprintf(stdout, "%s\n", name.c_str());
    for (task = tasks->begin(); task != tasks->end(); ++task) {
        fprintf(stdout, "%lu\t%s\t%s_%s\n",
                (*task)->getLevel(),
                taskTypeToString((*task)->getTaskType()).c_str(),
                (*task)->getParentName().c_str(),
                (*task)->getName().c_str());
    }
}

bool compare_task_levels(Task * i, Task * j) {
    return (i->getLevel() < j->getLevel());
}

int calculate_dependencies(std::vector<Task*> * tasks) {
    std::vector<Task*>::iterator task;
    size_t ii;

    /* Initialise task levels to be zero */
    for (task = tasks->begin(); task != tasks->end(); ++task) {
        (*task)->setLevel(0);
    }

    /* Calculate layers of dependency graph */
    /* This is achieved by finding functions with no dependencies */
    /* giving them a layer no, taking those functions away and doing
     * the operation again. */
    /* Boolean to track if all tasks have been leveled */
    bool finished = false;
    /* The current level that is being populated */
    size_t currentLevel = 1;
    /* Loop while tasks still being leveled */
    while (!finished) {
        finished = true;    /* Set finished to be true, until unleveled task */
        /* For every task */
        for (task = tasks->begin(); task != tasks->end(); ++task) {
            /* If task is not leveled */
            if ((*task)->getLevel() == 0) {
                /* Boolean to track if any dependencies
                 * still need to be leveled */
                bool unleveled_dependency = false;
                /* For each dependency */
                /* Didn't use iterator here as caused valgrind errors */
                for (ii = 0; ii < (*task)->getParents().size(); ii++) {
                    Dependency * dependency = (*task)->getParents().at(ii);
                    /* If the dependency is not leveled or just been leveled
                     * at the current level that is being populated */
                    if ((dependency)->getTask()->getLevel() == 0 ||
                        (dependency)->getTask()->getLevel() == currentLevel) {
                        /* Set that current task has an unleveled dependency */
                        unleveled_dependency = true;
                    }
                }
                /* If no unleveled dependencies */
                if (!unleveled_dependency) {
                    /* Add task to current level */
                    (*task)->setLevel(currentLevel);
                } else {
                    /* Else leveling has not finished */
                    finished = false;
                }
            }
        }
        /* Increment current level */
        currentLevel++;
    }

    return 0;
}

int calculate_task_list(std::vector<Task*> * tasks) {
    /* Sort the task list by level */
    sort(tasks->begin(), tasks->end(), compare_task_levels);

    printTaskList("tasks", tasks);

    return 0;
}

void write_dependency_graph(std::string filename, std::vector<Task*> * tasks) {
    /* File to write to */
    FILE *file;
    std::vector<Task*>::iterator task;
    size_t ii;

    /* print out the location of the source file */
    printf("Writing file : %s\n", filename.c_str());
    /* open the file to write to */
    file = fopen(filename.c_str(), "w");

    fputs("digraph dependency_graph {\n", file);
    fputs("\trankdir=BT;\n", file);
    fputs("\tsize=\"8,5;\"\n", file);
    fputs("\tnode [shape = rect];\n", file);

    fputs("\t\n\t/* Tasks */\n", file);
    /* For each task */
    for (task = tasks->begin(); task != tasks->end(); ++task) {
        fputs("\t", file);
        fputs((*task)->getParentName().c_str(), file);
        fputs("_", file);
        fputs((*task)->getName().c_str(), file);
        fputs("[label = \"", file);
        fputs((*task)->getParentName().c_str(), file);
        fputs("\\n", file);
        fputs((*task)->getName().c_str(), file);
        fputs("\"]\n", file);

        /* For every dependency */
        /* Didn't use iterator here as caused valgrind errors */
        for (ii = 0; ii < (*task)->getParents().size(); ii++) {
            Dependency * dependency = (*task)->getParents().at(ii);
            fputs("\t", file);
            fputs((*task)->getParentName().c_str(), file);
            fputs("_", file);
            fputs((*task)->getName().c_str(), file);
            fputs(" -> ", file);
            fputs(dependency->getTask()->getParentName().c_str(), file);
            fputs("_", file);
            fputs(dependency->getTask()->getName().c_str(), file);
            fputs(" [ label = \"<", file);
            if (dependency->getDependencyType() ==
                    Dependency::communication) {
                fputs("Message: ", file);
            }
            if (dependency->getDependencyType() ==
                    Dependency::data) {
                fputs("Memory: ", file);
            }
            if (dependency->getDependencyType() ==
                    Dependency::state) {
                fputs("State: ", file);
            }
            fputs(dependency->getName().c_str(), file);
            fputs(">\" ];\n", file);
        }
    }
    fputs("}", file);

    /* Close the file */
    fclose(file);
}

}}  // namespace flame::model
