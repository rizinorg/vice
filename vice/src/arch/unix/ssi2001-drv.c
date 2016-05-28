/*
 * ssi2001-drv.c - Unix specific SSI2001 (ISA SID card) driver.
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

#ifdef HAVE_SSI2001

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "io-access.h"
#include "ssi2001.h"
#include "types.h"

#define SSI2008_BASE 0x280

#define MAXSID 1

static int sids_found = -1;

void ssi2001_drv_store(WORD addr, BYTE value, int chipno)
{
    if (chipno < MAXSID && addr < 0x20) {
        io_access_store(SSI2008_BASE + (addr & 0x1f), value);
    }
}

int ssi2001_drv_read(WORD addr, int chipno)
{
    if (chipno < MAXSID && addr < 0x20) {
        return io_access_read(SSI2008_BASE + (addr & 0x1f));
    }
}

static int detect_sid(void)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        ssi2001_drv_store((WORD)i, 0, 0);
    }

    ssi2001_drv_store(0x12, 0xff, 0);

    for (i = 0; i < 100; ++i) {
        if (ssi2001_drv_read(0x1b, 0)) {
            return 0;
        }
    }

    ssi2001_drv_store(0x0e, 0xff, 0);
    ssi2001_drv_store(0x0f, 0xff, 0);
    ssi2001_drv_store(0x12, 0x20, 0);

    for (i = 0; i < 100; ++i) {
        if (ssi2001_drv_read(0x1b, 0)) {
            return 1;
        }
    }
    return 0;
}

int ssi2001_drv_open(void)
{
    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = 0;

    if (io_access_map(SSI2008_BASE, 32) < 0) {
        return -1;
    }
    if (detect_sid()) {
        sids_found = 1;
        return 0;
    }
    return -1;
}

int ssi2001_drv_close(void)
{
    io_access_unmap(SSI2008_BASE, 32);
    sids_found = -1;
    return 0;
}

int ssi2001_drv_available(void)
{
    return sids_found;
}
#endif
