//=======================================================================
// Copyright (c) 2014 Karol Wegrzycki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lsh_regression_basic_test.cpp
 * @brief lsh_regression binnary basic functionality
 * @author Karol Wegrzycki
 * @version 1.0
 * @date 2014-11-19
 */
#include "test_utils/logger.hpp"
#include "test_utils/get_test_dir.hpp"

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <iostream>
#include <string>
#include <stdio.h>


namespace {

std::string lsh_bin = get_build_dir("/bin/lsh-regression");
std::string temp = get_temp_dir();

}

BOOST_AUTO_TEST_SUITE(lsh_regression_bin_tests)


std::string exec(std::string cmd) {
    //TODO this is system specific function (not part of a standard) it should be removed
    FILE* pipe = popen((cmd + " >/dev/null 2>&1; echo -n $?").c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

inline void call(std::string command) {
    LOGLN("CALL: " << command);
    std::string result = exec(command);
    BOOST_CHECK_EQUAL(result, "0");
}

void call_fail(std::string command) {
    std::string result = exec(command);
    BOOST_CHECK(result != "0");
}

void test_files_are_equal(std::string file_1, std::string file_2) {
    // TODO test if files are equal with epsilons
    call("diff " + file_1 + " " + file_2);
}

std::string create_file(std::string file_name, std::string text) {
    // TODO use boost filesystem
    std::system(("echo \"" + text + "\" > " + temp + file_name).c_str());
    return temp + file_name;
}

BOOST_AUTO_TEST_CASE(lsh_bin_simple) {
    std::string training = create_file("training_1", "1 0:1\n0 1:1");
    std::string test = create_file("test_1", "1 0:1");
    std::string expt = create_file("expect_1", "1");
    std::string result = temp + "simple_test_1.svm";

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " --dimensions=2");
    test_files_are_equal(result, expt);
}

BOOST_AUTO_TEST_CASE(lsh_bin_test_formats) {
    std::string training = create_file("training_f", "  0   1:1e-6 2:1.5 4:1. 7:.5\n1\n0");
    std::string test = create_file("test_f", "0   1:1e-6 2:1.5 4:1. 7:.5\n 1 8:1");
    std::string result = temp + "result_test_f.svm";

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " --dimensions=9");
}

BOOST_AUTO_TEST_CASE(lsh_bin_test_metrics) {
    std::string training = create_file("training_l2", "0 0:1\n 1 0:10000\n 0 1:5");
    std::string test = create_file("test_l2", "0 0:3");
    std::string expect = create_file("expect_l2", "0");
    std::string result = temp + "result_test_l2.svm";

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " -m l2" + " --dimensions=1");
    test_files_are_equal(result, expect);
    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " -m l1" + " --dimensions=1");
    test_files_are_equal(result, expect);
}

BOOST_AUTO_TEST_CASE(lsh_bin_w_option) {
    std::string training = create_file("training_w", "0 0:1\n 1 0:10000");
    std::string test = create_file("test_w", "0 0:2");

    std::string result = temp + "result_w";

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " -m l2 -w 1" + " --dimensions=1");
    std::string expect05 = create_file("expect_w05", "0.5");
    test_files_are_equal(result, expect05);

    call(lsh_bin + " -d " + training + " -t " + test + " -o " + result + " -m l2 -w 10" + " --dimensions=1");
    std::string expect0 = create_file("expect_w0", "0");
    test_files_are_equal(result, expect0);
}

BOOST_AUTO_TEST_CASE(lsh_bin_bad_usage) {
    std::string example = create_file("example", "1 0:1");

    call_fail(lsh_bin + " -d " + example + " -t " + example);
    call_fail(lsh_bin + " -d " + example + " -t /dev/null");
    call_fail(lsh_bin + " -t " + example + " /dev/null");
    call_fail(lsh_bin + " -d " + example + " /dev/null");
    call_fail(lsh_bin + " -o /dev/null");
    call_fail(lsh_bin);

    call(lsh_bin + " --help");
}

BOOST_AUTO_TEST_CASE(lsh_bin_serialization) {
    std::string example = create_file("example", "1 0:1");
    std::string test = create_file("test", "0 0:3");
    std::string model(temp + "model.1"),
                model2(temp + "model.2"),
                res(temp + "res.txt");

    call(lsh_bin + " --model_out " + model + " -d " + example + " --dimensions=1");
    call(lsh_bin + " --model_in " + model + " -o " + res + " -t " + test);
    std::string expect = create_file("expect", "1");
    test_files_are_equal(res, expect);

    std::string example2 = create_file("example2", "1 0:1\n0 0:1\n0 0:1");

    call(lsh_bin + " --model_in " + model + " --model_out " + model2 + " -d " + example2);
    call(lsh_bin + " --model_in " + model2 + " -o " + res + " -t " + test);
    std::string expect2 = create_file("expect2", "0.5");
    test_files_are_equal(res, expect2);
}

BOOST_AUTO_TEST_SUITE_END()
