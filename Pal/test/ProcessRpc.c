/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

/* This Hello World demostrate a simple multithread program */

#include "pal.h"
#include "pal_debug.h"
#include "api.h"

#define PINGPONG 100

int main (int argc, char ** argv)
{
    if (argc == 1) {
        unsigned long time = DkSystemTimeQuery ();
        char time_arg[24];
        snprintf(time_arg, 24, "%ld", time);

        const char * newargs[3] = { "ProcessRpc", time_arg, NULL };

        PAL_HANDLE proc = DkProcessCreate("file:ProcessRpc", 0, newargs);

        if (!proc) {
            pal_printf("Can't create process\n");
            goto out;
        }

        char buffer[12];
        int ret = DkStreamRead(proc, 0, 12, &buffer, NULL, 0);
        if (ret < 0)
            goto out;

        pal_printf("read %d bytes: %s", ret, buffer);

        for (int i = 1 ; i < PINGPONG ; i++) {
            DkStreamWrite(proc, 0, 12, "Hello Kid!\n", NULL);

            ret = DkStreamRead(proc, 0, 12, &buffer, NULL, 0);
            if (ret < 0)
                break;

            pal_printf("read %d bytes: %s", ret, buffer);
        }

        DkStreamWrite(proc, 0, 12, "Byeee Kid!\n", NULL);

        DkObjectClose(proc);
    } else {
        unsigned long end = DkSystemTimeQuery ();
        unsigned long start = atol (argv[1]);
        pal_printf ("process creation time = %d\n", end - start);

        PAL_HANDLE proc = pal_control.parent_process;
        char buffer[12];

        for (int i = 0 ; i < PINGPONG ; i++) {
            DkStreamWrite(proc, 0, 12, "Hello Dad!\n", NULL);

            int ret = DkStreamRead(proc, 0, 12, &buffer, NULL, 0);
            if (ret < 0)
                break;

            pal_printf("read %d bytes: %s", ret, buffer);
        }

        start = end;
        end = DkSystemTimeQuery ();
        pal_printf ("RPC ping-pong time = %d\n", end - start);
    }

out:
    DkProcessExit(0);
    return 0;
}

