#define BOOST_TEST_MODULE Bucketery test

#include <iostream>

#include <boost/test/included/unit_test.hpp>

#include "../bucketery/bucketery.h"

/*BOOST_AUTO_TEST_CASE(hole_manager_test)
{
	hole_manager hm;
	auto pos1 = hm.insert(100);
	auto pos2 = hm.insert(100);
	auto pos3 = hm.insert(100);

	hm.remove(pos2, 100);
	auto new_pos = hm.insert(100);
	BOOST_TEST(pos2 == new_pos);

	hm.remove(pos2, 100);
	hm.remove(pos3, 100);
	new_pos = hm.insert(100);
	BOOST_TEST(pos2 == new_pos);

	hm.remove(pos2, 100);
	hm.remove(pos1, 100);
	new_pos = hm.insert(100);
	BOOST_TEST(pos1 == new_pos);
}*/

unsigned long long id1, id2;

std::string const test_phrase1 = "Some text to be stored";
std::string const test_phrase2 = "Some more text to be stored";

BOOST_AUTO_TEST_CASE(indexer_write)
{
	DeleteFileW(L"d:/useless.buf");

	storage s(L"d:/useless.buf");
	id1 = s.save_data(test_phrase1);
}

BOOST_AUTO_TEST_CASE(indexer_write_2)
{
	storage s(L"d:/useless.buf");
	id2 = s.save_data(test_phrase2);
}

BOOST_AUTO_TEST_CASE(indexer_read)
{
	storage s(L"d:/useless.buf");
	std::string read1, read2;
	s.load_data(id1, read1);
	s.load_data(id2, read2);

	BOOST_TEST(read1 == test_phrase1);
	BOOST_TEST(read2 == test_phrase2);
}

BOOST_AUTO_TEST_CASE(do_pause)
{
	std::cout << "Press enter to exit" << std::endl;
	std::string sGot;
	std::getline(std::cin, sGot);
}