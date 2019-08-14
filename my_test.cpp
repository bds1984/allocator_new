#define BOOST_TEST_MODULE my_test

#include "allocator.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(my_test)

BOOST_AUTO_TEST_CASE(test_m_ctr)
{
my_container<int, my_allocator<int, 0>> m_ctr_test(10);
m_ctr_test.push_back(250);
BOOST_CHECK(m_ctr_test[10]==250);
}

BOOST_AUTO_TEST_CASE(test_m_ctr_2)
{
my_container<int, my_allocator<int, 0>> m_ctr_test_2(4,5);
BOOST_CHECK(m_ctr_test_2[2]==5);
}

BOOST_AUTO_TEST_CASE(test_m_ctr_3)
{
std::map<int, double, std::less<int>, my_allocator<std::pair<const int, double>, 10>> m_map_test;
  for (int i = 0; i < 10; i++)
    {
        m_map_test.insert(std::make_pair(i, fact(i)));
    }
  BOOST_CHECK( m_map_test[7]==5040);
}
BOOST_AUTO_TEST_SUITE_END()



  
