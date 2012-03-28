/*!
 * \file src/io/xcondition.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XCondition: holds condition/filter information
 */
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <string>
#include "./xcondition.hpp"
#include "./xmachine.hpp"
#include "./xmessage.hpp"
#include "./xmodel.hpp"

namespace flame { namespace io {

XCondition::XCondition() {
    isNot = false;
    isTime = false;
    isValues = false;
    isConditions = false;
    lhsCondition = 0;
    rhsCondition = 0;
    lhsIsAgentVariable = false;
    rhsIsAgentVariable = false;
    lhsIsMessageVariable = false;
    rhsIsMessageVariable = false;
    lhsIsValue = false;
    rhsIsValue = false;
    lhsIsCondition = false;
    rhsIsCondition = false;
    timePeriod = "";
    timePhaseVariable = "";
    timeDurationString = "";
    timeDuration = 0;
    foundTimeDuration = false;
    tempValue = "";
    lhs = "";
    op = "";
    rhs = "";
    lhsDouble = 0.0;
    rhsDouble = 0.0;
}

XCondition::~XCondition() {
    /* Delete any nested conditions */
    if (lhsCondition != 0) delete lhsCondition;
    if (rhsCondition != 0) delete rhsCondition;
}

void XCondition::print() {
    std::fprintf(stdout, "\t\t");
    if (isNot) {
        std::fprintf(stdout, "not(");
    }
    if (isValues) {
        if (lhsIsAgentVariable) {
            std::fprintf(stdout, "a.%s", lhs.c_str());
        } else if (lhsIsMessageVariable) {
            std::fprintf(stdout, "m.%s", lhs.c_str());
        } else if (lhsIsValue) {
            std::fprintf(stdout, "%f", lhsDouble);
        }
        std::fprintf(stdout, " ");
        std::fprintf(stdout, "%s", op.c_str());
        std::fprintf(stdout, " ");
        if (rhsIsAgentVariable) {
            std::fprintf(stdout, "a.%s", rhs.c_str());
        } else if (rhsIsMessageVariable) {
            std::fprintf(stdout, "m.%s", rhs.c_str());
        } else if (rhsIsValue) {
            std::fprintf(stdout, "%f", rhsDouble);
        }
    }
    if (isConditions) {
        std::fprintf(stdout, "(");
        std::fprintf(stdout, "\n");
        lhsCondition->print();
        std::fprintf(stdout, "\t\t");
        std::fprintf(stdout, ") ");
        std::fprintf(stdout, "%s", op.c_str());
        std::fprintf(stdout, " (");
        std::fprintf(stdout, "\n");
        rhsCondition->print();
        std::fprintf(stdout, "\t\t");
        std::fprintf(stdout, ")");
    }
    if (isTime) {
        std::fprintf(stdout, "time(");
        std::fprintf(stdout, "%s", timePeriod.c_str());
        std::fprintf(stdout, ", ");
        if (timePhaseIsVariable) std::fprintf(stdout, "a.%s",
                timePhaseVariable.c_str());
        else
            std::fprintf(stdout, "%d", timePhaseValue);
        if (foundTimeDuration) {
            std::fprintf(stdout, ", ");
            std::fprintf(stdout, "%d", timeDuration);
        }
        std::fprintf(stdout, ")");
    }
    if (isNot) {
        std::fprintf(stdout, ")");
    }
    std::fprintf(stdout, "\n");
}

/*!
 * \brief Processes symbols in conditions/filters
 * \return Number of errors
 * Handles agent/message variables and numbers for values and handles operators.
 */
int XCondition::processSymbols() {
    int rc, errors = 0;

    if (isTime) {
        if (boost::starts_with(timePhaseVariable, "a.")) {
            timePhaseVariable.erase(0, 2);
            timePhaseIsVariable = true;
        } else {
            timePhaseIsVariable = false;
            try {
                timePhaseValue = boost::lexical_cast<int>(timePhaseVariable);
            } catch(const boost::bad_lexical_cast& E) {
                std::fprintf(stderr,
                    "Error: Cannot cast time phase to an integer: %s\n",
                    timePhaseVariable.c_str());
                errors++;
            }
        }
        if (foundTimeDuration) {
            /* Cast time duration from read in string to integer */
            try {
                timeDuration = boost::lexical_cast<int>(timeDurationString);
            } catch(const boost::bad_lexical_cast& E) {
                std::fprintf(stderr,
                    "Error: Cannot cast time duration to an integer: %s\n",
                    timeDurationString.c_str());
                errors++;
            }
        }
    } else {
        /* Check lhs and rhs are both values or both conditions else error */
        if (lhsIsValue && rhsIsValue) {
            isValues = true;
            if (boost::starts_with(lhs, "a.")) {
                lhsIsAgentVariable = true;
                lhsIsValue = false;
                lhs.erase(0, 2);
            } else if (boost::starts_with(lhs, "m.")) {
                lhsIsMessageVariable = true;
                lhsIsValue = false;
                lhs.erase(0, 2);
            } else {
                lhsIsValue = true;
                try {
                    lhsDouble = boost::lexical_cast<double>(lhs);
                } catch(const boost::bad_lexical_cast& E) {
                    std::fprintf(stderr,
                "Error: Condition/filter value not variable or number: %s\n",
                        lhs.c_str());
                    errors++;
                }
            }

            if (boost::starts_with(rhs, "a.")) {
                rhsIsAgentVariable = true;
                rhsIsValue = false;
                rhs.erase(0, 2);
            } else if (boost::starts_with(rhs, "m.")) {
                rhsIsMessageVariable = true;
                rhsIsValue = false;
                rhs.erase(0, 2);
            } else {
                rhsIsValue = true;
                try {
                    rhsDouble = boost::lexical_cast<double>(rhs);
                } catch(const boost::bad_lexical_cast& E) {
                    std::fprintf(stderr,
                "Error: Condition/filter value not variable or number: %s\n",
                        rhs.c_str());
                    errors++;
                }
            }

            if (op == "EQ") {
                op = "==";
            } else if (op == "NEQ") {
                op = "!=";
            } else if (op == "LEQ") {
                op = "<=";
            } else if (op == "GEQ") {
                op = ">=";
            } else if (op == "LT") {
                op = "<";
            } else if (op == "GT") {
                op = ">";
            } else {
                std::fprintf(stderr,
                    "Error: Condition/filter op value not recognised: %s\n",
                    op.c_str());
                errors++;
            }
        } else if (lhsIsCondition && rhsIsCondition) {
            isConditions = true;
            rc = lhsCondition->processSymbols();
            errors += rc;
            rc = rhsCondition->processSymbols();
            errors += rc;

            if (op == "AND") {
                op = "&&";
            } else if (op == "OR") {
                op = "||";
            } else {
                std::fprintf(stderr,
                    "Error: Condition/filter op value not recognised: %s\n",
                    op.c_str());
                errors++;
            }
        } else {
            std::fprintf(stderr,
        "Error: lhs and rhs are not both values or both nested conditions\n");
            errors++;
        }
    }

    return errors;
}

int XCondition::validate(XMachine * agent, XMessage * xmessage,
        XModel * model) {
    int rc, errors = 0;
    unsigned int ii;

    if (isTime) {
        /* Check time period is valid time unit */
        bool validPeriod = false;
        for (ii = 0; ii < model->getTimeUnits()->size(); ii++) {
            if (timePeriod == model->getTimeUnits()->at(ii)->getName())
                validPeriod = true;
        }
        if (!validPeriod) {
            std::fprintf(stderr,
                "Error: time period is not a valid time unit: '%s',\n",
                timePeriod.c_str());
            errors++;
        }
        /* If time phase is an agent variable then validate it */
        if (timePhaseIsVariable) {
            if (!agent->validateVariableName(timePhaseVariable)) {
                std::fprintf(stderr,
        "Error: time phase variable is not a valid agent variable: '%s',\n",
                    timePhaseVariable.c_str());
                errors++;
            }
        }
    } else if (isValues) {
        /* If values validate any agent or message variables */
        if (lhsIsAgentVariable) {
            if (!agent->validateVariableName(lhs)) {
                std::fprintf(stderr,
                    "Error: lhs is not a valid agent variable: '%s',\n",
                    lhs.c_str());
                errors++;
            }
        } else if (lhsIsMessageVariable) {
            if (xmessage != 0) {
                if (!xmessage->validateVariableName(lhs)) {
                    std::fprintf(stderr,
                        "Error: lhs is not a valid message variable: '%s',\n",
                        lhs.c_str());
                    errors++;
                }
            } else {
                std::fprintf(stderr,
        "Error: cannot validate lhs as the message type is invalid: '%s',\n",
                    lhs.c_str());
                errors++;
            }
        }
        if (rhsIsAgentVariable) {
            if (!agent->validateVariableName(rhs)) {
                std::fprintf(stderr,
                    "Error: rhs is not a valid agent variable: '%s',\n",
                    rhs.c_str());
                errors++;
            }
        } else if (rhsIsMessageVariable) {
            if (xmessage != 0) {
                if (!xmessage->validateVariableName(rhs)) {
                    std::fprintf(stderr,
                        "Error: rhs is not a valid message variable: '%s',\n",
                        rhs.c_str());
                    errors++;
                }
            } else {
                std::fprintf(stderr,
        "Error: cannot validate rhs as the message type is invalid: '%s',\n",
                    rhs.c_str());
                errors++;
            }
        }
    } else if (isConditions) {
        /* If nested conditions validate them */
        rc = lhsCondition->validate(agent, xmessage, model);
        errors += rc;
        rc = rhsCondition->validate(agent, xmessage, model);
        errors += rc;
    }

    return errors;
}

}}  // namespace flame::io
