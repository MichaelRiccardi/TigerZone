#ifndef __ARRAY_H
#define __ARRAY_H

#include <stdlib.h>
#include <algorithm>

template <typename T> class Array {
    public:
        Array() {
            this->array = NULL;
            this->size = 0;
        };

        Array(unsigned int size) {
            this->size = size;
            this->array = (T*)malloc(this->size * sizeof(T));
        };

        Array(T* other, unsigned int size) {
            this->size = size;
            this->array = (T*)malloc(this->size * sizeof(T));
            std::copy(other, other + this->size, this->array);
        };

        Array(const T& other) : size(other.size) {
            this->array = (T*)malloc(this->size * sizeof(T));
            std::copy(other.array, other.array + this->size, this->array);
        };

        ~Array() {
               if (this->array != NULL) {
                for (unsigned int i = 0; i < this->size; i++) this->array[i].~T();
                free(this->array);
                this->array = NULL;
                this->size = 0;
            }
        };

        Array(const Array<T>& other) : size(other.size) {
            this->array = (T*)malloc(this->size * sizeof(T));
            std::copy(other.array, other.array + this->size, this->array); 
        };

        Array<T>& operator=(const Array<T>& other) {
            this->size = other.size;
            this->array = (T*)malloc(this->size * sizeof(T));
            std::copy(other.array, other.array + this->size, this->array);

            return *this;
        };

        unsigned int getSize() const {
            return this->size;
        };

        T& operator[](unsigned int index) {
            return this->array[index];
        };

        const T& operator[](unsigned int index) const {
            return this->array[index];
        };

    private:
        unsigned int size;
        T * array;
};

#endif
