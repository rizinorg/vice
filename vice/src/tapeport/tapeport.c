/*
 * tapeport.c - tapeport handling.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <string.h>
#include <ctype.h>

#include "cmdline.h"
#include "cp-clockf83.h"
#include "datasette.h"
#include "dtl-basic-dongle.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "sense-dongle.h"
#include "snapshot.h"
#include "tapecart.h"
#include "tapeport.h"
#include "uiapi.h"
#include "util.h"

static old_tapeport_device_list_t old_tapeport_head = { NULL, NULL, NULL };
static old_tapeport_snapshot_list_t old_tapeport_snapshot_head = { NULL, NULL, NULL };

/* flag indicating if the tapeport exists on the current emulated model */
static int tapeport_active = 1;

/* current tapeport devices */
static int tapeport_current_device[TAPEPORT_MAX_PORTS] = { TAPEPORT_DEVICE_DATASETTE, TAPEPORT_DEVICE_DATASETTE };

static int old_tapeport_devices = 0;

static tapeport_device_t tapeport_device[TAPEPORT_MAX_DEVICES] = {0};

static int tapeport_ports = 0;

/* ---------------------------------------------------------------------------------------------------------- */

old_tapeport_device_list_t *old_tapeport_device_register(old_tapeport_device_t *device)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    old_tapeport_device_list_t *retval = NULL;
    int found = 0;
    int use_id = 0;

    if (old_tapeport_devices > 0) {
        while (!found) {
            if (current->device) {
                if (current->device->id == (old_tapeport_devices - 1)) {
                    found = 1;
                }
            }
            if (!found) {
                if (current->next) {
                    current = current->next;
                } else {
                    found = 2;
                }
            }
        }
        if (found == 2) {
            log_warning(LOG_DEFAULT, "TAPEPORT insertion error, highest id not present in chain");
            return NULL;
        } else {
            if (current->device->trigger_flux_change_passthrough || current->device->set_tape_sense_passthrough) {
                use_id = old_tapeport_devices;
            } else {
                if (device->trigger_flux_change_passthrough || device->set_tape_sense_passthrough) {
                    use_id = old_tapeport_devices - 1;
                    ++current->device->id;
                } else {
                    ui_error("last tapeport device %s does not support passthrough, and %s does not support passthrough either", current->device->name, device->name);
                    return NULL;
                }
            }
        }
    }

    retval = lib_malloc(sizeof(old_tapeport_device_list_t));

    while (current->next != NULL) {
        current = current->next;
    }
    current->next = retval;
    retval->previous = current;
    retval->device = device;
    retval->next = NULL;
    retval->device->id = use_id;

    ++old_tapeport_devices;

    return retval;
}

void old_tapeport_device_unregister(old_tapeport_device_list_t *device)
{
    old_tapeport_device_list_t *prev;
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int id;
    int end = 0;

    if (device) {
        prev = device->previous;
        prev->next = device->next;

        if (device->next) {
            device->next->previous = prev;
        }

        id = device->device->id;

        lib_free(device);

        if (old_tapeport_devices != id + 1) {
            while (!end) {
                if (current->device) {
                    if (current->device->id > id) {
                        --current->device->id;
                    }
                }
                if (current->next) {
                    current = current->next;
                } else {
                    end = 1;
                }
            }
        }
        --old_tapeport_devices;
    }
}

/* ---------------------------------------------------------------------------------------------------------- */

/* register a device to be used in the tapeport system */
int tapeport_device_register(int id, tapeport_device_t *device)
{
    if (id < 1 || id > TAPEPORT_MAX_DEVICES) {
        return -1;
    }

    tapeport_device[id].name = device->name;
    tapeport_device[id].enable = device->enable;
    tapeport_device[id].reset = device->reset;
    tapeport_device[id].set_motor = device->set_motor;
    tapeport_device[id].toggle_write_bit = device->toggle_write_bit;
    tapeport_device[id].set_sense_out = device->set_sense_out;
    tapeport_device[id].set_read_out = device->set_read_out;
    tapeport_device[id].write_snapshot = device->write_snapshot;
    tapeport_device[id].read_snapshot = device->read_snapshot;

    return 0;
}

