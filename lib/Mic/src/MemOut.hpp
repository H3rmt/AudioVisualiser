#pragma once

// copied from AudioOutput.h
#include <AudioTools.h>

/**
* @brief Writes to a preallocated memory
 * @ingroup io
 */
class MemOutput : public AudioOutput {
public:
    MemOutput(uint8_t *start, int len) {
        p_start = start;
        p_next = start;
        max_size = len;
        is_active = true;
        if (p_next == nullptr) {
            LOGE("start must not be null");
        }
    }

    // extra method to switch buffers
    void setBuffer(uint8_t *start, int len) {
        p_start = start;
        p_next = start;
        max_size = len;
        is_active = true;
        pos = 0;
    }

    bool begin() override {
        is_active = true;
        p_next = p_start;
        pos = 0;
        return true;
    }

    size_t write(const uint8_t *data, size_t len) override {
        if (p_next == nullptr) return 0;
        if (pos + len <= max_size) {
            memcpy(p_next, data, len);
            pos += len;
            p_next += len;
            return len;
        } else {
            LOGE("Buffer too small: pos:%d, size: %d ", pos, (int)max_size);
            return 0;
        }
    }

    int availableForWrite() override { return max_size - pos; }

    int size() { return max_size; }

protected:
    int pos = 0;
    uint8_t *p_start = nullptr;
    uint8_t *p_next = nullptr;
    size_t max_size;
};