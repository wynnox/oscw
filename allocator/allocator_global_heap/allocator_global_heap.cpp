
#include "allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(
    logger *logger): _logger(logger)
{
    trace_with_guard("START: " + get_typename() + ": constructor");
    trace_with_guard("END: " + get_typename() + ": constructor");
}

allocator_global_heap::~allocator_global_heap()
{
    trace_with_guard("START: " + get_typename() + ": destructor");

    trace_with_guard("END: " + get_typename() + ": destructor");
}

allocator_global_heap::allocator_global_heap(
    allocator_global_heap &&other) noexcept
{
    trace_with_guard("START: " + get_typename() + ": move constructor");
    _logger = other._logger;
    other._logger = nullptr;
    trace_with_guard("END: "  + get_typename() + ": move constructor");
}

allocator_global_heap &allocator_global_heap::operator=(
    allocator_global_heap &&other) noexcept
{
    trace_with_guard("START: " + get_typename() + ": move operator");

    if (this != &other)
    {
        std::swap(_logger, other._logger);
    }

    trace_with_guard("END: " + get_typename() + ": move operator");

    return *this;
}

[[nodiscard]] void *allocator_global_heap::allocate(
    size_t value_size,
    size_t values_count)
{
    debug_with_guard("START: " + get_typename() + ": allocate");

    block_size_t meta_size = sizeof(allocator *) + sizeof(size_t);
    size_t block_size = value_size * values_count;

    if (block_size == 0)
    {
        warning_with_guard("WARNING: " + get_typename() + ": allocate with zero size");
        block_size += meta_size;
    }

    void *memory = ::operator new(block_size);

    if (memory == nullptr)
    {
        error_with_guard("ERROR: " + get_typename() + ": allocate");
        throw std::bad_alloc();
    }

    debug_with_guard("END: " + get_typename() + ": allocate");

    return memory;
}

void allocator_global_heap::deallocate(
    void *at)
{
    debug_with_guard("START: " + get_typename() + ": deallocate");

    ::operator delete(at);

    debug_with_guard("END: " + get_typename() + ": deallocate");
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const noexcept
{
    return "allocator_global_heap";
}