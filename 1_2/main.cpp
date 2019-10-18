#include <iostream>
#include <fstream>
#include <cstring>

#define std_uint std::size_t
#ifdef DEBUG
    #define PRINT_BUFFER printBuffer();
#else
    #define PRINT_BUFFER ((void)0);
#endif


namespace Dequeue {

    template <typename T>
    class Dequeue
    {
    public:
        void push_front(T value);
        T pop_front();
        void push_back(T value);
        T pop_back();
        Dequeue() = default;
        ~Dequeue() {
            if (buffer) {
                delete[] buffer;
            }
        }
    private:
        const std_uint DEFAULT_CAPACITY = 1;
        T* buffer = new T[DEFAULT_CAPACITY];
        std_uint head = 0;
        std_uint tail = 0;
        std_uint size = 0;
        std_uint capacity = DEFAULT_CAPACITY;
        void reallocBuffer(std_uint newCapacity);
        void incSize();
        void decSize();
        void incModal(std_uint &value);
        void decModal(std_uint &value);
        void printBuffer();
    };

    template <typename T>
    void Dequeue<T>::push_front(T value) {
        incSize();
        decModal(head);
        buffer[head] = value;
#ifdef DEBUG
        printBuffer();
#endif
    }

    template <typename T>
    T Dequeue<T>::pop_back() {
        if (size == 0) return -1;
        decSize();
        decModal(tail);
        T result = buffer[tail];
#ifdef DEBUG
        printBuffer();
#endif
        return result;
    }

    template <typename T>
    void Dequeue<T>::push_back(T value) {
        incSize();
        buffer[tail] = value;
        incModal(tail);
#ifdef DEBUG
        printBuffer();
#endif
    }

    template <typename T>
    T Dequeue<T>::pop_front() {
        if (size == 0) return -1;
        decSize();
        T result = buffer[head];
        incModal(head);
#ifdef DEBUG
        printBuffer();
#endif
        return result;
    }

    template<typename T>
    void Dequeue<T>::reallocBuffer(std_uint newCapacity)
    {
        T* tempBuffer = new T[newCapacity];
#ifdef DEBUG
        for (std_uint i = 0; i < newCapacity; i++)
            tempBuffer[i] = 0;
#endif
        for (std_uint i = 0; i < size; i++) {
            if (head == capacity)
                head = 0;
            tempBuffer[i] = buffer[head];
            head++;
        }
        head = 0;
        tail = size % newCapacity;
        delete[] buffer;
        buffer = tempBuffer;
        capacity = newCapacity;
    }

    template<typename T>
    inline void Dequeue<T>::incSize() {
        if (size + 1 > capacity) {
            if (tail == 0) {
                tail = capacity;
            }
            reallocBuffer(capacity << 1);
        }
        size++;
    }

    template<typename T>
    inline void Dequeue<T>::decSize() {
        if (size - 1 < capacity / 4) {
            reallocBuffer(capacity >> 2);
        }
        size--;
    }

    template<typename T>
    inline void Dequeue<T>::incModal(std_uint &value) {
        value = ++value % capacity;
    }

    template<typename T>
    inline void Dequeue<T>::decModal(std_uint &value) {
        value = (--value + capacity) % capacity;
    }

    template<typename T>
    inline void Dequeue<T>::printBuffer() {
        for (std_uint i = 0; i < capacity; i++) {
            if (i == head)
                std::cout << "h";
            std::cout << buffer[i];
            if (i == tail)
                std::cout << "t";
            std::cout << "\t";
        }
        std::cout << std::endl;
    }
}

namespace PDequeue {

    template <typename T>
    class Dequeue
    {
    public:
        void push_front(T value);
        T pop_front();
        void push_back(T value);
        T pop_back();
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
        //T* left = buffer;
        T* right = buffer + capacity;
        // experimental
        //T* head = buffer;
        T* head = buffer;
        T* tail = buffer;

        void reallocBuffer(std_uint newCapacity);
        void incSize();
        void decSize();
        //void incModal(std_uint &value);
        //void decModal(std_uint &value);
        void incModal(T* &cur);
        void decModal(T* &cur);
        void printBuffer();
    };

    template <typename T>
    void Dequeue<T>::push_front(T value) {
        incSize();
        // experimental
        //decModal(head);
        *head = value;
        decModal(head);
        PRINT_BUFFER
    }

    template <typename T>
    T Dequeue<T>::pop_back() {
        if (size == 0) return -1;
        decSize();
        decModal(tail);
        PRINT_BUFFER
        return *tail;
    }

    template <typename T>
    void Dequeue<T>::push_back(T value) {
        incSize();
        *tail = value;
        incModal(tail);
        PRINT_BUFFER
    }

    template <typename T>
    T Dequeue<T>::pop_front() {
        if (size == 0) return -1;
        decSize();
        // experimental
        //T result = *head;
        incModal(head);
        PRINT_BUFFER
        //return result;
        return *head;
    }