/* attach device 'id' to a tapeport */
static int tapeport_set_device(int port, int id)
{
    /* 1st some sanity checks */
    if (id < TAPEPORT_DEVICE_NONE || id >= TAPEPORT_MAX_DEVICES) {
        return -1;
    }
    if (port >= TAPEPORT_MAX_PORTS) {
        return -1;
    }

    /* Nothing changes */
    if (id == tapeport_current_device[port]) {
        return 0;
    }

    /* check if id is registered */
    if (id != TAPEPORT_DEVICE_NONE && !tapeport_device[id].name) {
        ui_error("Selected tapeport device %d is not registered", id);
        return -1;
    }

    /* all checks done, now disable the current device and enable the new device */
    if (tapeport_device[tapeport_current_device[port]].enable) {
        tapeport_device[tapeport_current_device[port]].enable(port, 0);
    }

    if (tapeport_device[id].enable) {
        tapeport_device[id].enable(port, 1);
    }

    tapeport_current_device[port] = id;

    return 0;
}

tapeport_desc_t *tapeport_get_valid_devices(void)
{
    tapeport_desc_t *retval = NULL;
    int i;
    int j = 0;
    int valid = 0;

    for (i = 0; i < TAPEPORT_MAX_DEVICES; ++i) {
        if (tapeport_device[i].name) {
           ++valid;
        }
    }

    retval = lib_malloc(((size_t)valid + 1) * sizeof(tapeport_desc_t));

    for (i = 0; i < TAPEPORT_MAX_DEVICES; ++i) {
        if (tapeport_device[i].name) {
            retval[j].name = tapeport_device[i].name;
            retval[j].id = i;
            ++j;
        }
    }

    retval[j].name = NULL;

    return retval;
}

/* ---------------------------------------------------------------------------------------------------------- */

static void old_tapeport_set_motor(int flag)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int end = 0;

    if (tapeport_active) {
        while (!end) {
            if (current->device) {
                if (current->device->id == 0) {
                    if (current->device->set_motor) {
                        current->device->set_motor(flag);
                    }
                    end = 1;
                }
            }
            if (current->next) {
                current = current->next;
            } else {
                end = 1;
            }
        }
    }
}

void tapeport_set_motor(int port, int flag)
{
    /* ignore port 2 for now */
    if (port == TAPEPORT_PORT_1) {
        /* use new tapeport system if the device has been registered */
        if (tapeport_current_device[port] != TAPEPORT_DEVICE_NONE) {
            if (tapeport_device[tapeport_current_device[port]].name) {
                if (tapeport_device[tapeport_current_device[port]].set_motor) {
                    tapeport_device[tapeport_current_device[port]].set_motor(port, flag);
                }
            }
        }

        /* use old function as well for now */
        old_tapeport_set_motor(flag);
    }
}

static void old_tapeport_toggle_write_bit(int write_bit)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int end = 0;

    if (tapeport_active) {
        while (!end) {
            if (current->device) {
                if (current->device->id == 0) {
                    if (current->device->toggle_write_bit) {
                        current->device->toggle_write_bit(write_bit);
                    }
                    end = 1;
                }
            }
            if (current->next) {
                current = current->next;
            } else {
                end = 1;
            }
        }
    }
}

void tapeport_toggle_write_bit(int port, int write_bit)
{
    /* ignore port 2 for now */
    if (port == TAPEPORT_PORT_1) {
        /* use new tapeport system if the device has been registered */
        if (tapeport_current_device[port] != TAPEPORT_DEVICE_NONE) {
            if (tapeport_device[tapeport_current_device[port]].name) {
                if (tapeport_device[tapeport_current_device[port]].toggle_write_bit) {
                    tapeport_device[tapeport_current_device[port]].toggle_write_bit(port, write_bit);
                }
            }
        }

        /* use old function as well for now */
        old_tapeport_toggle_write_bit(write_bit);
    }
}

static void old_tapeport_set_sense_out(int sense)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int end = 0;

    if (tapeport_active) {
        while (!end) {
            if (current->device) {
                if (current->device->id == 0) {
                    if (current->device->set_sense_out) {
                        current->device->set_sense_out(sense);
                    }
                    end = 1;
                }
            }
            if (current->next) {
                current = current->next;
            } else {
                end = 1;
            }
        }
    }
}

