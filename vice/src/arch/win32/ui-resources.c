/*
 * uiresources.c - Windows resources.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>

#include "cmdline.h"
#include "intl.h"
#include "lib.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uilib.h"
#include "util.h"

/* UI-related resources.  */

struct {
    int fullscreendevice;
    int fullscreenbitdepth;
    int fullscreenwidth;
    int fullscreenheight;
    int fullscreenrefreshrate;
    int fullscreenenabled;
    int save_resources_on_exit;
    int confirm_on_exit;
    int single_cpu;
    int vblank_sync;
    int window_xpos[2];
    int window_ypos[2];
    char *monitor_dimensions;
    char *initialdir[UILIB_SELECTOR_STYLES_NUM];
} ui_resources;

static int set_fullscreen_device(resource_value_t v, void *param)
{
    ui_resources.fullscreendevice = (int)v;
    return 0;
}

static int set_fullscreen_bitdepth(resource_value_t v, void *param)
{
    ui_resources.fullscreenbitdepth = (int)v;
    return 0;
}

static int set_fullscreen_width(resource_value_t v, void *param)
{
    ui_resources.fullscreenwidth = (int)v;
    return 0;
}

static int set_fullscreen_height(resource_value_t v, void *param)
{
    ui_resources.fullscreenheight = (int)v;
    return 0;
}

static int set_fullscreen_refreshrate(resource_value_t v, void *param)
{
    ui_resources.fullscreenrefreshrate = (int)v;
    return 0;
}

static int set_fullscreen_enabled(resource_value_t v, void *param)
{
    ui_resources.fullscreenenabled = (int)v;
    return 0;
}

static int set_save_resources_on_exit(resource_value_t v, void *param)
{
    ui_resources.save_resources_on_exit = (int)v;
    return 0;
}

static int set_confirm_on_exit(resource_value_t v, void *param)
{
    ui_resources.confirm_on_exit = (int)v;
    return 0;
}

static int set_single_cpu(resource_value_t v, void *param)
{
    DWORD process_affinity;
    DWORD system_affinity;

    ui_resources.single_cpu = (int)v;
    if (GetProcessAffinityMask(GetCurrentProcess(), &process_affinity, &system_affinity)) {
        //  Check if multi CPU system or not
        if ((system_affinity & (system_affinity - 1))) {
            if (ui_resources.single_cpu == 1) {
                //  Set it to first CPU
                SetThreadAffinityMask(GetCurrentThread(),system_affinity ^ (system_affinity & (system_affinity - 1)));
            } else {
                //  Set it to all CPU
                SetThreadAffinityMask(GetCurrentThread(),system_affinity);
            }
        }
    }
    return 0;
}

static int set_monitor_dimensions(resource_value_t v, void *param)
{
    const char *name = (const char *)v;
    if (ui_resources.monitor_dimensions != NULL && name != NULL)
        if (strcmp(name, ui_resources.monitor_dimensions) == 0)
            return 0;
    util_string_set(&ui_resources.monitor_dimensions, name ? name : "");
    return 0;
}

static int set_initial_dir(resource_value_t v, void *param)
{
    const char *name = (const char *)v;
    int index = (int)param;
    if (ui_resources.initialdir[index] != NULL && name != NULL)
        if (strcmp(name, ui_resources.initialdir[index]) == 0)
            return 0;
    util_string_set(&ui_resources.initialdir[index], name ? name : "");
    return 0;
}

static int set_window_xpos(resource_value_t v, void *param)
{
    ui_resources.window_xpos[(int) param] = (int) v;
    return 0;
}

static int set_window_ypos(resource_value_t v, void *param)
{
    ui_resources.window_ypos[(int) param] = (int) v;
    return 0;
}

static int set_vblank_sync(resource_value_t v, void *param)
{
    ui_resources.vblank_sync = (int) v;
    return 0;
}

