#include "pch.h"
#include "png_implementation_pixel_cast.hpp"

using floating_type = float;

template <pixel_type end, pixel_type start> constexpr bool my_pixel_cast_implemented_v{ 0 };
template <pixel_type end, pixel_type start> end my_pixel_cast(start);

#define DEFINE_MY_PIXEL_CAST(end_type, start_type) \
template <> constexpr bool my_pixel_cast_implemented_v<end_type, start_type>{ 1 };\
template <> end_type my_pixel_cast<end_type, start_type>(start_type in)

#define GENERATE_GREYSCALE_TO_GREYSCALE(start, start_max, end, end_max) \
DEFINE_MY_PIXEL_CAST(greyscale_ ## end, greyscale_ ## start) {\
    if constexpr (std::is_same_v<greyscale_ ## end, greyscale_ ## start>) {\
        return in;\
    } else if constexpr (!std::is_same_v<greyscale_ ## end, greyscale_float>) {\
        return { static_cast<decltype(greyscale_ ## end{}.grey)>(std::round(static_cast<floating_type>(in.grey) / start_max * end_max)) };\
    } else {\
        return { static_cast<decltype(greyscale_ ## end{}.grey)>(static_cast<floating_type>(in.grey) / start_max * end_max) };\
    }\
}

#define GENERATE_GREYSCALE_TO_ALL_GREYSCALE(start, start_max) \
GENERATE_GREYSCALE_TO_GREYSCALE(start, start_max, 1, 1)\
GENERATE_GREYSCALE_TO_GREYSCALE(start, start_max, 2, 3)\
GENERATE_GREYSCALE_TO_GREYSCALE(start, start_max, 4, 15)\
GENERATE_GREYSCALE_TO_GREYSCALE(start, start_max, 8, 255)\
GENERATE_GREYSCALE_TO_GREYSCALE(start, start_max, 16, 65535)\
GENERATE_GREYSCALE_TO_GREYSCALE(start, start_max, float, 1)

GENERATE_GREYSCALE_TO_ALL_GREYSCALE(1, 1)
GENERATE_GREYSCALE_TO_ALL_GREYSCALE(2, 3)
GENERATE_GREYSCALE_TO_ALL_GREYSCALE(4, 15)
GENERATE_GREYSCALE_TO_ALL_GREYSCALE(8, 255)
GENERATE_GREYSCALE_TO_ALL_GREYSCALE(16, 65535)
GENERATE_GREYSCALE_TO_ALL_GREYSCALE(float, 1)

#define GENERATE_TRUECOLOUR_TO_TRUECOLOUR(start, start_max, end, end_max) \
DEFINE_MY_PIXEL_CAST(truecolour_ ## end, truecolour_ ## start) {\
    if constexpr (std::is_same_v<truecolour_ ## end, truecolour_ ## start>) {\
        return in;\
    } else if constexpr (!std::is_same_v<truecolour_ ## end, truecolour_float>) {\
        return { static_cast<decltype(truecolour_ ## end{}.red)>(std::round(static_cast<floating_type>(in.red) / start_max * end_max)),\
            static_cast<decltype(truecolour_ ## end{}.green)>(std::round(static_cast<floating_type>(in.green) / start_max * end_max)),\
            static_cast<decltype(truecolour_ ## end{}.blue)>(std::round(static_cast<floating_type>(in.blue) / start_max * end_max)) };\
    } else {\
        return { static_cast<decltype(truecolour_ ## end{}.red)>(static_cast<floating_type>(in.red) / start_max * end_max),\
            static_cast<decltype(truecolour_ ## end{}.green)>(static_cast<floating_type>(in.green) / start_max * end_max),\
            static_cast<decltype(truecolour_ ## end{}.blue)>(static_cast<floating_type>(in.blue) / start_max * end_max) };\
    }\
}

#define GENERATE_TRUECOLOUR_TO_ALL_TRUECOLOUR(start, start_max) \
GENERATE_TRUECOLOUR_TO_TRUECOLOUR(start, start_max, 8, 255)\
GENERATE_TRUECOLOUR_TO_TRUECOLOUR(start, start_max, 16, 65535)\
GENERATE_TRUECOLOUR_TO_TRUECOLOUR(start, start_max, float, 1)

GENERATE_TRUECOLOUR_TO_ALL_TRUECOLOUR(8, 255)
GENERATE_TRUECOLOUR_TO_ALL_TRUECOLOUR(16, 65535)
GENERATE_TRUECOLOUR_TO_ALL_TRUECOLOUR(float, 1)

#define GENERATE_GREYSCALE_WITH_ALPHA_TO_GREYSCALE_WITH_ALPHA(start, start_max, end, end_max) \
DEFINE_MY_PIXEL_CAST(greyscale_with_alpha_ ## end, greyscale_with_alpha_ ## start) {\
    if constexpr (std::is_same_v<greyscale_with_alpha_ ## end, greyscale_with_alpha_ ## start>) {\
        return in;\
    } else if constexpr(!std::is_same_v<greyscale_with_alpha_ ## end, greyscale_with_alpha_float>) {\
        return { static_cast<decltype(greyscale_with_alpha_ ## end{}.grey)>(std::round(static_cast<floating_type>(in.grey) / start_max * end_max)),\
            static_cast<decltype(greyscale_with_alpha_ ## end{}.alpha)>(std::round(static_cast<floating_type>(in.alpha) / start_max * end_max)) };\
    } else {\
        return { static_cast<decltype(greyscale_with_alpha_ ## end{}.grey)>(static_cast<floating_type>(in.grey) / start_max * end_max),\
            static_cast<decltype(greyscale_with_alpha_ ## end{}.alpha)>(static_cast<floating_type>(in.alpha) / start_max * end_max) };\
    }\
}

#define GENERATE_GREYSCALE_WITH_ALPHA_TO_ALL_GREYSCALE_WITH_ALPHA(start, start_max) \
GENERATE_GREYSCALE_WITH_ALPHA_TO_GREYSCALE_WITH_ALPHA(start, start_max, 8, 255)\
GENERATE_GREYSCALE_WITH_ALPHA_TO_GREYSCALE_WITH_ALPHA(start, start_max, 16, 65535)\
GENERATE_GREYSCALE_WITH_ALPHA_TO_GREYSCALE_WITH_ALPHA(start, start_max, float, 1)

GENERATE_GREYSCALE_WITH_ALPHA_TO_ALL_GREYSCALE_WITH_ALPHA(8, 255)
GENERATE_GREYSCALE_WITH_ALPHA_TO_ALL_GREYSCALE_WITH_ALPHA(16, 65535)
GENERATE_GREYSCALE_WITH_ALPHA_TO_ALL_GREYSCALE_WITH_ALPHA(float, 1)

#define GENERATE_TRUECOLOUR_WITH_ALPHA_TO_TRUECOLOUR_WITH_ALPHA(start, start_max, end, end_max) \
DEFINE_MY_PIXEL_CAST(truecolour_with_alpha_ ## end, truecolour_with_alpha_ ## start) {\
    if constexpr (std::is_same_v<truecolour_with_alpha_ ## end, truecolour_with_alpha_ ## start>) {\
        return in;\
    } else if constexpr (!std::is_same_v<truecolour_with_alpha_ ## end, truecolour_with_alpha_float>) {\
        return { static_cast<decltype(truecolour_with_alpha_ ## end{}.red)>(std::round(static_cast<floating_type>(in.red) / start_max * end_max)),\
            static_cast<decltype(truecolour_with_alpha_ ## end{}.green)>(std::round(static_cast<floating_type>(in.green) / start_max * end_max)),\
            static_cast<decltype(truecolour_with_alpha_ ## end{}.blue)>(std::round(static_cast<floating_type>(in.blue) / start_max * end_max)),\
            static_cast<decltype(truecolour_with_alpha_ ## end{}.alpha)>(std::round(static_cast<floating_type>(in.alpha) / start_max * end_max)) };\
    } else {\
        return { static_cast<decltype(truecolour_with_alpha_ ## end{}.red)>(static_cast<floating_type>(in.red) / start_max * end_max),\
            static_cast<decltype(truecolour_with_alpha_ ## end{}.green)>(static_cast<floating_type>(in.green) / start_max * end_max),\
            static_cast<decltype(truecolour_with_alpha_ ## end{}.blue)>(static_cast<floating_type>(in.blue) / start_max * end_max),\
            static_cast<decltype(truecolour_with_alpha_ ## end{}.alpha)>(static_cast<floating_type>(in.alpha) / start_max * end_max) };\
    }\
}

#define GENERATE_TRUECOLOUR_WITH_ALPHA_TO_ALL_TRUECOLOUR_WITH_ALPHA(start, start_max) \
GENERATE_TRUECOLOUR_WITH_ALPHA_TO_TRUECOLOUR_WITH_ALPHA(start, start_max, 8, 255)\
GENERATE_TRUECOLOUR_WITH_ALPHA_TO_TRUECOLOUR_WITH_ALPHA(start, start_max, 16, 65535)\
GENERATE_TRUECOLOUR_WITH_ALPHA_TO_TRUECOLOUR_WITH_ALPHA(start, start_max, float, 1)

GENERATE_TRUECOLOUR_WITH_ALPHA_TO_ALL_TRUECOLOUR_WITH_ALPHA(8, 255)
GENERATE_TRUECOLOUR_WITH_ALPHA_TO_ALL_TRUECOLOUR_WITH_ALPHA(16, 65535)
GENERATE_TRUECOLOUR_WITH_ALPHA_TO_ALL_TRUECOLOUR_WITH_ALPHA(float, 1)

DEFINE_MY_PIXEL_CAST(greyscale_with_alpha_8, greyscale_8) {
    return { in.grey, 1 };
}
DEFINE_MY_PIXEL_CAST(truecolour_with_alpha_8, truecolour_8) {
    return { in.red, in.green, in.blue, 1 };
}
DEFINE_MY_PIXEL_CAST(greyscale_with_alpha_16, greyscale_16) {
    return { in.grey, 1 };
}
DEFINE_MY_PIXEL_CAST(truecolour_with_alpha_16, truecolour_16) {
    return { in.red, in.green, in.blue, 1 };
}
DEFINE_MY_PIXEL_CAST(greyscale_8, greyscale_with_alpha_8) {
    return { in.grey };
}
DEFINE_MY_PIXEL_CAST(truecolour_8, truecolour_with_alpha_8) {
    return { in.red, in.green, in.blue };
}
DEFINE_MY_PIXEL_CAST(greyscale_16, greyscale_with_alpha_16) {
    return { in.grey };
}
DEFINE_MY_PIXEL_CAST(truecolour_16, truecolour_with_alpha_16) {
    return { in.red, in.green, in.blue };
}
DEFINE_MY_PIXEL_CAST(truecolour_float, greyscale_float) {
    return { in.grey, in.grey, in.grey };
}
DEFINE_MY_PIXEL_CAST(greyscale_with_alpha_float, greyscale_float) {
    return { in.grey, 1 };
}
DEFINE_MY_PIXEL_CAST(truecolour_with_alpha_float, greyscale_float) {
    return { in.grey, in.grey, in.grey, 1 };
}
DEFINE_MY_PIXEL_CAST(greyscale_float, truecolour_float) {
    floating_type acc{ in.red + in.green + in.blue };
    acc /= 3;
    return { acc };
}
DEFINE_MY_PIXEL_CAST(greyscale_with_alpha_float, truecolour_float) {
    floating_type acc{ in.red + in.green + in.blue };
    acc /= 3;
    return { acc, 1 };
}
DEFINE_MY_PIXEL_CAST(truecolour_with_alpha_float, truecolour_float) {
    return { in.red, in.green, in.blue, 1 };
}
DEFINE_MY_PIXEL_CAST(greyscale_float, greyscale_with_alpha_float) {
    return { in.grey };
}
DEFINE_MY_PIXEL_CAST(truecolour_float, greyscale_with_alpha_float) {
    return { in.grey, in.grey, in.grey };
}
DEFINE_MY_PIXEL_CAST(truecolour_with_alpha_float, greyscale_with_alpha_float) {
    return { in.grey, in.grey, in.grey, 1 };
}
DEFINE_MY_PIXEL_CAST(greyscale_float, truecolour_with_alpha_float) {
    floating_type acc{ in.red + in.green + in.blue };
    acc /= 3;
    return { acc };
}
DEFINE_MY_PIXEL_CAST(truecolour_float, truecolour_with_alpha_float) {
    return { in.red, in.green, in.blue };
}
DEFINE_MY_PIXEL_CAST(greyscale_with_alpha_float, truecolour_with_alpha_float) {
    floating_type acc{ in.red + in.green + in.blue };
    acc /= 3;
    return { acc, 1 };
}

template <pixel_type> struct get_floating_pixel_type;
template <> struct get_floating_pixel_type<greyscale_1> { using type = greyscale_float; };
template <> struct get_floating_pixel_type<greyscale_2> { using type = greyscale_float; };
template <> struct get_floating_pixel_type<greyscale_4> { using type = greyscale_float; };
template <> struct get_floating_pixel_type<greyscale_8> { using type = greyscale_float; };
template <> struct get_floating_pixel_type<greyscale_16> { using type = greyscale_float; };
template <> struct get_floating_pixel_type<greyscale_float> { using type = greyscale_float; };
template <> struct get_floating_pixel_type<truecolour_8> { using type = truecolour_float; };
template <> struct get_floating_pixel_type<truecolour_16> { using type = truecolour_float; };
template <> struct get_floating_pixel_type<truecolour_float> { using type = truecolour_float; };
template <> struct get_floating_pixel_type<greyscale_with_alpha_8> { using type = greyscale_with_alpha_float; };
template <> struct get_floating_pixel_type<greyscale_with_alpha_16> { using type = greyscale_with_alpha_float; };
template <> struct get_floating_pixel_type<greyscale_with_alpha_float> { using type = greyscale_with_alpha_float; };
template <> struct get_floating_pixel_type<truecolour_with_alpha_8> { using type = truecolour_with_alpha_float; };
template <> struct get_floating_pixel_type<truecolour_with_alpha_16> { using type = truecolour_with_alpha_float; };
template <> struct get_floating_pixel_type<truecolour_with_alpha_float> { using type = truecolour_with_alpha_float; };
template <pixel_type type> using get_floating_pixel_type_t = typename get_floating_pixel_type<type>::type;

template <pixel_type end_type, pixel_type start_type> end_type pixel_cast(start_type in) {
    if constexpr (my_pixel_cast_implemented_v<end_type, start_type>) {
        return my_pixel_cast<end_type, start_type>(in);
    } else {
        return my_pixel_cast<end_type, get_floating_pixel_type_t<end_type>>(
            my_pixel_cast<get_floating_pixel_type_t<end_type>, get_floating_pixel_type_t<start_type>>(
                my_pixel_cast<get_floating_pixel_type_t<start_type>, start_type>(in)
                )
            );
    }
}

#define INSTANTIATE_PIXEL_CAST_1(start_type, end_type) \
template end_type pixel_cast<end_type, start_type>(start_type);

#define INSTANTIATE_PIXEL_CAST_2(start_type) \
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_1)\
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_2)\
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_4)\
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_8)\
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_16)\
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_float)\
INSTANTIATE_PIXEL_CAST_1(start_type, truecolour_8)\
INSTANTIATE_PIXEL_CAST_1(start_type, truecolour_16)\
INSTANTIATE_PIXEL_CAST_1(start_type, truecolour_float)\
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_with_alpha_8)\
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_with_alpha_16)\
INSTANTIATE_PIXEL_CAST_1(start_type, greyscale_with_alpha_float)\
INSTANTIATE_PIXEL_CAST_1(start_type, truecolour_with_alpha_8)\
INSTANTIATE_PIXEL_CAST_1(start_type, truecolour_with_alpha_16)\
INSTANTIATE_PIXEL_CAST_1(start_type, truecolour_with_alpha_float)

INSTANTIATE_PIXEL_CAST_2(greyscale_1)
INSTANTIATE_PIXEL_CAST_2(greyscale_2)
INSTANTIATE_PIXEL_CAST_2(greyscale_4)
INSTANTIATE_PIXEL_CAST_2(greyscale_8)
INSTANTIATE_PIXEL_CAST_2(greyscale_16)
INSTANTIATE_PIXEL_CAST_2(greyscale_float)
INSTANTIATE_PIXEL_CAST_2(truecolour_8)
INSTANTIATE_PIXEL_CAST_2(truecolour_16)
INSTANTIATE_PIXEL_CAST_2(truecolour_float)
INSTANTIATE_PIXEL_CAST_2(greyscale_with_alpha_8)
INSTANTIATE_PIXEL_CAST_2(greyscale_with_alpha_16)
INSTANTIATE_PIXEL_CAST_2(greyscale_with_alpha_float)
INSTANTIATE_PIXEL_CAST_2(truecolour_with_alpha_8)
INSTANTIATE_PIXEL_CAST_2(truecolour_with_alpha_16)
INSTANTIATE_PIXEL_CAST_2(truecolour_with_alpha_float)