void tapeport_set_sense_out(int port, int sense)
{
    /* ignore port 2 for now */
    if (port == TAPEPORT_PORT_1) {
        /* use new tapeport system if the device has been registered */
        if (tapeport_current_device[port] != TAPEPORT_DEVICE_NONE) {
            if (tapeport_device[tapeport_current_device[port]].name) {
                if (tapeport_device[tapeport_current_device[port]].set_sense_out) {
                    tapeport_device[tapeport_current_device[port]].set_sense_out(port, sense);
                }
            }
        }

        /* use old function as well for now */
        old_tapeport_set_sense_out(sense);
    }
}

static void old_tapeport_reset(void)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int end = 0;

    if (tapeport_active) {
        while (!end) {
            if (current->device) {
                if (current->device->reset) {
                    current->device->reset();
                }
            }
            if (current->next) {
                current = current->next;
            } else {
                end = 1;
            }
        }
    }
}

void tapeport_reset(void)
{
    int i;

    if (tapeport_active) {
        for (i = 0; i < TAPEPORT_MAX_PORTS; i++) {
            /* use new tapeport system if the device has been registered */
            if (tapeport_current_device[i] != TAPEPORT_DEVICE_NONE) {
                if (tapeport_device[tapeport_current_device[i]].name) {
                    if (tapeport_device[tapeport_current_device[i]].reset) {
                        tapeport_device[tapeport_current_device[i]].reset(i);
                    }
                }
            }
        }

        /* use old function as well for now */
        old_tapeport_reset();
    }
}

void old_tapeport_trigger_flux_change(unsigned int on, int id)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int end = 0;

    if (!tapeport_active) {
        return;
    }

    if (id == 0) {
        machine_trigger_flux_change(on);
    } else {
        while (!end) {
            if (current->device) {
                if (current->device->id == id - 1) {
                    if (current->device->trigger_flux_change_passthrough) {
                        current->device->trigger_flux_change_passthrough(on);
                    }
                }
            }
            if (current->next) {
                current = current->next;
            } else {
                end = 1;
            }
        }
    }
}

/* this function is not used yet, it will be used when devices start to switch over to the new system */
void tapeport_trigger_flux_change(unsigned int on, int port)
{
    /* ignore port 2 for now */
    if (port == TAPEPORT_PORT_1) {
        machine_trigger_flux_change(on);
    }
}

void old_tapeport_set_tape_sense(int sense, int id)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int end = 0;

    if (!tapeport_active) {
        return;
    }

    if (id == 0) {
        machine_set_tape_sense(sense);
    } else {
        while (!end) {
            if (current->device) {
                if (current->device->id == id - 1) {
                    if (current->device->set_tape_sense_passthrough) {
                        current->device->set_tape_sense_passthrough(sense);
                    }
                }
            }
            if (current->next) {
                current = current->next;
            } else {
                end = 1;
            }
        }
    }
}

/* this function is not used yet, it will be used when devices start to switch over to the new system */
void tapeport_set_tape_sense(int sense, int port)
{
    /* ignore port 2 for now */
    if (port == TAPEPORT_PORT_1) {
        machine_set_tape_sense(sense);
    }
}

void old_tapeport_set_write_in(int val, int id)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int end = 0;

    if (!tapeport_active) {
        return;
    }

    if (id == 0) {
        machine_set_tape_write_in(val);
    } else {
        while (!end) {
            if (current->device) {
                if (current->device->id == id - 1) {
                    if (current->device->set_tape_write_in_passthrough) {
                        current->device->set_tape_write_in_passthrough(val);
                    }
                }
            }
            if (current->next) {
                current = current->next;
            } else {
                end = 1;
            }
        }
    }
}

/* this function is not used yet, it will be used when devices start to switch over to the new system */
void tapeport_set_write_in(int val, int port)
{
    /* ignore port 2 for now */
    if (port == TAPEPORT_PORT_1) {
        machine_set_tape_write_in(val);
    }
}

void old_tapeport_set_motor_in(int val, int id)
{
    old_tapeport_device_list_t *current = &old_tapeport_head;
    int end = 0;

    if (!tapeport_active) {
        return;
    }

    if (id == 0) {
        machine_set_tape_motor_in(val);
    } else {
        while (!end) {
            if (current->device) {
                if (current->device->id == id - 1) {
                    if (current->device->set_tape_motor_in_passthrough) {
                        current->device->set_tape_motor_in_passthrough(val);
                    }
                }
            }
            if (current->next) {
                current = current->next;
            } else {
                end = 1;
            }
        }
    }
}

