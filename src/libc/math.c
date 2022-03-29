#include <libc/math.h>

U64 pow(U64 x, U64 y) {
    if (y == 0) {
        return 1;
    }
    if (x == 0) {
        return 0;
    }
    for (U64 i = 0; i < y - 1; i++) {
        x *= y;
    }
    return x;
}
