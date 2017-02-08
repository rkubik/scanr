#ifndef SCANR_ASSERT_H
#define SCANR_ASSERT_H

#define ASSERT_ALWAYS(exp) \
{ \
    if ((exp)) { \
        abort(); \
    } \
}

#endif
