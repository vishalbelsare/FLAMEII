/*!
 * \file src/io/tests/test_io_xml_model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the io xml model method
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE IO testcases
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../io_xml_model.hpp"

namespace io = flame::io;

BOOST_AUTO_TEST_SUITE(IOModule)

/* Test the reading of XML model files and sub model files. */
BOOST_AUTO_TEST_CASE(test_read_XML_model) {
    int rc;
    io::IOXMLModel ioxmlmodel;
    io::XModel model;

    rc = ioxmlmodel.readXMLModel("tests/models/missing.xml", &model);
    BOOST_CHECK(rc == 1);

    rc = ioxmlmodel.readXMLModel("tests/models/malformed_xml.xml", &model);
    BOOST_CHECK(rc == 2);

    rc = ioxmlmodel.readXMLModel("tests/models/not_xmodel.xml", &model);
    BOOST_CHECK(rc == 3);

    rc = ioxmlmodel.readXMLModel("tests/models/xmodelv1.xml", &model);
    BOOST_CHECK(rc == 4);

    rc = ioxmlmodel.readXMLModel(
            "tests/models/submodel_enable_error.xml", &model);
    BOOST_CHECK(rc == 5);

    rc = ioxmlmodel.readXMLModel(
            "tests/models/submodel_end_not_xml.xml", &model);
    BOOST_CHECK(rc == 6);

    rc = ioxmlmodel.readXMLModel("tests/models/submodel_duplicate.xml", &model);
    BOOST_CHECK(rc == 7);

    rc = ioxmlmodel.readXMLModel("tests/models/submodel_missing.xml", &model);
    BOOST_CHECK(rc == 8);

    rc = ioxmlmodel.readXMLModel("tests/models/all_not_valid.xml", &model);
    BOOST_CHECK(rc == 0);
}

BOOST_AUTO_TEST_CASE(validate_model) {
    int rc;
    io::IOXMLModel ioxmlmodel;
    io::XModel model;

    /* Test model validation using single file with all errors.
     * Possibly in future use a correct file and mutate for each
     * error and test separately. */
    ioxmlmodel.readXMLModel("tests/models/all_not_valid.xml", &model);
    rc = model.validate();
    BOOST_CHECK(rc == 41);
}

BOOST_AUTO_TEST_SUITE_END()