/* this function is not used yet, it will be used when devices start to switch over to the new system */
void tapeport_set_motor_in(int val, int port)
{
    /* ignore port 2 for now */
    if (port == TAPEPORT_PORT_1) {
        machine_set_tape_motor_in(val);
    }
}

/* ---------------------------------------------------------------------------------------------------------- */

void old_tapeport_snapshot_register(old_tapeport_snapshot_t *s)
{
    old_tapeport_snapshot_list_t *current = &old_tapeport_snapshot_head;
    old_tapeport_snapshot_list_t *retval = NULL;

    retval = lib_malloc(sizeof(old_tapeport_snapshot_list_t));

    while (current->next != NULL) {
        current = current->next;
    }
    current->next = retval;
    retval->previous = current;
    retval->snapshot = s;
    retval->next = NULL;
}

static void old_tapeport_snapshot_unregister(old_tapeport_snapshot_list_t *s)
{
    old_tapeport_snapshot_list_t *prev;

    if (s) {
        prev = s->previous;
        prev->next = s->next;

        if (s->next) {
            s->next->previous = prev;
        }

        lib_free(s);
    }
}

/* ------------------------------------------------------------------------- */

static int set_tapeport_device(int val, void *param)
{
    int port = vice_ptr_to_int(param);

    return tapeport_set_device(port, val);
}

static const resource_int_t resources_int_port1[] = {
    { "TapePort1Device", TAPEPORT_DEVICE_DATASETTE, RES_EVENT_NO, NULL,
      &tapeport_current_device[TAPEPORT_PORT_1], set_tapeport_device, (void *)TAPEPORT_PORT_1 },
    RESOURCE_INT_LIST_END
};

static const resource_int_t resources_int_port2[] = {
    { "TapePort2Device", TAPEPORT_DEVICE_DATASETTE, RES_EVENT_NO, NULL,
      &tapeport_current_device[TAPEPORT_PORT_2], set_tapeport_device, (void *)TAPEPORT_PORT_2 },
    RESOURCE_INT_LIST_END
};

static int tapeport_device_resources_init(int amount)
{
    if (tapertc_resources_init(amount) < 0) {
        return -1;
    }
    if (sense_dongle_resources_init(amount) < 0) {
        return -1;
    }
    if (dtlbasic_dongle_resources_init(amount) < 0) {
        return -1;
    }
    if (tapecart_resources_init(amount) < 0) {
        return -1;
    }

    return 0;
}

int tapeport_resources_init(int amount)
{
    memset(tapeport_device, 0, sizeof(tapeport_device));
    tapeport_device[0].name = "None";
    tapeport_ports = amount;

    if (tapeport_ports >= 1) {
        if (resources_register_int(resources_int_port1) < 0) {
            return -1;
        }
    }

    if (tapeport_ports >= 2) {
        if (resources_register_int(resources_int_port2) < 0) {
            return -1;
        }
    }

    return tapeport_device_resources_init(amount);
}

static void old_tapeport_resources_shutdown(void)
{
    old_tapeport_device_list_t *current = old_tapeport_head.next;
    old_tapeport_snapshot_list_t *c = old_tapeport_snapshot_head.next;

    while (current) {
        if (current->device && current->device->shutdown) {
            current->device->shutdown();
        }
        old_tapeport_device_unregister(current);
        current = old_tapeport_head.next;
    }

    while (c) {
        old_tapeport_snapshot_unregister(c);
        c = old_tapeport_snapshot_head.next;
    }
}

void tapeport_resources_shutdown(void)
{
    /* use old function for now */
    old_tapeport_resources_shutdown();
}

/* ------------------------------------------------------------------------- */

struct tapeport_opt_s {
    const char *name;
    int id;
};

