#pragma once
#include <stdint.h>

// Called by one serialized initialization owner. A distinct object belongs to
// each initialization attempt; late completions retain their original owner.
class AX211Selection {
public:
    AX211Selection(bool imageMatches, uint16_t vendor, uint16_t product, uint32_t location)
        : active_(imageMatches && vendor == 0x8087 && product == 0x0033 &&
                  location == 0x14a00000) {}
    AX211Selection(const AX211Selection &) = delete;
    AX211Selection &operator=(const AX211Selection &) = delete;

    void recordCommands(uint8_t status, const uint8_t *bytes, unsigned length) {
        preferV1_ = active_ && status == 0 && bytes && length == 64 &&
                    (bytes[25] & 0x02) && !(bytes[41] & 0x20);
    }
    bool chooseV1InsteadOfV2() const { return active_ && preferV1_; }
    void close() { active_ = false; preferV1_ = false; }

private:
    bool active_;
    bool preferV1_ = false;
};
