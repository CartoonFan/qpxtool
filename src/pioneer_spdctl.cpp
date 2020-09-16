/*
 * This file is part of the QPxTool project.
 * Copyright (C) 2006 Gennady "ShultZ" Kozlov <qpxtool@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include <cstdio>
#include <iostream>

#include "qpx_transport.hpp"
#include "qpx_mmc.hpp"
#include "pioneer_spdctl.hpp"

auto pioneer_get_quiet(drive_info* drive) -> int
{
    drive->cmd[0] = 0x3C;
    drive->cmd[1] = 0x01;
    drive->cmd[2] = 0xF4;
    drive->cmd[7] = 0x01;
    if ((drive->err = drive->cmd.transport(READ, drive->rd_buf, 256)))
    {
        if (!drive->silent) {
            sperror("PIO_GET_QUIET", drive->err);
            return drive->err;
        }
    }
    drive->pioneer.limit  = drive->rd_buf[0];
    drive->pioneer.silent = drive->rd_buf[2];
    return 0;
}

auto pioneer_set_quiet(drive_info* drive, char silent, bool limit, bool save) -> int
{
    drive->cmd[0] = 0xBB;
    drive->cmd[1] = save? 0x0C : 0x04;
    drive->cmd[2] = limit? 0x00 : 0xFF;
    drive->cmd[3] = limit? 0x00 : 0xFF;
    drive->cmd[10]= (save? 0xC0 : 0x80) | silent;
    if ((drive->err = drive->cmd.transport(NONE, nullptr, 0)))
    {
        if (!drive->silent) {
            sperror("PIO_SET_QUIET", drive->err);
            return drive->err;
        }
    }
    return 0;
}

auto pioneer_set_silent(drive_info* drive, char silent, bool save) -> int
{
    pioneer_get_quiet(drive);
    if (!drive->silent) {
        std::cout << "Setting Quiet mode to \"" << pioneer_silent_tbl[(int)silent] << "\"... ";
    }
    pioneer_set_quiet(drive, silent, drive->pioneer.limit, save);
    pioneer_get_quiet(drive);
    if (!drive->silent) {
        std::cout << ((drive->pioneer.silent == silent) ? "OK":"FAIL") << "\n";
    }
    return drive->err;
}

auto pioneer_set_spdlim(drive_info* drive, bool limit, bool save) -> int
{
    pioneer_get_quiet(drive);
    if (!drive->silent) {
        std::cout << "Setting SpeedLimit " << (limit ? "ON":"OFF") << "... ";
    }
    pioneer_set_quiet(drive, drive->pioneer.silent, limit, save);
    pioneer_get_quiet(drive);
    if (!drive->silent) {
        std::cout << ((drive->pioneer.limit == limit) ? "OK":"FAIL") << "\n";
    }
    return drive->err;
}

int pioneer_set_peakpower(drive_info* drive, bool en, bool save)
{
}

int pioneer_set_pureread(drive_info* drive, char pr)
{
}

