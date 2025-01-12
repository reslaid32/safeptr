#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>

template <typename T>
class SafePointer
{
private:
    T* ptr_ = nullptr;
    bool allocated_ = false;
    size_t size_;  // size in terms of elements, not bytes
public:
    SafePointer() = default;
    explicit SafePointer(size_t size) { allocate(size); }
    ~SafePointer() { deallocate(); }

    void allocate(size_t size) {
        if (size == 0) {
            throw std::invalid_argument("Cannot allocate 0 elements");
        }
        if (allocated_) {
            reallocate(size);
            return;
        }
        ptr_ = (T*)malloc(size * sizeof(T));  // Allocate memory based on element size
        if (!ptr_) {
            throw std::runtime_error("Memory allocation failed");
        }
        size_ = size;
        allocated_ = true;
    }

    void callocate(size_t num, size_t size) {
        if (num == 0 || size == 0) {
            throw std::invalid_argument("Cannot allocate 0 elements or size");
        }
        if (allocated_) {
            deallocate();
        }
        ptr_ = (T*)calloc(num, size);  // Allocate memory for 'num' elements of size
        if (!ptr_) {
            throw std::runtime_error("Memory allocation failed");
        }
        size_ = num;
        allocated_ = true;
    }

    void reallocate(size_t size) {
        if (size == 0) {
            throw std::invalid_argument("Cannot reallocate to 0 elements");
        }
        if (!allocated_) {
            allocate(size);
            return;
        }
        T* new_ptr = (T*)realloc((void*)ptr_, size * sizeof(T));  // Reallocate memory based on element size
        if (!new_ptr) {
            throw std::runtime_error("Memory reallocation failed");
        }
        size_ = size;
        ptr_ = new_ptr;
    }

    void deallocate() {
        if (allocated_) {
            free((void*)ptr_);
            ptr_ = nullptr;
            allocated_ = false;
            size_ = 0;
        }
    }

    bool is_allocated() const { return allocated_; }
    T* get() const { return ptr_; }
    size_t size() const { return size_; }
    bool is_empty() const { return allocated_ && size_ == 0; }

    void clear(bool doDeallocate = false) {
        if (doDeallocate) {
            deallocate();  // Frees the memory if doDeallocate is true
        } else {
            ptr_ = nullptr;  // Just clears the pointer without deallocating
            size_ = 0;       // Resets the size to 0
        }
    }

    void resize(size_t size) {
        if (size == size_) return;
        if (size == 0) {
            deallocate();
            return;
        }
        reallocate(size);
    }

    void fill(T *begin, T *end, const T& value) {
        if (!allocated_ || ptr_ == nullptr) {
            throw std::runtime_error("Cannot fill: memory is not allocated");
        }
        std::fill(begin, end, value);
    }

    void fill(const T& value) {
        fill(begin(), end(), value);
    }

    void swap(SafePointer<T>& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(allocated_, other.allocated_);
        std::swap(size_, other.size_);
    }

    SafePointer<T> clone() const {
        SafePointer<T> new_sptr;
        new_sptr.allocate(size_);
        std::copy(ptr_, ptr_ + size_, new_sptr.ptr_);
        return new_sptr;
    }

    bool compare(const SafePointer<T>& other) const {
        return ptr_ == other.ptr_;
    }

    void set(T* ptr) {
        if (allocated_) {
            deallocate();
        }
        ptr_ = ptr;
        allocated_ = (ptr_ != nullptr);
    }

    void copy(const SafePointer<T>& other, size_t size) {
        if (!other.is_allocated()) {
            throw std::runtime_error("Cannot copy from an unallocated SafePointer");
        }
        allocate(size);
        std::copy(other.ptr_, other.ptr_ + size, ptr_);
    }

    void move(SafePointer<T>&& other) {
        if (this != &other) {
            deallocate();
            ptr_ = other.ptr_;
            allocated_ = other.allocated_;
            other.ptr_ = nullptr;
            other.allocated_ = false;
            other.size_ = 0;
        }
    }

    T* begin() const { return ptr_; }
    T* end() const { return ptr_ + size_; }

    void set_value(const T& value, size_t idx = 0) {
        if (!allocated_ || ptr_ == nullptr) {
            throw std::runtime_error("Cannot set value: memory is not allocated");
        }
        ptr_[idx] = value;
    }

    void set_values(const T* src_begin, const T* src_end, T* dst_begin) {
        if (!allocated_ || ptr_ == nullptr) {
            throw std::runtime_error("Cannot set values: memory is not allocated");
        }

        if (src_begin == nullptr || src_end == nullptr || dst_begin == nullptr) {
            throw std::invalid_argument("Source and destination pointers cannot be null");
        }

        size_t src_count = src_end - src_begin;
        size_t dst_count = size_ - (dst_begin - ptr_);

        if (src_count > dst_count) {
            throw std::invalid_argument("Source range exceeds destination space");
        }

        std::copy(src_begin, src_end, dst_begin);
    }

    void set_values(const T* src_begin, const T* src_end) {
        set_values(src_begin, src_end, begin());
    }

    void get_values(const T* src_begin, const T* src_end, T* dst_begin) const {
        if (!allocated_ || ptr_ == nullptr) {
            throw std::runtime_error("Cannot get values: memory is not allocated");
        }

        if (src_begin == nullptr || src_end == nullptr || dst_begin == nullptr) {
            throw std::invalid_argument("Source and destination pointers cannot be null");
        }

        size_t count = src_end - src_begin;
        std::copy(src_begin, src_end, dst_begin);
    }

    void get_values(T* dst_begin) const {
        get_values(begin(), end(), dst_begin);
    }

    SafePointer<T>& operator=(const SafePointer<T>& other) {
        if (this == &other) {
            return *this;
        }
        deallocate();
        allocate(other.size_);
        std::copy(other.ptr_, other.ptr_ + other.size_, ptr_);
        return *this;
    }

    // SafePointer(const SafePointer&) = delete;
    // SafePointer& operator=(const SafePointer&) = delete;
};
