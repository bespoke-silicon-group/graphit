#ifndef _BLOCKED_ARRAY_HPP_
#define _BLOCKED_ARRAY_HPP_
#include <printing.hpp>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <direct_mapped_cache.hpp>
#include <cache.hpp>

using Cache = direct_mapped_cache;

template <typename array_type, const size_t block_size, const size_t window_size = 0> class blocked_array {
    int64_t     current_block;
    bool        automatic_mode;
    int64_t     reads, flushes;
    int64_t     window_reads, window_flushes;
    bool        potentially_dirty;
    bool        debug;
    bool        windowed_mode;
    int64_t     number_of_windows;
    std::vector<int64_t>   windows;                   //[data.size() / block_size + 1];
    std::vector<bool>     potentially_dirty_window; //[data.size() / block_size + 1];
    std::vector<int64_t>  block_reads;
    std::vector<int64_t>  block_flushes;
    std::string name;

    Cache::ptr cache;
    int64_t array_offset;

public:
    std::string stats_csv_header() const {
        return "block_size,block_bytes,reads,flushes";
    }
    std::string stats_csv() const {
        std::stringstream ss;
        ss << block_size   << ","
           << block_size * sizeof(array_type) << ","
           << reads        << ","
           << flushes      << "";
        return ss.str();
    }

    std::vector<array_type>   &data;
    typedef std::vector<int64_t>::const_iterator block_read_iter;
    typedef std::vector<int64_t>::const_iterator block_flush_iter;

    block_read_iter block_reads_begin() const { return block_reads.cbegin(); }
    block_read_iter block_reads_end()   const { return block_reads.cend(); }
    block_flush_iter block_flushes_begin() const { return block_flushes.cbegin(); }
    block_flush_iter block_flushes_end() const   { return block_flushes.cend(); }

    int64_t get_block_reads(size_t block) const { return block_reads[block]; }
    int64_t get_block_flushes(size_t block) const { return block_flushes[block]; }

    void    reset() {
        reads = flushes = 0;
        window_reads = window_flushes = 0;
        automatic_mode = false;
        potentially_dirty = false;
        current_block = -1;
        for (int64_t i = 0; i < number_of_windows; i++) {
            windows[i] = i * block_size;
            potentially_dirty_window[i] = false;
        }

        block_reads.clear();
        block_reads.resize(number_of_blocks(), 0);

        block_flushes.clear();
        block_flushes.resize(number_of_blocks(), 0);
    }
    void set_debug(bool dbg) { debug = dbg; }
    blocked_array(std::vector<array_type> &d,
                  std::string name = "",
		  Cache::ptr cachep = nullptr,
		  int64_t array_offset = 0) :

        data(d),
        name(name),
        debug(false),
	array_offset(array_offset) {

        number_of_windows = data.size() / block_size;
        windows.resize(data.size()/block_size + 1);
        potentially_dirty_window.resize(data.size()/block_size + 1);

        if (number_of_windows * block_size < data.size())
            ++number_of_windows;

        if (window_size != 0)
            windowed_mode = true;
        else
            windowed_mode = false;

	/* set the cache */
	if (cachep) {
	    cache = cachep;
	} else {
	    cache = Cache::make(1, block_size * sizeof(array_type));
	}

        reset();
    }

    Cache::ptr get_cache() { return cache; }

    // core calculations
    int64_t size_of() { return data.size(); }
    int64_t size_of_block() { return block_size; }
    int64_t size_of_window() { return window_size; }
    int64_t block_of(int offset) { return offset / size_of_block(); }
    int64_t  window_of(int offset) { return offset / size_of_window(); }
    int64_t block_base_of(int offset) { return block_of(offset) * size_of_block(); }
    int64_t window_base_of(int offset) { return window_of(offset) * size_of_window(); }
    int     number_of_blocks() { return (data.size() % block_size) == 0 ? (data.size() / block_size) : ((data.size() / block_size) + 1); }

    // Mode setting
    void    automatic(bool _automatic) { automatic_mode = _automatic; }
    void    windowed(bool _windowed) { windowed_mode = _windowed; }

    // Stat gathering
    int64_t number_of_reads() { return windowed_mode ? window_reads : reads; }
    int64_t number_of_flushes() { return windowed_mode ? window_flushes : flushes; }

    // Functions to inform the system about reads and writes
    void    flush_block() {
        ++block_flushes[current_block];
        ++flushes;
        potentially_dirty = false;
    }
    void    read_block(int offset) {
        if (current_block != block_of(offset)) {
            if (potentially_dirty)
                flush_block();

            if (debug)
                std::cerr << name
                          << ": reading block " << block_of(offset)
                          << " for offset " << offset << std::endl;

            ++block_reads[block_of(offset)];
            ++reads;
            current_block = block_of(offset);
        }
    }
    void    write_block(int offset) {
        read_block(offset);
        potentially_dirty = true;
    }
    void    flush_window(int block) {
        ++window_flushes;
        potentially_dirty_window[block] = false;
    }
    void    read_window(int offset) {
        int64_t block = block_of(offset);
        int64_t window = window_of(offset);

        if (window != windows[block]) {
            if (potentially_dirty_window[block])
                flush_block();
            ++window_reads;
            windows[block] = window;
        }
    }
    void    write_window(int offset) {
        read_window(offset);
        potentially_dirty_window[block_of(offset)] = true;
    }

    // Functions for validating accesses are within range (or updating the range in automatic mode)
    void    _fail() { char *bad_access = NULL; *bad_access = (char)0xDEAD; }
    void    check_access(int64_t offset) {
        if (!windowed_mode) {
            if (block_of(offset) != current_block) {
                if (!automatic_mode) {
                    printf("Blocked array offset error.  offset = %d current_block = %d block size = %zu\n",
                           offset, current_block, block_size);
                    _fail();
                }
                read_block(offset);
            }
        } else {
            int64_t block = block_of(offset);
            int64_t window = window_of(offset);

            if (window != windows[block]) {
                if (!automatic_mode) {
                    printf("Window array offset error.  offset = %d / window = %d window offset = %d window size = %zu\n",
                           offset, window, windows[window], block_size);
                    _fail();
                }
                read_window(offset);
            }
        }
    }

    // Accessors
    array_type operator [] (int64_t index) const {
        check_access(index);
	cache->check_read_multi(array_offset + index*sizeof(array_type), sizeof(array_type));
        return data[index];
    }
    array_type & operator [] (int64_t index) {
        check_access(index);
        if (!windowed_mode)
            potentially_dirty = true;
        else
            potentially_dirty_window[block_of(index)] = true;
	cache->check_write_multi(array_offset + index*sizeof(array_type), sizeof(array_type));
        return data[index];
    }
};


#endif
