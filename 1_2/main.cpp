#include <iostream>
#include <fstream>
#include <cstring>

#define std_uint std::size_t

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
    //std::cout << queue.pop_front() << std::endl;
    queue.push_back(5);
    //std::cout << queue.pop_front() << std::endl;
    //std::cout << queue.pop_front() << std::endl;
    //std::cout << queue.pop_front() << std::endl;
    //std::cout << queue.pop_front() << std::endl;
    queue.push_back(6);
    //std::cout << queue.pop_front() << std::endl;
    //std::cout << queue.pop_front() << std::endl;
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

int main() {
#ifdef DEBUG
    test();
    return EXIT_SUCCESS;
#endif
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