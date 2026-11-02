#include "AvatarRules.h"
namespace MeatShield {
UniformStyle UniformFor(std::uint64_t Life)
{
    const auto Mixed=(Life^(Life>>32))*2654435761u;
    return {unsigned(Life%3),float(Mixed%997),bool(Life&1)};
}
}
