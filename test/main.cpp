#define BOOST_TEST_MODULE Bucketery test

#include <iostream>

#include <boost/test/included/unit_test.hpp>

#include "../bucketery/bucketery.h"

BOOST_AUTO_TEST_CASE(hole_manager_test)
{
	hole_manager hm;
	auto pos1 = hm.insert(100);
	auto pos2 = hm.insert(100);
	auto pos3 = hm.insert(100);

	hm.remove(pos2, 100);
	pos2 = hm.insert(100);
	BOOST_TEST(pos2 == 100);

	hm.remove(pos2, 100);
	hm.remove(pos3, 100);
	pos2 = hm.insert(100);
	BOOST_TEST(pos2 == 100);

	hm.remove(0, 100);
	hm.remove(100, 100);
	pos1 = hm.insert(100);
	BOOST_TEST(pos1 == 0);
}

BOOST_AUTO_TEST_CASE(do_pause)
{
	std::cout << "Press enter to exit" << std::endl;
	std::string sGot;
	std::getline(std::cin, sGot);
}