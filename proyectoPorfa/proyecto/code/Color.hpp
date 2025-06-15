/// @author Katya
/// @copyright () Katya
/// 
#pragma once

#include <cstdint>

namespace PracticaKatya
{
    using Monochrome8 = uint8_t;
    union Rgba8888
    {
        enum { RED, GREEN, BLUE, ALPHA };

        uint32_t value;
        uint8_t  components[4];
    };

}
