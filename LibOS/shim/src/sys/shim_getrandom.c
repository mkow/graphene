/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 Intel Corporation
 *                    Michał Kowalczyk <mkow@invisiblethingslab.com>
 */

#include "shim_internal.h"
#include "shim_table.h"

long shim_do_getrandom(char* buf, size_t count, unsigned int flags) {
    if (test_user_memory(buf, count, /*write=*/true))
        return -EFAULT;

    int ret = DkRandomBitsRead(buf, count);
    if (ret < 0)
        return -convert_pal_errno(-ret);

    return 0;
}