static const struct tapeport_opt_s id_match[] = {
    { "none",           TAPEPORT_DEVICE_NONE },
    { "datasette",      TAPEPORT_DEVICE_DATASETTE },
    { "casette",        TAPEPORT_DEVICE_DATASETTE },
    { "tape",           TAPEPORT_DEVICE_DATASETTE },
    { "tapecart",       TAPEPORT_DEVICE_TAPECART },
#ifdef TAPEPORT_EXPERIMENTAL_DEVICES
    { "harness",        TAPEPORT_DEVICE_TAPE_DIAG_586220_HARNESS },
#endif
    { "rtc",            TAPEPORT_DEVICE_CP_CLOCK_F83 },
    { "sensedongle",    TAPEPORT_DEVICE_SENSE_DONGLE },
    { "tapedongle",     TAPEPORT_DEVICE_SENSE_DONGLE },
    { "playdongle",     TAPEPORT_DEVICE_SENSE_DONGLE },
    { "dtl",            TAPEPORT_DEVICE_DTL_BASIC_DONGLE },
    { "dtldongle",      TAPEPORT_DEVICE_DTL_BASIC_DONGLE },
    { "dtlbasic",       TAPEPORT_DEVICE_DTL_BASIC_DONGLE },
    { "dtlbasicdongle", TAPEPORT_DEVICE_DTL_BASIC_DONGLE },
    { NULL, -1 }
};

