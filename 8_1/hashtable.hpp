#pragma once
#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <string>
#include <vector>
#include <functional>

namespace made {

    namespace stl {
        using std::vector;
        using std::string;

        struct Hasher {
            unsigned int operator()(const std::string& str) const {
                unsigned int hash = 0;
                for (unsigned int i = 0; i < str.length(); i++)
                    hash = hash * 2047 + str[i];
                return hash;
            }
        };

        template<class T, class H>
        class HashTable {
            using uint = unsigned int;
            static const uint INITIAL_SIZE = 8;
        public:
            HashTable(const H& _hasher) :
                hasher_(_hasher), table_size_(INITIAL_SIZE), table_(INITIAL_SIZE, nullptr), key_count_(0), deleted_key_count_(0) {};
            ~HashTable() { for (auto elem : table_) delete elem; }

            HashTable(const HashTable& t) = delete;
            HashTable& operator=(const HashTable& t) = delete;

            bool Add(const T& key);
            bool Has(const T& key);
            bool Delete(const T& key);
        private:
            struct Element {
                T key;
                bool deleted = false;
                Element(const T& _key) : key(_key) {}
            };
            typedef std::function<void(Element*)> SeekEvent;

            H hasher_;
            size_t table_size_;
            vector<Element*> table_;
            uint key_count_;
            uint deleted_key_count_;

            bool Seek(const T& key, SeekEvent&& cb);
            void GrowTable();
            uint Probe(uint hash, uint i);
        };

#pragma region HashTable
        template<class T, class H>
        bool HashTable<T, H>::Add(const T& key) {
            if (key_count_ > table_size_ - (table_size_ >> 2)) {
                GrowTable();
            }

            uint j = hasher_(key) % table_size_;
            Element** elem = &table_[j];
            Element** deleted_elem = nullptr;
            for (uint i = 0; *elem && (i < table_size_); ++i) {
                if ((*elem)->deleted)
                    deleted_elem = deleted_elem ? deleted_elem : elem;
                else if ((*elem)->key == key)
                    return false;
                j = Probe(j, i);
                elem = &table_[j];
            }

            if (deleted_elem) {
                --deleted_key_count_;
                delete *deleted_elem;
                elem = deleted_elem;
            }
            else {
                ++key_count_;
            }
            *elem = new Element(key);
            return true;
        }

        template<class T, class H>
        inline bool HashTable<T, H>::Has(const T& key) {
            return Seek(key, [](Element* elem) {});
        }

        template<class T, class H>
        inline bool HashTable<T, H>::Delete(const T& key) {
            return Seek(key, [this](Element* elem) {
                elem->deleted = true;
                ++deleted_key_count_;
            });
        }

        template<class T, class H>
        bool HashTable<T, H>::Seek(const T& key, SeekEvent&& cb) {
            uint index = hasher_(key) % table_size_;
            Element* elem = table_[index];
            for (uint i = 0; elem && (i < table_size_); ++i) {
                if (!elem->deleted && (elem->key == key)) {
                    cb(elem);
                    return true;
                }
                index = Probe(index, i);
                elem = table_[index];
            }
            return false;
        }

        template<class T, class H>
        void HashTable<T, H>::GrowTable() {
            assert(key_count_ >= deleted_key_count_);
            uint actual_key_count = key_count_ - deleted_key_count_;
            assert(table_size_ < SIZE_MAX >> 1);
            table_size_ = table_size_ << 1;
            // Little optimization : resizing can make result table_ smaller if we had too many deleted keys
            // actual_key_count < table_size_ * 0.375
            while ((actual_key_count < ((table_size_ >> 1) - (table_size_ >> 3))) && (table_size_ > INITIAL_SIZE)) {
                table_size_ /= 2;
            }

            vector<Element*> newTable = std::move(table_);
            table_ = vector<Element*>(table_size_, nullptr);
            key_count_ = 0;
            deleted_key_count_ = 0;
            for (auto elem : newTable) {
                if (elem && !elem->deleted) {
                    Add(elem->key);
                }
                delete elem;
            }
        }

        template<class T, class H>
        inline unsigned int HashTable<T, H>::Probe(uint index, uint i) {
            return (index + i + 1) % table_size_;
        }
#pragma endregion HashTable

    }
}
#endif // !HASH_TABLE_H_