#include <iostream>
#include <fstream>

#define std_uint std::size_t

namespace Dequeue {

    template <typename T>
    class Dequeue
    {
    public:
        void pushFront(T value);
        T popFront();
        void pushBack(T value);
        T popBack();
        Dequeue() = default;
        ~Dequeue() {
            if (buffer) {
                delete[] buffer;
            }
        }
    private:
        const std_uint DEFAULT_CAPACITY = 1;
        std_uint size = 0;
        std_uint capacity = DEFAULT_CAPACITY;
        T* buffer = new T[DEFAULT_CAPACITY];
        T* right = buffer + capacity;
        T* head = buffer;
        T* tail = buffer;

        void reallocBuffer(std_uint newCapacity);
        void incSize();
        void decSize();
        void incModal(T* &cur);
        void decModal(T* &cur);
    };

    template <typename T>
    void Dequeue<T>::pushFront(T value) {
        incSize();
        *head = value;
        decModal(head);
    }

    template <typename T>
    T Dequeue<T>::popFront() {
        if (size == 0) return -1;
        decSize();
        incModal(head);
        return *head;
    }

    template <typename T>
    void Dequeue<T>::pushBack(T value) {
        incSize();
        *tail = value;
        incModal(tail);
    }

    template <typename T>
    T Dequeue<T>::popBack() {
        if (size == 0) return -1;
        decSize();
        decModal(tail);
        return *tail;
    }

    template<typename T>
    void Dequeue<T>::reallocBuffer(std_uint newCapacity)
    {
        T* newBuffer = new T[newCapacity];
        incModal(head);
        decModal(tail);
        ++tail;
        T* newTail;
        if (head < tail) {
            newTail = std::copy(head, tail, newBuffer) - 1;
        } else {
            newTail = std::copy(head, right, newBuffer);
            newTail = std::copy(buffer, tail, newTail) - 1;
        }
        delete[] buffer;
        buffer = newBuffer;
        capacity = newCapacity;
        right = buffer + capacity;
        head = right - 1;
        tail = newTail;
        incModal(tail);
    }

    template<typename T>
    inline void Dequeue<T>::incSize() {
        if (++size > capacity) {
            reallocBuffer(capacity << 1);
        }
    }

    template<typename T>
    inline void Dequeue<T>::decSize() {
        if (--size < capacity >> 2) {
            reallocBuffer(capacity >> 2);
        }
    }

    template<typename T>
    inline void Dequeue<T>::incModal(T* &cur) {
        if (++cur == right)
            cur = buffer;
    }

    template<typename T>
    inline void Dequeue<T>::decModal(T* &cur) {
        if (--cur < buffer)
            cur = right - 1;
    }
}

int main() {
    std::ifstream in("input.txt");
    std::cin.rdbuf(in.rdbuf());
    int n;
    std::cin >> n;
    Dequeue::Dequeue<int> dequeue;
    int operation, value;
    for (int i = 0; i < n; i++) {
        std::cin >> operation >> value;
        switch (operation) {
        case 1:
            dequeue.pushFront(value);
            break;
        case 2:
            if (value != dequeue.popFront()) {
                std::cout << "NO" << std::endl;
                return EXIT_SUCCESS;
            }
            break;
        case 3:
            dequeue.pushBack(value);
            break;
        case 4:
            if (value != dequeue.popBack()) {
                std::cout << "NO" << std::endl;
                return EXIT_SUCCESS;
            }
            break;
        }
    }
    std::cout << "YES" << std::endl;
    return EXIT_SUCCESS;
}