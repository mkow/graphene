/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 Intel Corporation
 *                    Michał Kowalczyk <mkow@invisiblethingslab.com>
 */

#include <linux/random.h>
#include <stdint.h>

#include "shim_internal.h"
#include "shim_table.h"

long shim_do_getrandom(char* buf, size_t count, unsigned int flags) {
    if (flags & ~(GRND_NONBLOCK | GRND_RANDOM | GRND_INSECURE))
        return -EINVAL;

    if ((flags & (GRND_INSECURE | GRND_RANDOM)) == (GRND_INSECURE | GRND_RANDOM))
        return -EINVAL;

    /* Weird, but that's what kernel does */
    if (count > INT_MAX)
        count = INT_MAX;

    if (test_user_memory(buf, count, /*write=*/true))
        return -EFAULT;

    int ret = DkRandomBitsRead(buf, count);
    if (ret < 0)
        return -convert_pal_errno(-ret);

    return count;
}