    template<typename T>
    void Dequeue<T>::reallocBuffer(std_uint newCapacity)
    {
        T* newBuffer = new T[newCapacity];
#ifdef DEBUG
        for (std_uint i = 0; i < newCapacity; i++)
            newBuffer[i] = 0;
#endif
        //for (std_uint i = 0; i < size; i++) {
        //    if (head == capacity)
        //        head = 0;
        //    tempBuffer[i] = buffer[head];
        //    head++;
        //}
        //head = 0;
        //tail = size % newCapacity;
        T* newRight = newBuffer + newCapacity;
        incModal(head);
        decModal(tail);
        T* newHead;
        T* newTail;
        if (head <= tail) {
            //newHead = head - buffer + newBuffer;
            newHead = newBuffer;
            newTail = tail - head + newHead;
            for (T* cur = newHead; head <= tail; head++, cur++) {
                *cur = *head;
            }
        } else {
            newHead = newBuffer;
            newTail = newHead;
            for (; head < right; head++, newTail++) {
                *newTail = *head;
            }
            for (head = buffer; head <= tail; head++, newTail++) {
                *newTail = *head;
            }
            newTail--;
        }
        right = newRight;
        delete[] buffer;
        buffer = newBuffer;
        capacity = newCapacity;
        head = newHead;
        tail = newTail;//= T* cur
        decModal(head);
        incModal(tail);
    }

    template<typename T>
    inline void Dequeue<T>::incSize() {
        if (size + 1 > capacity) {
            //if (tail == buffer) {
            //    tail = right;
            //}
            reallocBuffer(capacity << 1);
        }
        size++;
    }

    template<typename T>
    inline void Dequeue<T>::decSize() {
        if (size - 1 < capacity / 4) {
            reallocBuffer(capacity >> 2);
        }
        size--;
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

    template<typename T>
    inline void Dequeue<T>::printBuffer() {
        for (T* p = buffer; p < right; p++) {
            if (p == head)
                std::cout << "h";
            std::cout << *p;
            if (p == tail)
                std::cout << "t";
            std::cout << "\t";
        }
        std::cout << std::endl;
    }
}

void test() {
    Dequeue::Dequeue<int> queue;
    queue.push_back(1);
    queue.push_back(2);
    queue.push_back(3);
    queue.push_back(4);
    queue.push_back(18);
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
    queue.push_back(5);
    queue.push_back(6);

    queue.push_back(7);
    //std::cout << queue.pop_front() << std::endl;
    queue.push_back(8);
    //std::cout << queue.pop_front() << std::endl;
    queue.push_back(9);
    std::cout << queue.pop_front() << std::endl;
    queue.push_back(10);
    std::cout << queue.pop_front() << std::endl;
    queue.push_back(11);
    std::cout << queue.pop_front() << std::endl;
    queue.push_back(12);
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
}
void test2() {
    PDequeue::Dequeue<int> queue;
    queue.push_back(1);
    queue.push_back(2);
    queue.push_back(3);
    queue.push_back(4);
    queue.push_back(18);
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
    queue.push_back(5);
    queue.push_back(6);

    queue.push_back(7);
    //std::cout << queue.pop_front() << std::endl;
    queue.push_back(8);
    //std::cout << queue.pop_front() << std::endl;
    queue.push_back(9);
    std::cout << queue.pop_front() << std::endl;
    queue.push_back(10);
    std::cout << queue.pop_front() << std::endl;
    queue.push_back(11);
    std::cout << queue.pop_front() << std::endl;
    queue.push_back(12);
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
    std::cout << queue.pop_front() << std::endl;
}
void test3() {
    PDequeue::Dequeue<int> queue;
    queue.push_back(1);
    queue.push_back(2);
    queue.push_back(3);
    queue.push_back(4);

}
int main() {
#ifdef DEBUG
    test();
    test2();
    return EXIT_SUCCESS;
#endif
    std::ifstream in("input.txt");
    std::cin.rdbuf(in.rdbuf());
    int n;
    std::cin >> n;
    PDequeue::Dequeue<int> dequeue;
    int operation, value;
    for (int i = 0; i < n; i++) {
        std::cin >> operation >> value;
        switch (operation) {
        case 1:
            dequeue.push_front(value);
            break;
        case 2:
            if (value != dequeue.pop_front()) {
                std::cout << "NO" << std::endl;
                return EXIT_SUCCESS;
            }
            break;
        case 3:
            dequeue.push_back(value);
            break;
        case 4:
            if (value != dequeue.pop_back()) {
                std::cout << "NO" << std::endl;
                return EXIT_SUCCESS;
            }
            break;
        }
    }
    std::cout << "YES" << std::endl;
    return EXIT_SUCCESS;
}