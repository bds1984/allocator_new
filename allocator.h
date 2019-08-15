#include <iostream>
#include <map>


double fact(int N);

template <typename T, int D>
struct my_allocator
{

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using syze_type = std::size_t;
    pointer tmp_ptr;
    pointer ptr_head;
    size_t Nalloc = {};//"флаг" первая аллокация или нет
    size_t alloc_index = {}; // порядковый номер элемента из первоначально аллоцированой памяти 


    T* allocate(size_t n)
    {
        if (D == 0)// стандартный функционал работы аллокатора
            //аллокатор каждый раз будет выполнять allocate и выделять столько памяти, сколько будет затребовано 
        {
            tmp_ptr = reinterpret_cast<T*>(malloc(n * sizeof(T)));
            ptr_head = tmp_ptr;
        }
        else // функционал работы аллокатора в соотвествии с условиями домашнего задания
        {
            if (Nalloc == 0)
            {//первая аллокация
                tmp_ptr = reinterpret_cast<T*>(malloc(D * n * sizeof(T)));
                ptr_head = tmp_ptr;
                Nalloc++;
            }
            else
            {//последующие запуски allocate
                alloc_index ++;
            }
        }
        return tmp_ptr+alloc_index;
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
    void deallocate(pointer, size_t)
    {
        if (ptr_head)
        {
            std::free(ptr_head);
            ptr_head = nullptr;
        }
    }
};

template <class T, class Alloc = std::allocator<T>>
class my_container
{

    using Traits = std::allocator_traits<Alloc>;
    using myPtr = typename Traits::pointer;
    myPtr m_ptr = nullptr;
    Alloc my_allocator;
    size_t count_of_elements = {};//счетчик элементов в контейнере
    using allocator_type = Alloc;

public:
    explicit my_container(size_t number_elem = {}, T value = {}, Alloc a = {})
        : my_allocator(a)
    {
        this->emplace(number_elem, value);
    }
    ~my_container()
    {
        clear();
    }
    T& operator[](size_t i)//доступ по []
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
    void push_back(T&& value) //добавление нового элемента, "аналог" push_back 
    {
        /*создается новый ресурс памяти, туда копируется содержимое старого ресурса, после этого 
        старый ресурс удаляется и указатели контейнера "переписываются" на новый*/
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
                try
                {
                Traits::destroy(my_allocator, static_cast<T*>(m_ptr + i));
                } 
                catch (...)
                {
                    throw;
                 }
            }
            try
            {
            Traits::deallocate(my_allocator, m_ptr, count_of_elements);
            }
             catch (...)
            {
                 throw;
            }
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

