#include "../src/ax211_selection.hpp"
#include <cassert>

int main() {
    uint8_t mask[64] = {};
    mask[25] = 0xf7;
    mask[41] = 0xc8;
    AX211Selection target(true, 0x8087, 0x0033, 0x14a00000);
    assert(!target.chooseV1InsteadOfV2());
    target.recordCommands(0, mask, sizeof mask);
    assert(target.chooseV1InsteadOfV2());
    mask[41] |= 0x20;
    target.recordCommands(0, mask, sizeof mask);
    assert(!target.chooseV1InsteadOfV2());
    mask[41] = 0xc8;
    mask[25] = 0;
    target.recordCommands(0, mask, sizeof mask);
    assert(!target.chooseV1InsteadOfV2());
    mask[25] = 0xf7;
    for (int scenario = 0; scenario < 4; ++scenario) {
        AX211Selection other(scenario != 0, scenario == 1 ? 0 : 0x8087,
                             scenario == 2 ? 0 : 0x0033, scenario == 3 ? 0 : 0x14a00000);
        other.recordCommands(0, mask, sizeof mask);
        assert(!other.chooseV1InsteadOfV2());
    }
    target.recordCommands(1, mask, sizeof mask);
    assert(!target.chooseV1InsteadOfV2());
    target.recordCommands(0, mask, 63);
    assert(!target.chooseV1InsteadOfV2());
    target.close();
    AX211Selection retry(true, 0x8087, 0x0033, 0x14a00000);
    target.recordCommands(0, mask, sizeof mask);
    assert(!target.chooseV1InsteadOfV2() && !retry.chooseV1InsteadOfV2());
}
