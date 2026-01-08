#pragma once
#include "GlobalMemory.hpp"
#include <new>
#include <cstddef>

template <typename T>
class Allocator {
    public:
        using value_type = T;

        Allocator() noexcept {}
        template <class U>Allocator(const Allocator<U>&)noexcept {}

        // allocate space for n objects of type T
        [[nodiscard]] T* allocate(std::size_t n){
            std::size_t bytes = n * sizeof(T);
            void* ptr = GlobalMemoryManager.allocate(bytes);
            if(!ptr)throw std::bad_alloc();
            return static_cast<T*>(ptr);
        }

        // dellocate space for n objects
        void deallocate(T* p, std::size_t)noexcept{
            GlobalMemoryManager.freeMem(static_cast<void*>(p));
        }

        // required by STL to support rebinding
        template <class U>
        struct rebind {using other = Allocator<U>; };
};

//STL allocator equality rules: all instance compare equal
template <class T,class U>
bool operator == (const Allocator<T>&, const Allocator<U>&){return true; }

template<class T, class U>
bool operator !=(const Allocator<T>&, const Allocator<U>&){return false; }