static const resource_t resources[] = {
    { "FullscreenDevice", RES_INTEGER, (resource_value_t)0,
      (void *)&ui_resources.fullscreendevice, set_fullscreen_device, NULL },
    { "FullscreenBitdepth", RES_INTEGER, (resource_value_t)8,
      (void *)&ui_resources.fullscreenbitdepth, set_fullscreen_bitdepth, NULL },
    { "FullscreenWidth", RES_INTEGER, (resource_value_t)640,
      (void *)&ui_resources.fullscreenwidth, set_fullscreen_width, NULL },
    { "FullscreenHeight", RES_INTEGER, (resource_value_t)480,
      (void *)&ui_resources.fullscreenheight, set_fullscreen_height, NULL },
    { "FullscreenRefreshRate", RES_INTEGER, (resource_value_t)0,
      (void *)&ui_resources.fullscreenrefreshrate,
      set_fullscreen_refreshrate, NULL },
    { "FullscreenEnabled", RES_INTEGER, (resource_value_t)0,
      (void *)&ui_resources.fullscreenenabled, set_fullscreen_enabled, NULL },
    { "SaveResourcesOnExit", RES_INTEGER, (resource_value_t)0,
      (void *)&ui_resources.save_resources_on_exit,
      set_save_resources_on_exit, NULL },
    { "ConfirmOnExit", RES_INTEGER, (resource_value_t)1,
      (void *)&ui_resources.confirm_on_exit, set_confirm_on_exit, NULL },
    { "MonitorDimensions", RES_STRING, (resource_value_t)"",
      (void *)&ui_resources.monitor_dimensions, set_monitor_dimensions, NULL },
    { "InitialDefaultDir", RES_STRING, (resource_value_t)"",
      (void *)&ui_resources.initialdir[0], set_initial_dir, (void *)0 },
    { "InitialTapeDir", RES_STRING, (resource_value_t)"",
      (void *)&ui_resources.initialdir[1], set_initial_dir, (void *)1 },
    { "InitialDiskDir", RES_STRING, (resource_value_t)"",
      (void *)&ui_resources.initialdir[2], set_initial_dir, (void *)2 },
    { "InitialAutostartDir", RES_STRING, (resource_value_t)"",
      (void *)&ui_resources.initialdir[3], set_initial_dir, (void *)3 },
    { "InitialCartDir", RES_STRING, (resource_value_t)"",
      (void *)&ui_resources.initialdir[4], set_initial_dir, (void *)4 },
    { "InitialSnapshotDir", RES_STRING, (resource_value_t)"",
      (void *)&ui_resources.initialdir[5], set_initial_dir, (void *)5 },
    { "SingleCPU", RES_INTEGER, (resource_value_t)0,
      (void *)&ui_resources.single_cpu, set_single_cpu, NULL },
    { "Window0Xpos", RES_INTEGER, (resource_value_t)CW_USEDEFAULT,
      (void *)&ui_resources.window_xpos[0], set_window_xpos, (void *)0 },
    { "Window0Ypos", RES_INTEGER, (resource_value_t)CW_USEDEFAULT,
      (void *)&ui_resources.window_ypos[0], set_window_ypos, (void *)0 },
    { "Window1Xpos", RES_INTEGER, (resource_value_t)CW_USEDEFAULT,
      (void *)&ui_resources.window_xpos[1], set_window_xpos, (void *)1 },
    { "Window1Ypos", RES_INTEGER, (resource_value_t)CW_USEDEFAULT,
      (void *)&ui_resources.window_ypos[1], set_window_ypos, (void *)1 },
    { "VBLANKSync", RES_INTEGER, (resource_value_t)1,
      (void *)&ui_resources.vblank_sync, set_vblank_sync, NULL },
    { NULL }
};

int ui_resources_init(void)
{
    intl_resources_init();
    return resources_register(resources);
}

void ui_resources_shutdown(void)
{
    int i;

    intl_resources_shutdown();
    if (ui_resources.monitor_dimensions != NULL)
        lib_free(ui_resources.monitor_dimensions);

    for (i = 0; i < UILIB_SELECTOR_STYLES_NUM; i++)
        if (ui_resources.initialdir[i] != NULL)
            lib_free(ui_resources.initialdir[i]);
}

int ui_vblank_sync_enabled()
{
    return ui_resources.vblank_sync;
}

/* ------------------------------------------------------------------------ */

/* UI-related command-line options.  */

static const cmdline_option_t cmdline_options[] = {
    { "-saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (resource_value_t)1,
      NULL, "Save settings (resources) on exit" },
    { "+saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (resource_value_t)0,
      NULL, "Never save settings (resources) on exit" },
    { "-confirmexit", SET_RESOURCE, 0, NULL, NULL,
      "ConfirmOnExit", (resource_value_t)0,
      NULL, "Confirm quiting VICE" },
    { "+confirmexit", SET_RESOURCE, 0, NULL, NULL,
      "ConfirmOnExit", (resource_value_t)1,
      NULL, "Never confirm quiting VICE" },
    { "-singlecpu", SET_RESOURCE, 0, NULL, NULL,
      "SingleCPU", (resource_value_t)0,
      NULL, "Use all CPU on SMP systems" },
    { "+singlecpu", SET_RESOURCE, 0, NULL, NULL,
      "SingleCPU", (resource_value_t)1,
      NULL, "Use only first CPU on SMP systems" },
    { NULL }
};


int ui_cmdline_options_init(void)
{
    intl_cmdline_options_init();
    return cmdline_register_options(cmdline_options);
}