static int is_a_number(const char *str)
{
    size_t i;
    size_t len = strlen(str);

    for (i = 0; i < len; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

static int set_tapeport_cmdline_device(const char *param, void *extra_param)
{
    int temp = -1;
    int i = 0;
    int port = vice_ptr_to_int(extra_param);

    if (!param) {
        return -1;
    }

    do {
        if (strcmp(id_match[i].name, param) == 0) {
            temp = id_match[i].id;
        }
        i++;
    } while ((temp == -1) && (id_match[i].name != NULL));

    if (temp == -1) {
        if (!is_a_number(param)) {
            return -1;
        }
        temp = atoi(param);
    }

    return set_tapeport_device(temp, int_to_void_ptr(port));
}

/* ------------------------------------------------------------------------- */

static char *build_tapeport_string(int port)
{
    int i = 0;
    char *tmp1;
    char *tmp2;
    char number[4];
    tapeport_desc_t *devices = tapeport_get_valid_devices();

    tmp1 = lib_msprintf("Set Tapeport %d device (0: None", port);

    for (i = 1; devices[i].name; ++i) {
        sprintf(number, "%d", devices[i].id);
        tmp2 = util_concat(tmp1, ", ", number, ": ", devices[i].name, NULL);
        lib_free(tmp1);
        tmp1 = tmp2;
    }
    tmp2 = util_concat(tmp1, ")", NULL);
    lib_free(tmp1);
    lib_free(devices);
    return tmp2;
}

static cmdline_option_t cmdline_options_port1[] =
{
    { "-tapeport1device", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS | CMDLINE_ATTRIB_DYNAMIC_DESCRIPTION,
      set_tapeport_cmdline_device, (void *)TAPEPORT_PORT_1, NULL, NULL,
      "Device", NULL },
    CMDLINE_LIST_END
};

static cmdline_option_t cmdline_options_port2[] =
{
    { "-tapeport2device", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS | CMDLINE_ATTRIB_DYNAMIC_DESCRIPTION,
      set_tapeport_cmdline_device, (void *)TAPEPORT_PORT_2, NULL, NULL,
      "Device", NULL },
    CMDLINE_LIST_END
};

static int tapeport_devices_cmdline_options_init(void)
{
    if (tapertc_cmdline_options_init() < 0) {
        return -1;
    }

    if (sense_dongle_cmdline_options_init() < 0) {
        return -1;
    }

    if (dtlbasic_dongle_cmdline_options_init() < 0) {
        return -1;
    }

    if (tapecart_cmdline_options_init() < 0) {
        return -1;
    }

    return 0;
}

int tapeport_cmdline_options_init(void)
{
    union char_func cf;

    if (tapeport_ports >= 1) {
        cf.f = build_tapeport_string;
        cmdline_options_port1[0].description = cf.c;
        cmdline_options_port1[0].attributes |= (TAPEPORT_PORT_1 << 8);
        if (cmdline_register_options(cmdline_options_port1) < 0) {
            return -1;
        }
    }

    if (tapeport_ports >= 2) {
        cf.f = build_tapeport_string;
        cmdline_options_port1[0].description = cf.c;
        cmdline_options_port1[0].attributes |= (TAPEPORT_PORT_2 << 8);
        if (cmdline_register_options(cmdline_options_port2) < 0) {
            return -1;
        }
    }

    return tapeport_devices_cmdline_options_init();
}

void tapeport_enable(int val)
{
    tapeport_active = val ? 1 : 0;
}

/* ---------------------------------------------------------------------------------------------------------- */

/* TAPEPORT snapshot module format:

   type  | name   | description
   ----------------------------
   BYTE  | active | tape port active flag
   BYTE  | amount | amount of active devices

   if 'amount' is non-zero the following is saved per active device:

   type  | name | description
   --------------------------
   BYTE  | id   | device id
 */

static char snap_module_name[] = "TAPEPORT";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

static int old_tapeport_snapshot_write_module(snapshot_t *s, int write_image)
{
    snapshot_module_t *m;
    int amount = 0;
    int *devices = NULL;
    old_tapeport_device_list_t *current = old_tapeport_head.next;
    old_tapeport_snapshot_list_t *c = NULL;
    int i = 0;

    while (current) {
        ++amount;
        current = current->next;
    }

    if (amount) {
        devices = lib_malloc(sizeof(int) * (amount + 1));
        current = old_tapeport_head.next;
        while (current) {
            devices[current->device->id] = current->device->device_id;
            current = current->next;
            ++i;
        }
        devices[i] = -1;
    }

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_B(m, (uint8_t)tapeport_active) < 0
        || SMW_B(m, (uint8_t)amount) < 0) {
        goto fail;
    }

    /* Save device id's */
    if (amount) {
        for (i = 0; i < amount; ++i) {
            if (SMW_B(m, (uint8_t)devices[i]) < 0) {
                goto fail;
            }
        }
    }

    snapshot_module_close(m);

    /* save device snapshots */
    if (amount) {
        for (i = 0; i < amount; ++i) {
            c = old_tapeport_snapshot_head.next;
            while (c) {
                if (c->snapshot->id == devices[i]) {
                    if (c->snapshot->write_snapshot) {
                        if (c->snapshot->write_snapshot(s, write_image) < 0) {
                            lib_free(devices);
                            return -1;
                        }
                    }
                }
                c = c->next;
            }
        }
    }

    lib_free(devices);

    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}

int tapeport_snapshot_write_module(snapshot_t *s, int write_image)
{
   /* use old function for now */
    return old_tapeport_snapshot_write_module(s, write_image);
}

static int old_tapeport_snapshot_read_module(snapshot_t *s)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;
    int amount = 0;
    char **detach_resource_list = NULL;
    old_tapeport_device_list_t *current = old_tapeport_head.next;
    int *devices = NULL;
    old_tapeport_snapshot_list_t *c = NULL;
    int i = 0;

    /* detach all tapeport devices */
    while (current) {
        ++amount;
        current = current->next;
    }

    if (amount) {
        detach_resource_list = lib_malloc(sizeof(char *) * (amount + 1));
        memset(detach_resource_list, 0, sizeof(char *) * (amount + 1));
        current = old_tapeport_head.next;
        while (current) {
            detach_resource_list[i++] = current->device->resource;
            current = current->next;
        }
        for (i = 0; i < amount; ++i) {
            resources_set_int(detach_resource_list[i], 0);
        }
        lib_free(detach_resource_list);
    }

    m = snapshot_module_open(s, snap_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(major_version, minor_version, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_B_INT(m, &tapeport_active) < 0
        || SMR_B_INT(m, &amount) < 0) {
        goto fail;
    }

    if (amount) {
        devices = lib_malloc(sizeof(int) * (amount + 1));
        for (i = 0; i < amount; ++i) {
            if (SMR_B_INT(m, &devices[i]) < 0) {
                lib_free(devices);
                goto fail;
            }
        }
        snapshot_module_close(m);
        for (i = 0; i < amount; ++i) {
            c = old_tapeport_snapshot_head.next;
            while (c) {
                if (c->snapshot->id == devices[i]) {
                    if (c->snapshot->read_snapshot) {
                        if (c->snapshot->read_snapshot(s) < 0) {
                            lib_free(devices);
                            return -1;
                        }
                    }
                }
                c = c->next;
            }
        }
        lib_free(devices);
        return 0;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

int tapeport_snapshot_read_module(snapshot_t *s)
{
   /* use old function for now */
    return old_tapeport_snapshot_read_module(s);
}
