#define BOOST_TEST_MODULE Bucketery test

#include <iostream>

#include <boost/test/included/unit_test.hpp>

#include "../bucketery/bucketery.h"

unsigned long long id1, id2;

std::string const test_phrase1 = "Some text to be stored";
std::string const test_phrase2 = "Some more text to be stored";
std::string const test_phrase3 = "Even more text to be stored and verified. This entry is particualrily interesting to see how larger data is being recorded";

BOOST_AUTO_TEST_CASE(indexer_complete)
{
	DeleteFileW(L"d:/useless.buf");

	auto s = std::make_unique<storage>(L"d:/useless.buf");
	s->save_data(0, test_phrase1);

	std::string read, read1, read2, read3;
	s->load_data(0, read);
	BOOST_ASSERT(read == test_phrase1);

	s.reset();

	s = std::make_unique<storage>(L"d:/useless.buf");
	read = "";
	s->load_data(0, read);
	BOOST_ASSERT(read == test_phrase1);

	s->save_data(0, test_phrase2);
	s->load_data(0, read);
	BOOST_ASSERT(read == test_phrase2);

	s.reset();

	s = std::make_unique<storage>(L"d:/useless.buf");
	read = "";
	s->load_data(0, read);
	BOOST_ASSERT(read == test_phrase2);

	s->save_data(0, test_phrase1);
	s->save_data(2, test_phrase3);
	s->save_data(1, test_phrase2);
	s.reset();

	s = std::make_unique<storage>(L"d:/useless.buf");
	s->load_data(1, read1);
	s->load_data(2, read2);
	s->load_data(3, read3);
	BOOST_ASSERT(read1 == test_phrase1);
	BOOST_ASSERT(read2 == test_phrase2);
	BOOST_ASSERT(read3 == test_phrase3);
}

/*BOOST_AUTO_TEST_CASE(indexer_complete)
{
	DeleteFileW(L"d:/useless.buf");

	storage s(L"d:/useless.buf");
	id1 = s.save_data(test_phrase1);
	id2 = s.save_data(test_phrase2);
	std::string read1, read2;
	s.load_data(id1, read1);
	s.load_data(id2, read2);

	BOOST_TEST(read1 == test_phrase1);
	BOOST_TEST(read2 == test_phrase2);
}

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
}*/

BOOST_AUTO_TEST_CASE(do_pause)
{
	std::cout << "Press enter to exit" << std::endl;
	std::string sGot;
	std::getline(std::cin, sGot);
}