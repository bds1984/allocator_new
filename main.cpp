#include "allocator.h"



int main()
{
    std::map<int, double> m_map;
    my_container<int> m_ctr;
    std::map<int, double, std::less<int>, my_allocator<std::pair<const int, double>, 10>> m_map_my_alloc;
    my_container<int, my_allocator<int, 0>> m_ctr_my_alloc(10);

    for (int i = 0; i < 10; i++)
    {
        m_map_my_alloc.insert(std::make_pair(i, fact(i)));
        m_map.insert(std::make_pair(i, fact(i)));
    }
    int i = {};
    for (auto& a : m_ctr_my_alloc)
    {
        a = i;
        i++;
    }
    i = 0;
    for (auto& a : m_ctr)
    {
        a = i;
        i++;
    }

    std::cout << "Display containers\nmap with my allocator \n";
    for (auto& a : m_map_my_alloc)
    {
        std::cout << '\t' << a.first << " " << a.second << '\n';
    }
    std::cout << "my container with my allocator \n\t";
    for (auto& a : m_ctr_my_alloc)
    {
        std::cout << a << " ";
    }
    m_ctr_my_alloc.push_back(133);
    std::cout << "\nmy container with my allocator after push_back(133) \n\t";
    for (auto& a : m_ctr_my_alloc)
    {
        std::cout << a << " ";
    }

    return 0;
}
