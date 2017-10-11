#define BOOST_TEST_MODULE Bucketery test

#include <iostream>

#include <boost/test/included/unit_test.hpp>

#include "../bucketery/bucketery.h"

BOOST_AUTO_TEST_CASE(hole_manager_test)
{
	int i = 1;
	BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(do_pause)
{
	std::cout << "Press enter to exit" << std::endl;
	std::string sGot;
	std::getline(std::cin, sGot);
}