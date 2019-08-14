#include <iostream>
#include <map>


template <typename T, int D>
struct my_allocator
{

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using syze_type = std::size_t;
    pointer tmp_ptr;
    pointer ptr_head;
    size_t Nalloc = {};
    size_t alloc_index = {};
    size_t del_counter = {};

    T* allocate(size_t n)
    {
        if (D == 0)
        {
            tmp_ptr = reinterpret_cast<T*>(malloc(n * sizeof(T)));
            ptr_head = tmp_ptr;
        }
        else
        {
            if (Nalloc == 0)
            {
                tmp_ptr = reinterpret_cast<T*>(malloc(D * n * sizeof(T)));
                ptr_head = tmp_ptr;
                Nalloc++;
            }
            else
            {
                alloc_index += sizeof(T);
                tmp_ptr += alloc_index;
            }
        }
        return tmp_ptr;
    }

    template <typename U>
    struct rebind
    {
        using other = my_allocator<U, D>;
    };
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        new (p) U(std::forward<Args>(args)...);
    }
    void destroy(pointer p)
    {
        p->~T();
    }
    void deallocate(pointer p, size_t n = {})
    {
        if (ptr_head)
        {
            std::free(ptr_head);
            ptr_head = nullptr;
        }
        p += n;
    }
};

template <class T, class Alloc = std::allocator<T>>
class my_container
{

    using Traits = std::allocator_traits<Alloc>;
    using myPtr = typename Traits::pointer;
    myPtr m_ptr = nullptr;
    Alloc my_allocator;
    size_t count_of_elements = {};
    using allocator_type = Alloc;

public:
    explicit my_container(size_t number_elem = {}, T value = {}, Alloc a = {})
        : my_allocator(a)
    {
        clear();
        this->emplace(number_elem, value);
    }
    ~my_container()
    {
        clear();
    }
    T& operator[](size_t i)
    {
        return *(m_ptr + i);
    }
    T* begin()
    {
        return m_ptr;
    }
    T* end()
    {
        return m_ptr + count_of_elements;
    }
    size_t size()
    {
        return count_of_elements;
    };
    void push_back(T&& value)
    {
        Alloc tmp_alocator;
        myPtr tmp_ptr;
        try
        {
            tmp_ptr = Traits::allocate(tmp_alocator, count_of_elements + 1);
        }
        catch (...)
        {
            throw;
        }
        if (tmp_ptr)
        { 
            for (size_t i = 0; i < count_of_elements; i++)
            {
                *(tmp_ptr + i) = *(m_ptr + i);
                Traits::destroy(my_allocator, static_cast<T*>(m_ptr + i));
            }
            Traits::deallocate(my_allocator, m_ptr, count_of_elements);
            tmp_ptr[count_of_elements] = value;
            count_of_elements++;
            m_ptr = tmp_ptr;
            my_allocator = tmp_alocator;
            tmp_ptr = nullptr;
        } 
    }

    template <class... Args>
    void emplace(size_t count, Args&&... args)
    { 
        try
        {
            m_ptr = Traits::allocate(my_allocator, count);
            T* raw_ptr = static_cast<T*>(m_ptr);
            if (count > 0)
            {
                for (size_t i = 0; i < count; i++)
                {
                    Traits::construct(
                        my_allocator, raw_ptr + count_of_elements, std::forward<Args>(args)...);
                    count_of_elements++;
                }
            }
        }
        catch (...)
        {
            Traits::deallocate(my_allocator, m_ptr, count);
            throw;
        }
    }
    void clear() noexcept
    {
        if (m_ptr)
        {
            T* raw_ptr = static_cast<T*>(m_ptr);
            size_t i = {};
            if (count_of_elements > 0)
            {
                while (i < count_of_elements)
                {
                    Traits::destroy(my_allocator, raw_ptr + i);
                    i++;
                }
            }

            Traits::deallocate(my_allocator, m_ptr, count_of_elements);
            m_ptr = nullptr;
        }
    }
};

double fact(int N)
{
    if (N < 0)
    {
        return 0;
    }
    if (N == 0)
    {
        return 1;
    }
    else
    {
        return N * fact(N - 1);
    }
}


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
