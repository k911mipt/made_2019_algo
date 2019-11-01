#ifndef COMMON_H_
#define COMMON_H_

namespace made {

    namespace task {

        typedef size_t size_type;

        struct WindowElement {
            size_type index;
            int value;
            friend bool operator == (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value == rhs.value; }
            friend bool operator < (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value < rhs.value; }
            friend bool operator <= (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value <= rhs.value; }
            friend bool operator > (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value > rhs.value; }
            friend bool operator >= (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value >= rhs.value; }
        };

        class lessWindowElement
        {
        public:
            bool operator() (const WindowElement& lhs, const WindowElement& rhs) { return lhs.value < rhs.value; }
        };
    }
}

#endif // COMMON_H_