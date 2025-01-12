#include <cassert>
#include <iostream>
#include "safeptr.hpp"

void test_allocate_and_deallocate() {
    SafePointer<int> sptr;

    // Test allocation
    sptr.allocate(5);
    assert(sptr.is_allocated());
    assert(sptr.size() == 5);
    assert(sptr.get() != nullptr);

    // Test deallocation
    sptr.deallocate();
    assert(!sptr.is_allocated());
    assert(sptr.size() == 0);
    assert(sptr.get() == nullptr);
}

void test_resize() {
    SafePointer<int> sptr;
    sptr.allocate(5);

    // Resize to a larger size
    sptr.resize(10);
    assert(sptr.size() == 10);

    // Resize back to a smaller size
    sptr.resize(3);
    assert(sptr.size() == 3);

    // Resize to zero (deallocate)
    sptr.resize(0);
    assert(!sptr.is_allocated());
    assert(sptr.size() == 0);
}

void test_fill() {
    SafePointer<int> sptr;
    sptr.allocate(5);

    // Test filling with a specific value
    sptr.fill(42);
    for (size_t i = 0; i < sptr.size(); ++i) {
        assert(sptr.get()[i] == 42);
    }
}

void test_clone() {
    SafePointer<int> sptr;
    sptr.allocate(5);

    // Fill with values
    for (size_t i = 0; i < sptr.size(); ++i) {
        sptr.set_value(i + 1, i);
    }

    // Clone the SafePointer
    SafePointer<int> sptr_clone = sptr.clone();

    // Check that the clone has the same values
    for (size_t i = 0; i < sptr.size(); ++i) {
        assert(sptr_clone.get()[i] == i + 1);
    }
}

void test_move() {
    SafePointer<int> sptr1;
    sptr1.allocate(5);
    for (size_t i = 0; i < sptr1.size(); ++i) {
        sptr1.set_value(i + 1, i);
    }

    SafePointer<int> sptr2;
    sptr2.move(std::move(sptr1));

    // Check that sptr1 is now empty
    assert(!sptr1.is_allocated());
    assert(sptr1.size() == 0);
    assert(sptr1.get() == nullptr);

    // Check that sptr2 has the values from sptr1
    for (size_t i = 0; i < sptr2.size(); ++i) {
        assert(sptr2.get()[i] == i + 1);
    }
}

void test_copy() {
    SafePointer<int> sptr1;
    sptr1.allocate(5);
    for (size_t i = 0; i < sptr1.size(); ++i) {
        sptr1.set_value(i + 1, i);
    }

    SafePointer<int> sptr2;
    sptr2.copy(sptr1, sptr1.size());

    // Check that sptr2 has the same values as sptr1
    for (size_t i = 0; i < sptr1.size(); ++i) {
        assert(sptr2.get()[i] == i + 1);
    }
}

void test_swap() {
    SafePointer<int> sptr1;
    SafePointer<int> sptr2;

    sptr1.allocate(5);
    sptr2.allocate(3);

    sptr1.fill(1);
    sptr2.fill(2);

    sptr1.swap(sptr2);

    // After swap, sptr1 should contain 2, and sptr2 should contain 1
    for (size_t i = 0; i < sptr1.size(); ++i) {
        assert(sptr1.get()[i] == 2);
    }

    for (size_t i = 0; i < sptr2.size(); ++i) {
        assert(sptr2.get()[i] == 1);
    }
}

void test_clear() {
    {
        SafePointer<int> sptr;
        sptr.allocate(5);

        // Test clearing
        sptr.clear();
        assert(sptr.is_allocated()); // clear does not release memory argument doDeallocate is false
        assert(sptr.size() == 0);
        assert(sptr.get() == nullptr);
    }
    {
        SafePointer<int> sptr;
        sptr.allocate(5);

        // Test clearing
        sptr.clear(
            /*doDeallocate (default is false)*/
            true);
        assert(!sptr.is_allocated()); // clear release memory argument doDeallocate is true
        assert(sptr.size() == 0);
        assert(sptr.get() == nullptr);
    }
}

int unit() {
    // Run tests
    test_allocate_and_deallocate();
    test_resize();
    test_fill();
    test_clone();
    test_move();
    test_copy();
    test_swap();
    test_clear();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}