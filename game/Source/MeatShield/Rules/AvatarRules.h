#pragma once
#include <cstdint>
namespace MeatShield {
struct UniformStyle { unsigned Palette=0; float Pattern=0; bool Vest=false; };
UniformStyle UniformFor(std::uint64_t Life);
}
