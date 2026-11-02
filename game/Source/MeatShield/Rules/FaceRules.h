#pragma once
#include <array>
#include <cstdint>
namespace MeatShield { struct FaceExpression { unsigned Preset=0; std::array<float,15> Values{}; }; FaceExpression ExpressionFor(std::uint64_t Life); }
