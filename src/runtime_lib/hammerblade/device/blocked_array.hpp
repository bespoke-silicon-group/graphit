#ifndef _BLOCKED_ARRAY_HPP_
#define _BLOCKED_ARRAY_HPP_
#include <vector>
#include <cstdint>
#include <string>

template <typename array_type, const size_t block_size> class blocked_array {
    int64_t     current_block;
    bool        automatic_mode;
    int64_t     reads, flushes;
    bool        potentially_dirty;
    bool        debug;
    std::string name;

    int64_t array_offset;

public:

    //std::vector<array_type>   &data;
    array_type * data;
    int64_t size;


    void    reset() {
        reads = flushes = 0;
        automatic_mode = false;
        potentially_dirty = false;
        current_block = -1;

    }
    void set_debug(bool dbg) { debug = dbg; }
    blocked_array(array_type * d, int64_t sz,
                  std::string name = "",
		  int64_t array_offset = 0) :

        data(d), size(sz),
        name(name),
        debug(false),
	array_offset(array_offset) {

        reset();
    }


    // core calculations
    int64_t size_of() { return size; }
    int64_t size_of_block() { return block_size; }
    int64_t block_of(int offset) { return offset / size_of_block(); }
    int64_t block_base_of(int offset) { return block_of(offset) * size_of_block(); }
    int     number_of_blocks() { return (size % block_size) == 0 ? (size / block_size) : ((size / block_size) + 1); }

    // Mode setting
    void    automatic(bool _automatic) { automatic_mode = _automatic; }

    // Stat gathering

    // Functions to inform the system about reads and writes
    void    read_block(int offset) {
        if (current_block != block_of(offset)) {

            ++reads;
            current_block = block_of(offset);
        }
    }
    void    write_block(int offset) {
        read_block(offset);
        potentially_dirty = true;
    }

    // Functions for validating accesses are within range (or updating the range in automatic mode)
    void    _fail() { char *bad_access = NULL; *bad_access = (char)0xDEAD; }
    void    check_access(int64_t offset) {
            if (block_of(offset) != current_block) {
                if (!automatic_mode) {
                    printf("Blocked array offset error.  offset = %d current_block = %d block size = %zu\n",
                           offset, current_block, block_size);
                    _fail();
                }
                read_block(offset);
            }
    }

    // Accessors
    array_type operator [] (int64_t index) const {
        check_access(index);
        return data[index];
    }
    array_type & operator [] (int64_t index) {
        check_access(index);
        potentially_dirty = true;
        return data[index];
    }
};


#endif
