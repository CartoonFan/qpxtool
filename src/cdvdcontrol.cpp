/*
 * This file is part of the QPxTool project.
 * Copyright (C) 2005-2009 Gennady "ShultZ" Kozlov <qpxtool@mail.ru>
 *
 *
 * Some Plextor commands got from PxScan and CDVDlib (C) Alexander Noe`
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "pioneer_spdctl.hpp"
#include "plextor_features.hpp"
#include "qpx_mmc.hpp"
#include "yamaha_features.hpp"

constexpr std::string_view version = "1.4";

const uint32_t FL_HELP = 0x00000001;
const uint32_t FL_SCAN = 0x00000002;
const uint32_t FL_DEVICE = 0x00000004;
const uint32_t FL_CURRENT = 0x00000008;

const uint32_t FL_SUPPORTED = 0x00000010;
const uint32_t FL_LOCK = 0x00000020;
const uint32_t FL_POWEREC = 0x00000040;
const uint32_t FL_GIGAREC = 0x00000080;

const uint32_t FL_VARIREC_CD = 0x00000100;
const uint32_t FL_VARIREC_CD_OFF = 0x00000200;
const uint32_t FL_VARIREC_DVD = 0x00000400;
const uint32_t FL_VARIREC_DVD_OFF = 0x00000800;

const uint32_t FL_HCDR = 0x00001000;
const uint32_t FL_SSS = 0x00002000;
const uint32_t FL_SPDREAD = 0x00004000;
const uint32_t FL_BOOK_R = 0x00008000;

const uint32_t FL_BOOK_RDL = 0x00010000;
const uint32_t FL_TESTWRITE = 0x00020000;
const uint32_t FL_SILENT = 0x00040000;
const uint32_t FL_AS = 0x00080000;

const uint32_t FL_PXERASER = 0x00100000;
const uint32_t FL_SECUREC = 0x00200000;
const uint32_t FL_NOSECUREC = 0x00400000;
const uint32_t FL_VERBOSE = 0x00800000;

const uint32_t FL_YMH_AMQR = 0x01000000;
const uint32_t FL_YMH_FORCESPEED = 0x02000000;
const uint32_t FL_MQCK = 0x04000000;

const uint32_t FL_PIOLIMIT = 0x08000000;
const uint32_t FL_PIOQUIET = 0x10000000;
const uint32_t FL_LOEJ = 0x20000000;

const uint32_t FLAS_RETR = 0x00000001;
const uint32_t FLAS_STOR = 0x00000002;
const uint32_t FLAS_CREATE = 0x00000004;
const uint32_t FLAS_DEL = 0x00000008;

const uint32_t FLAS_VIEW = 0x00000010;
const uint32_t FLAS_ACT = 0x00000020;
const uint32_t FLAS_DEACT = 0x00000040;
const uint32_t FLAS_CLEAR = 0x00000080;

uint32_t flags = 0;
uint32_t flags_as = 0;

auto get_device_info(drive_info *dev) -> int {
  dev->ven_features = 0;
  dev->chk_features = 0;
  detect_capabilities(dev);
  //  detect_check_capabilities(dev);
  determine_disc_type(dev);
  if (isPlextor(dev) == 0 && isYamaha(dev) == 0 && isPioneer(dev) == 0) {
    std::cout << dev->device
              << ": drive not supported, only common controls will work!\n";
    //      return 1;
  }
  if (isPlextor(dev) != 0) {
    plextor_get_life(dev);
    if (dev->life.ok) {
      std::cout << "Discs loaded: " << dev->life.dn << "\n";
      std::cout << "Drive operating time:\n";
      std::cout << "  CD Rd  : " << dev->life.cr.h << ":" << dev->life.cr.m
                << ":" << dev->life.cr.s << "\n";
      std::cout << "  CD Wr  : " << dev->life.cw.h << ":" << dev->life.cw.m
                << ":" << dev->life.cw.s << "\n";
      if ((dev->rd_capabilities & DEVICE_DVD) != 0U) {
        std::cout << "  DVD Rd : " << dev->life.dr.h << ":" << dev->life.dr.m
                  << ":" << dev->life.dr.s << "\n";
      }
      if ((dev->wr_capabilities & DEVICE_DVD) != 0U) {
        std::cout << "  DVD Wr : " << dev->life.dw.h << ":" << dev->life.dw.m
                  << ":" << dev->life.dw.s << "\n";
      }
    }

    //      if ( isPlextorLockPresent(dev) )
    plextor_px755_do_auth(dev);
    if (plextor_get_hidecdr_singlesession(dev) == 0) {
      dev->ven_features |= PX_HCDRSS;
    }
    if (plextor_get_speedread(dev) == 0) {
      dev->ven_features |= PX_SPDREAD;
    }
    if (dev->wr_capabilities == 0U) {
      //          if (!yamaha_check_amqr(dev)) dev->ven_features|=YMH_AMQR;
      if (plextor_get_powerec(dev) == 0) {
        dev->ven_features |= PX_POWEREC;
        //              plextor_get_speeds(dev);
      }
      if (plextor_get_gigarec(dev) == 0) {
        dev->ven_features |= PX_GIGAREC;
      }
      if (plextor_get_varirec(dev, VARIREC_CD) == 0) {
        dev->ven_features |= PX_VARIREC_CD;
      }
      //          if (!plextor_get_securec(dev)) dev->ven_features|=PX_SECUREC;
      if (plextor_get_silentmode(dev) == 0) {
        dev->ven_features |= PX_SILENT;
      }
      if (plextor_get_securec_state(dev) == 0) {
        dev->ven_features |= PX_SECUREC;
      }
    }
    if ((dev->wr_capabilities & DEVICE_DVD) != 0U) {
      if (plextor_get_varirec(dev, VARIREC_DVD) == 0) {
        dev->ven_features |= PX_VARIREC_DVD;
      }
      if (plextor_get_bitset(dev, PLEX_BITSET_R) == 0) {
        dev->ven_features |= PX_BITSET_R;
      }
      if (plextor_get_bitset(dev, PLEX_BITSET_RDL) == 0) {
        dev->ven_features |= PX_BITSET_RDL;
      }
      if (plextor_get_autostrategy(dev) == 0) {
        dev->ven_features |= PX_ASTRATEGY;
      }
      if (plextor_get_testwrite_dvdplus(dev) == 0) {
        dev->ven_features |= PX_SIMUL_PLUS;
      }
    }
#warning "PlexEraser DETECTION. Just assume PX755/760 and Premium-II"
    // if ((dev->dev_ID == PLEXTOR_755) || (dev->dev_ID == PLEXTOR_760) ||
    // (dev->dev_ID == PLEXTOR_PREMIUM2))
    if (isPlextorLockPresent(dev) != 0) {
      dev->ven_features |= PX_ERASER;
    }
    if (yamaha_check_amqr(dev) == 0) {
      dev->ven_features |= YMH_AMQR;
    }
  } else if (isYamaha(dev) != 0) {
    if (yamaha_check_amqr(dev) == 0) {
      dev->ven_features |= YMH_AMQR;
    }
    if (yamaha_check_forcespeed(dev) == 0) {
      dev->ven_features |= YMH_FORCESPEED;
    }
    if (yamaha_f1_get_tattoo(dev) == 0) {
      dev->ven_features |= YMH_TATTOO;
    }
  } else if (isPioneer(dev) != 0) {
    if (pioneer_get_quiet(dev) == 0) {
      dev->ven_features |= PIO_QUIET;
    }
  }

  //  std::cout << "Trying opcode E9 modes...\n";
  //  for (int i=0; i<256; i++) {if (!plextor_get_mode(dev,i)) std::cout << "
  //  MODE 0x" << i << "\n";}

  //  std::cout << "Trying opcode ED modes...\n";
  //  for (int i=0; i<256; i++) {if (!plextor_get_mode2(dev,i)) std::cout << "
  //  MODE 0x" << i << "\n";}

  if ((flags & FL_SUPPORTED) != 0U) {
    //      std::cout << "____________________________\n";
    std::cout << "\n** Supported features:\n";
    std::cout << "AudioMaster Q.R.    : "
              << (((dev->ven_features & YMH_AMQR) != 0U) ? "YES" : "-") << "\n";
    std::cout << "Yamaha ForceSpeed   : "
              << (((dev->ven_features & YMH_FORCESPEED) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "Yamaha DiscT@2      : "
              << (((dev->ven_features & YMH_TATTOO) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "Hide CD-R           : "
              << (((dev->ven_features & PX_HCDRSS) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "SingleSession       : "
              << (((dev->ven_features & PX_HCDRSS) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "SpeedRead           : "
              << (((dev->ven_features & PX_SPDREAD) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "PoweRec             : "
              << (((dev->ven_features & PX_POWEREC) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "GigaRec             : "
              << (((dev->ven_features & PX_GIGAREC) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "VariRec CD          : "
              << (((dev->ven_features & PX_VARIREC_CD) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "VariRec DVD         : "
              << (((dev->ven_features & PX_VARIREC_DVD) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "SecuRec             : "
              << (((dev->ven_features & PX_SECUREC) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "Silent mode         : "
              << (((dev->ven_features & PX_SILENT) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "DVD+R bitsetting    : "
              << (((dev->ven_features & PX_BITSET_R) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "DVD+R DL bitsetting : "
              << (((dev->ven_features & PX_BITSET_RDL) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "DVD+R(W) testwrite  : "
              << (((dev->ven_features & PX_SIMUL_PLUS) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "AutoStrategy        : "
              << (((dev->ven_features & PX_ASTRATEGY) != 0U) ? "YES" : "-")
              << ((((dev->ven_features & PX_ASTRATEGY) != 0U) &&
                   (dev->dev_ID & (PLEXTOR_755 | PLEXTOR_760)))
                      ? " (EXTENDED)"
                      : "")
              << "\n";
    std::cout << "PlexEraser          : "
              << (((dev->ven_features & PX_ERASER) != 0U) ? "YES" : "-")
              << "\n";
    std::cout << "Pioneer QuietMode   : "
              << (((dev->ven_features & PIO_QUIET) != 0U) ? "YES" : "-")
              << "\n";
  }

  //  get_media_info(dev);
  //  if (dev->rd_capabilities & DEVICE_DVD) {

  if ((flags & FL_CURRENT) != 0U) {
    //      std::cout << "____________________________\n";
    std::cout << "\n** Current drive settings:\n";
  }
  if (((dev->capabilities & CAP_LOCK) != 0U) && ((flags & FL_CURRENT) != 0U)) {
    get_lock(dev);
    std::cout << "Lock state          : "
              << (((dev->parms.status & STATUS_LOCK) != 0) ? "ON" : "OFF")
              << "\n";
  }
  //  if ((dev->ven_features & YMH_AMQR)       && ((flags & FL_CURRENT) ||
  //  (flags & FL_YMH_AMQR)))
  //      std::cout << "AudioMaster Q.R.    : " << ((dev->yamaha.amqr) ?
  //      "ON":"OFF") << "\n";
  //  if ((dev->ven_features & YMH_FORCESPEED)       && ((flags & FL_CURRENT) ||
  //  (flags & FL_YMH_FORCESPEED)))
  //      std::cout << "Yamaha ForceSpeed   : " << ((dev->yamaha.forcespeed) ?
  //      "ON":"OFF") << "\n";
  if (((dev->ven_features & YMH_TATTOO) != 0U) &&
      ((flags & FL_CURRENT) != 0U)) {
    if ((dev->yamaha.tattoo_rows) != 0U) {
      std::cout << "Yamaha DiscT@2      : inner " << dev->yamaha.tattoo_i
                << "outer " << dev->yamaha.tattoo_o << "image 3744 x "
                << dev->yamaha.tattoo_rows << "\n";
    } else {
      if ((dev->media.type & DISC_CD) != 0U) {
        std::cout
            << "DiscT@2: Can't write T@2 on inserted disc! No space left?\n";
      } else {
        std::cout << "DiscT@2: No disc found! Can't get T@2 info!\n";
      }
    }
  }
  if (((dev->ven_features & PX_HCDRSS) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_HCDR) != 0U))) {
    std::cout << "Hide-CDR state      : "
              << (((dev->plextor.hcdr) != 0) ? "ON" : "OFF") << "\n";
  }
  if (((dev->ven_features & PX_HCDRSS) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || (((flags & FL_SSS) != 0U)))) {
    std::cout << "SingleSession state : "
              << (((dev->plextor.sss) != 0) ? "ON" : "OFF") << "\n";
  }
  if (((dev->ven_features & PX_SPDREAD) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_SPDREAD) != 0U))) {
    std::cout << "SpeedRead state     : "
              << (((dev->plextor.spdread) != 0) ? "ON" : "OFF") << "\n";
  }
  if (((dev->ven_features & PX_POWEREC) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_POWEREC) != 0U))) {
    std::cout << "PoweRec state       : "
              << (((dev->plextor.powerec_state) != 0) ? "ON" : "OFF") << "\n";
    if ((dev->media.type & DISC_CD) != 0U) {
      std::cout << "      PoweRec Speed : " << (dev->plextor.powerec_spd / 176)
                << "X (CD)\n";
    }
    if ((dev->media.type & DISC_DVD) != 0U) {
      std::cout << "      PoweRec Speed : " << (dev->plextor.powerec_spd / 1385)
                << "X (DVD)\n";
    }
    //      show_powerec_speed();
  }
  if (((dev->ven_features & PX_GIGAREC) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_GIGAREC) != 0U))) {
    print_gigarec_value(dev);
  }
  if (((dev->ven_features & PX_VARIREC_CD) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_VARIREC_CD) != 0U))) {
    std::cout << "VariRec CD state    : "
              << (((dev->plextor.varirec_state_cd) != 0) ? "ON" : "OFF")
              << "\n";
    if ((dev->plextor.varirec_state_cd) != 0) {
      print_varirec(dev, VARIREC_CD);
    }
  }
  if (((dev->ven_features & PX_VARIREC_DVD) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_VARIREC_DVD) != 0U))) {
    std::cout << "VariRec DVD state   : "
              << (((dev->plextor.varirec_state_dvd) != 0) ? "ON" : "OFF")
              << "\n";
    if ((dev->plextor.varirec_state_dvd) != 0) {
      print_varirec(dev, VARIREC_DVD);
    }
  }
  if (((dev->ven_features & PX_SECUREC) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_SECUREC) != 0U))) {
    std::cout << "SecuRec state       : "
              << (((dev->plextor.securec) != 0) ? "ON" : "OFF") << "\n";
  }
  if (((dev->ven_features & PX_SILENT) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_SILENT) != 0U))) {
    std::cout << "Silent mode         : "
              << (((dev->plextor_silent.state) != 0) ? "ON" : "OFF") << "\n";
    if ((dev->plextor_silent.state) != 0U) {
      plextor_print_silentmode_state(dev);
    }
  }
  if (((dev->ven_features & PX_BITSET_R) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_BOOK_R) != 0U))) {
    std::cout << "DVD+R bitsetting    : "
              << (((dev->book_plus_r) != 0) ? "ON" : "OFF") << "\n";
  }
  if (((dev->ven_features & PX_BITSET_RDL) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_BOOK_RDL) != 0U))) {
    std::cout << "DVD+R DL bitsetting : "
              << (((dev->book_plus_rdl) != 0) ? "ON" : "OFF") << "\n";
  }
  if (((dev->ven_features & PX_SIMUL_PLUS) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_TESTWRITE) != 0U))) {
    std::cout << "DVD+R(W) testwrite  : "
              << (((dev->plextor.testwrite_dvdplus) != 0) ? "ON" : "OFF")
              << "\n";
  }
  if (((dev->ven_features & PX_ASTRATEGY) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_AS) != 0U))) {
    plextor_print_autostrategy_state(dev);
  }
  if ((dev->ven_features & PX_ERASER) != 0U) {
    std::cout << "PlexEraser          : supported\n";
  }
  if (((dev->ven_features & PIO_QUIET) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_PIOQUIET) != 0U))) {
    std::cout << "QuietMode setting   : "
              << pioneer_silent_tbl[(int)dev->pioneer.silent] << "\n";
  }
  if (((dev->ven_features & PIO_QUIET) != 0U) &&
      (((flags & FL_CURRENT) != 0U) || ((flags & FL_PIOLIMIT) != 0U))) {
    std::cout << "Speed Limit         : "
              << (((dev->pioneer.limit) != 0) ? "ON" : "OFF") << "\n";
  }
  return 0;
}

void usage(char *bin) {
  std::cout << stderr << "\nusage: " << bin << " [-d device] [options]\n";
}

void print_opts() {
  std::cout << "Common options:\n";
  std::cout
      << "\t-l, --scanbus                list drives (scan IDE/SCSI bus)\n";
  std::cout << "\t-h, --help                   show help\n";
  std::cout << "\t-v, --verbose                be verbose\n";
  std::cout << "\t-c, --current                show current drive settings\n";
  std::cout << "\t-s, --supported              show supported features\n";
  std::cout << "\t--[un]lock                   lock/unlock disc in drive\n";
  std::cout << "\t--lockt                      toggle media lock state\n";
  std::cout << "\t--eject                      eject media\n";
  std::cout
      << "\t--load                       load media (only for tray devices)\n";
  std::cout << "\t--loej                       load/eject media (toggle tray "
               "state)\n";
  std::cout << "\t--loej-immed                 don't wait for media is loaded "
               "or ejected\n";

  std::cout << "\noptions for Plextor devices:\n";
  std::cout << "\t--spdread [on|off]           turn SpeedRead on/off (default: "
               "off)\n";
  std::cout << "\t--sss [on|off]               turn SingleSession on/off "
               "(default: off)\n";
  std::cout
      << "\t--hcdr [on|off]              turn Hide-CDR on/off (default: off)\n";
  std::cout
      << "\t--powerec [on|off]           turn PoweREC on/off (default: on)\n";
  //  std::cout << "\t--amqr [on|off]              turn Yamaha AMQR on/off
  //  (default: off)\n"; std::cout << "\t--forcespeed [on|off]        turn
  //  Yamaha ForceSpeed on/off (default: off)\n";
  std::cout
      << "\t--gigarec <state>            set GigaREC rate or turn it off\n";
  std::cout << "\t                             values: 0.6, 0.7, 0.8, 0.9, "
               "1.1, 1.2, 1.3, 1.4, off\n";
  std::cout << "\t--varirec-cd <pwr>           set VariREC power for CD or "
               "turn VariREC off\n";
  std::cout << "\t                             values: +4, +3, +2, +1, 0, -1, "
               "-2, -3, -4, off\n";
  std::cout << "\t--varirec-cd-strategy <str>  set VariREC strategy for CD\n";
  std::cout << "\t                             values: default, azo, cya, pha, "
               "phb, phc, phd\n";
  std::cout << "\t--varirec-dvd <pwr>          set VariREC power for DVD or "
               "turn VariREC off\n";
  std::cout << "\t                             values: +4, +3, +2, +1, 0, -1, "
               "-2, -3, -4, off\n";
  std::cout << "\t--varirec-dvd-strategy <str> set VariREC strategy for DVD\n";
  std::cout << "\t                             values: default, 0, 1, 2, 3, 4, "
               "5, 6, 7\n";
  std::cout
      << "\t--securec <passwd>           set SecuREC password and turn it on\n";
  std::cout << "\t                             passwd must be 4..10 characters "
               "length\n";
  std::cout << "\t--nosecurec                  turn SecuRec off\n";
  std::cout << "\t--bitset+r [on|off]          turn on/off setting DVD-ROM "
               "book on DVD+R media\n";
  std::cout << "\t--bitset+rdl [on|off]        turn on/off setting DVD-ROM "
               "book on DVD+R DL media\n";
  std::cout << "\t--mqck <quick|advanced>      run MediaQuality Check\n";
  std::cout
      << "\t--mqck-speed #               set MQCK speed (default: maximum)\n";
  std::cout << "\t--as-mode <mode>             AS: select AutoStrategy mode: "
               "forced,auto,on,off,\n";
  std::cout << "\t--as-list                    AS: view strategies list\n";
  std::cout << "\t--as-on #                    AS: activate strategy #\n";
  std::cout << "\t--as-off #                   AS: deactivate strategy #\n";
  std::cout << "\t--as-del #                   AS: delete strategy #\n";
  std::cout << "\t--as-create <q|f> <a|r>      AS: create new strategy for "
               "inserted DVD media (PX-755/PX-760 only)\n";
  std::cout << "\t                             mode:    q = quick, f = full\n";
  std::cout
      << "\t                             action:  a = add,   r = replace\n";
  std::cout << "\t--as-save <file>             AS: save Strategies            "
               "/ EXPERIMENTAL /\n";
  std::cout << "\t--as-load <file>             AS: load Strategy from file    "
               "/ EXPERIMENTAL /\n";
  std::cout << "\t--as-clear                   AS: remove ALL strategies from "
               "database\n";
  std::cout << "\t--dvd+testwrite [on|off]     turn on/off testwrite on "
               "DVD+R(W) media\n";
  std::cout << "\t--silent [on|off]            just turn Silent Mode on/off "
               "(default: on)\n";
  std::cout
      << "\t--sm-nosave                  don't save Silent Mode settings\n";
  std::cout << "\t--sm-cd-rd #                 set max CD READ speed (default: "
               "32X)\n";
  std::cout << "\t                             values: 4, 8, 24, 32, 40, 48\n";
  std::cout << "\t--sm-cd-wr #                 set max CD WRITE speed "
               "(default: 32X)\n";
  std::cout << "\t                             values: 4, 8, 16, 24, 32, 48\n";
  std::cout << "\t--sm-dvd-rd #                set max DVD READ speed "
               "(default: 12X)\n";
  std::cout << "\t                             values: 2, 5, 8, 12, 16\n";
  //  std::cout << "\t--sm-dvd-wr #                set max DVD WRITE speed
  //  (default: 8X)\n"; std::cout << "\t                             values: 4,
  //  6, 8, 12, 16\n";
  std::cout << "\t--sm-load #                  set tray load speed. spd can be "
               "0 to 80 (default: 63)\n";
  std::cout << "\t--sm-eject #                 set tray eject speed. spd can "
               "be 0 to 80 (default: 0)\n";
  std::cout << "\t--sm-access <slow|fast>      set access time, has effect "
               "only with CD/DVD speed setting\n";
  std::cout << "\t--destruct <quick|full>      perform PlexEraser function\n";
  std::cout << "\t                             WARNING! data on inserted "
               "CD-R/DVD-R will be lost!\n";
  std::cout << "\t                             works on PX-755/PX-760/Premium2 "
               "only\n";

  std::cout << "\noptions for Pioneer devices:\n";
  std::cout << "\t--pio-limit [on|off]         limit (or not) read speed by "
               "24x for CD and 8x for DVD\n";
  std::cout << "\t--pio-quiet [quiet|perf|std] select QuietMode setting: "
               "Quiet, Performance or Standard (default: quiet)\n";
  std::cout << "\t--pio-nosave                 don't save settings to drive "
               "(changes will be lost after reboot)\n";
}

void needs_parameter(char *argv[], int i) {
  std::cout << "option " << argv[i] << " needs parameter!\n";
}

auto main(int argc, char *argv[]) -> int {
  const int eleven_bit_limit = 2048;
  const int six_bit_limit_minus_one = 63;
  const int eight_bit_limit = 256;
  int i = 0;
  char *device = nullptr;
  drive_info *dev = nullptr;
  char aslfn[eleven_bit_limit];
  char assfn[eleven_bit_limit];
  //  char    asfn2[1032];
  FILE *asf;
  //  FILE*   asf2;

  int powerec = 1;
  int gigarec = GIGAREC_10;
  int varirec_cd_pwr = VARIREC_NULL;
  int varirec_cd_str = 0;
  int varirec_dvd_pwr = VARIREC_NULL;
  int varirec_dvd_str = 0;
  int hcdr = 1;
  int sss = 1;
  int spdread = 1;
  int bookr = 1;
  int bookrdl = 1;
  int testwrite = 1;
  int as_mqck = -1;
  int as_mqck_spd = -1;
  int as = AS_AUTO;
  int ascre = ASDB_CRE_QUICK;
  int as_idx_act = 0;
  int as_idx_deact = 0;
  int as_idx_del = 0;
  int silent = 1;
  int silent_load = six_bit_limit_minus_one;
  int silent_eject = 0;
  int silent_access = SILENT_ACCESS_FAST;
  int silent_cd_rd = SILENT_CD_RD_32X;
  int silent_cd_wr = SILENT_CD_WR_32X;
  int silent_dvd_rd = SILENT_DVD_RD_12X;
  //  int silent_dvd_wr = SILENT_DVD_WR_8X;
  int silent_cd = 0;
  int silent_dvd = 0;
  int silent_tray = 0;
  int silent_save = 1;
  int pxeraser = 0;
  char passwd[eight_bit_limit];

  bool piolimit = true;
  int eject = 2;
  int lock = 2;
  bool loej_immed = false;

  std::cout << "**  CDVD Control v" << version
            << "  (c) 2005-2009  Gennady \"ShultZ\" Kozlov\n";

  //  std::cout << "Parsing commandline options (" << argc-1 << " args)...\n";
  for (i = 1; i < argc; i++) {
    const char *conflicting_lock_option =
        "Conflicting/duplicated lock/unlock option!\n";
    //      std::cout << "arg[" << i << "]: " << argv[i] << "\n";
    if (strcmp(argv[i], "-d") == 0) {
      if (argc > (i + 1)) {
        i++;
        flags |= FL_DEVICE;
        device = argv[i];
      } else {
        needs_parameter(argv, i);
        exit(1);
      }
    } else if ((strcmp(argv[i], "-h") == 0) ||
               (strcmp(argv[i], "--help") == 0)) {
      flags |= FL_HELP;
    } else if ((strcmp(argv[i], "-c") == 0) ||
               (strcmp(argv[i], "--current") == 0)) {
      flags |= FL_CURRENT;
    } else if ((strcmp(argv[i], "-s")) == 0 ||
               (strcmp(argv[i], "--supported") == 0)) {
      flags |= FL_SUPPORTED;
    } else if ((strcmp(argv[i], "-l")) == 0 ||
               (strcmp(argv[i], "--scanbus")) == 0) {
      flags |= FL_SCAN;
    }
    //  ************   Lock
    else if ((strcmp(argv[i], "--unlock")) == 0) {
      if ((flags & FL_LOCK) == 0U) {
        flags |= FL_LOCK;
        lock = 0;
      } else {
        std::cout << conflicting_lock_option;
      }
    } else if ((strcmp(argv[i], "--lock") == 0)) {
      if ((flags & FL_LOCK) == 0U) {
        flags |= FL_LOCK;
        lock = 1;
      } else {
        std::cout << conflicting_lock_option;
      }
    } else if ((strcmp(argv[i], "--lockt")) == 0) {
      if ((flags & FL_LOCK) == 0U) {
        flags |= FL_LOCK;
        lock = 2;
      } else {
        std::cout << conflicting_lock_option;
      }
    } else if ((strcmp(argv[i], "--load")) == 0) {
      if ((flags & FL_LOEJ) == 0U) {
        flags |= FL_LOEJ;
        eject = 0;
      } else {
        std::cout << conflicting_lock_option;
      }
    } else if ((strcmp(argv[i], "--eject")) == 0) {
      if ((flags & FL_LOEJ) == 0U) {
        flags |= FL_LOEJ;
        eject = 1;
      } else {
        std::cout << conflicting_lock_option;
      }
    } else if ((strcmp(argv[i], "--loej")) == 0) {
      if ((flags & FL_LOEJ) == 0U) {
        flags |= FL_LOEJ;
        eject = 2;
      } else {
        std::cout << conflicting_lock_option;
      }
    } else if ((strcmp(argv[i], "--loej-immed")) == 0) {
      loej_immed = true;
    } else if ((strcmp(argv[i], "--spdread")) == 0) {
      flags |= FL_SPDREAD;
      if (argc > (i + 1)) {
        i++;
        if ((strcmp(argv[i], "off")) == 0) {
          spdread = 0;
        } else if ((strcmp(argv[i], "on")) == 0) {
          spdread = 1;
        } else {
          std::cout << "Illegal argument for SpeedRead: " << argv[i] << "\n";
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if ((strcmp(argv[i], "--sss")) == 0) {
      flags |= FL_SSS;
      if (argc > (i + 1)) {
        i++;
        if ((strcmp(argv[i], "off")) == 0) {
          sss = 0;
        } else if ((strcmp(argv[i], "on")) == 0) {
          sss = 1;
        } else {
          std::cout << "Illegal argument for SingleSession: " << argv[i]
                    << "\n";
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if ((strcmp(argv[i], "--hcdr")) == 0) {
      flags |= FL_HCDR;
      if (argc > (i + 1)) {
        i++;
        if ((strcmp(argv[i], "off")) == 0) {
          hcdr = 0;
        } else if ((strcmp(argv[i], "on")) == 0) {
          hcdr = 1;
        } else {
          std::cout << "Illegal argument for Hide-CDR: " << argv[i] << "\n";
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    }
    //  ************   PoweREC
    else if ((strcmp(argv[i], "--powerec")) == 0) {
      flags |= FL_POWEREC;
      if (argc > (i + 1)) {
        i++;
        if ((strcmp(argv[i], "off")) == 0) {
          powerec = 0;
        } else if ((strcmp(argv[i], "on")) == 0) {
          powerec = 1;
        } else {
          std::cout << "Illegal argument for PoweRec: " << argv[i] << "\n";
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    }
    //  ************   GigaREC
    else if ((strcmp(argv[i], "--gigarec")) == 0) {
      if (argc > (i + 1)) {
        i++;
        flags |= FL_GIGAREC;
        if ((strcmp(argv[i], "0.6")) == 0) {
          gigarec = GIGAREC_06;
        } else if ((strcmp(argv[i], "0.7")) == 0) {
          gigarec = GIGAREC_07;
        } else if ((strcmp(argv[i], "0.8")) == 0) {
          gigarec = GIGAREC_08;
        } else if ((strcmp(argv[i], "0.9")) == 0) {
          gigarec = GIGAREC_09;
        } else if ((strcmp(argv[i], "1.0")) == 0 ||
                   ((strcmp(argv[i], "off")) == 0)) {
          gigarec = GIGAREC_10;
        } else if ((strcmp(argv[i], "1.1")) == 0) {
          gigarec = GIGAREC_11;
        } else if ((strcmp(argv[i], "1.2")) == 0) {
          gigarec = GIGAREC_12;
        } else if ((strcmp(argv[i], "1.3")) == 0) {
          gigarec = GIGAREC_13;
        } else if ((strcmp(argv[i], "1.4")) == 0) {
          gigarec = GIGAREC_14;
        } else {
          std::cout << "Illegal GigaREC value: " << argv[i] << "\n";
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    }
    //  ************   VariREC  CD
    else if ((strcmp(argv[i], "--varirec-cd")) == 0) {
      if (argc > (i + 1)) {
        i++;
        flags |= FL_VARIREC_CD;
        int val = 0;
        if ((strcmp(argv[i], "off")) == 0) {
          flags |= FL_VARIREC_CD_OFF;
        } else {
          val = strtol(argv[i], nullptr, 0);
          if (errno || val < -4 || val > 4) {
            std::cout << "Illegal VariREC CD power value: " << argv[i] << "\n";
            return 5;
          } else {
            switch (val) {
            case -4:
              varirec_cd_pwr = VARIREC_MINUS_4;
              break;
            case -3:
              varirec_cd_pwr = VARIREC_MINUS_3;
              break;
            case -2:
              varirec_cd_pwr = VARIREC_MINUS_2;
              break;
            case -1:
              varirec_cd_pwr = VARIREC_MINUS_1;
              break;
            case 0:
              varirec_cd_pwr = VARIREC_NULL;
              break;
            case 1:
              varirec_cd_pwr = VARIREC_PLUS_1;
              break;
            case 2:
              varirec_cd_pwr = VARIREC_PLUS_2;
              break;
            case 3:
              varirec_cd_pwr = VARIREC_PLUS_3;
              break;
            case 4:
              varirec_cd_pwr = VARIREC_PLUS_4;
              break;
            }
          }
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if ((strcmp(argv[i], "--varirec-cd-strategy")) == 0) {
      const int max_varirec_args = 6;
      if (argc > (i + 1)) {
        i++;
        int val = std::stol(argv[i]);
        if (!errno && val >= -1 && val < max_varirec_args) {
          varirec_cd_str = val + 1;
        } else if ((strcmp(argv[i], "default")) == 0) {
          varirec_cd_str = 0;
        } else if ((strcmp(argv[i], "azo")) == 0) {
          varirec_cd_str = 1;
        } else if ((strcmp(argv[i], "cya")) == 0) {
          varirec_cd_str = 2;
        } else if ((strcmp(argv[i], "pha")) == 0) {
          varirec_cd_str = 3;
        } else if ((strcmp(argv[i], "phb")) == 0) {
          varirec_cd_str = 4;
        } else if ((strcmp(argv[i], "phc")) == 0) {
          varirec_cd_str = 5;
        } else if ((strcmp(argv[i], "phd")) == 0) {
          varirec_cd_str = 6;
        } else {
          std::cout << "Illegal VariREC CD strategy: " << argv[i] << "\n";
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    }
    //  ************   VariREC  DVD
    else if ((strcmp(argv[i], "--varirec-dvd")) == 0) {
      if (argc > (i + 1)) {
        i++;
        flags |= FL_VARIREC_DVD;
        int val = 0;
        if ((strcmp(argv[i], "off")) == 0) {
          flags |= FL_VARIREC_DVD_OFF;
        } else {
          val = strtol(argv[i], nullptr, 0);
          if (errno || val < -4 || val > 4) {
            std::cout << "Illegal VariREC DVD power value: " << argv[i] << "\n";
            return 5;
          } else
            switch (val) {
            case -4:
              varirec_dvd_pwr = VARIREC_MINUS_4;
              break;
            case -3:
              varirec_dvd_pwr = VARIREC_MINUS_3;
              break;
            case -2:
              varirec_dvd_pwr = VARIREC_MINUS_2;
              break;
            case -1:
              varirec_dvd_pwr = VARIREC_MINUS_1;
              break;
            case 0:
              varirec_dvd_pwr = VARIREC_NULL;
              break;
            case 1:
              varirec_dvd_pwr = VARIREC_PLUS_1;
              break;
            case 2:
              varirec_dvd_pwr = VARIREC_PLUS_2;
              break;
            case 3:
              varirec_dvd_pwr = VARIREC_PLUS_3;
              break;
            case 4:
              varirec_dvd_pwr = VARIREC_PLUS_4;
              break;
            }
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--varirec-dvd-strategy")) {
      if (argc > (i + 1)) {
        i++;
        int val = atol(argv[i]);
        if (!errno && val >= -1 && val < 8) {
          varirec_dvd_str = val + 1;
        } else if (!strcmp(argv[i], "default"))
          varirec_dvd_str = 0;
        else {
          std::cout << "Illegal VariREC DVD strategy: " << argv[i] << "\n";
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--securec")) {
      if (argc > (i + 1)) {
        i++;
        strcpy(passwd, argv[i]);
        flags |= FL_SECUREC;
        //  std::cout << "SecuRec pass: " << passwd << "\n";
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--nosecurec"))
      flags |= FL_NOSECUREC;
    else if (!strcmp(argv[i], "--bitset+r")) {
      flags |= FL_BOOK_R;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "off"))
          bookr = 0;
        else if (!strcmp(argv[i], "on"))
          bookr = 1;
      }
    } else if (!strcmp(argv[i], "--bitset+rdl")) {
      flags |= FL_BOOK_RDL;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "off"))
          bookrdl = 0;
        else if (!strcmp(argv[i], "on"))
          bookrdl = 1;
      }
    } else if (!strcmp(argv[i], "--dvd+testwrite")) {
      flags |= FL_TESTWRITE;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "off"))
          testwrite = 0;
        else if (!strcmp(argv[i], "on"))
          testwrite = 1;
      }
    } else if (!strcmp(argv[i], "--mqck")) {
      flags |= FL_MQCK;
      if (argc > (i + 1)) {
        i++;
        as_idx_act = (int)strtol(argv[i], NULL, 0);
        if (!strcmp(argv[i], "quick"))
          as_mqck = AS_MEDIACK_QUICK;
        else if (!strcmp(argv[i], "adv"))
          as = AS_MEDIACK_ADV;
        else if (!strcmp(argv[i], "advanced"))
          as = AS_MEDIACK_ADV;
        else
          std::cout << "invalid MQCK mode requested: " << argv[i] << "\n";
      }
    } else if (!strcmp(argv[i], "--mqck-speed")) {
      if (argc > (i + 1)) {
        i++;
        as_mqck_spd = (int)strtol(argv[i], NULL, 0);
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--as-mode")) {
      flags |= FL_AS;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "forced"))
          as = AS_FORCED;
        else if (!strcmp(argv[i], "on"))
          as = AS_ON;
        else if (!strcmp(argv[i], "auto"))
          as = AS_AUTO;
        else if (!strcmp(argv[i], "off"))
          as = AS_OFF;
        else
          std::cout << "invalid AutoStrategy mode requested: " << argv[i]
                    << "\n";
      }
    } else if (!strcmp(argv[i], "--as-create")) {
      flags_as |= FLAS_CREATE;
      ascre = 0;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "q"))
          ascre |= ASDB_CRE_QUICK;
        else if (!strcmp(argv[i], "f"))
          ascre |= ASDB_CRE_FULL;
        else
          std::cout << "invalid --as-create parameter 1: '" << argv[i]
                    << "'. Only 'q|f' are valid\n";
      }
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "a"))
          ascre |= ASDB_ADD;
        else if (!strcmp(argv[i], "r"))
          ascre |= ASDB_REPLACE;
        else
          std::cout << "invalid --as-create parameter 2: '" << argv[i]
                    << "'. Only 'a|r' are valid\n";
      }
    } else if (!strcmp(argv[i], "--as-list")) {
      flags_as |= FLAS_VIEW;
    } else if (!strcmp(argv[i], "--as-clear")) {
      flags_as |= FLAS_CLEAR;
    } else if (!strcmp(argv[i], "--as-on")) {
      flags_as |= FLAS_ACT;
      if (argc > (i + 1)) {
        i++;
        as_idx_act = (int)strtol(argv[i], NULL, 0);
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--as-off")) {
      flags_as |= FLAS_DEACT;
      if (argc > (i + 1)) {
        i++;
        as_idx_deact = (int)strtol(argv[i], NULL, 0);
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--as-del")) {
      flags_as |= FLAS_DEL;
      if (argc > (i + 1)) {
        i++;
        as_idx_del = (int)strtol(argv[i], NULL, 0);
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--as-save")) {
      if (argc > (i + 1)) {
        i++;
        flags_as |= FLAS_RETR;
        strcpy(assfn, argv[i]);
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--as-load")) {
      if (argc > (i + 1)) {
        i++;
        flags_as |= FLAS_STOR;
        strcpy(aslfn, argv[i]);
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--silent")) {
      flags |= FL_SILENT;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "off"))
          silent = 0;
        else if (!strcmp(argv[i], "on"))
          silent = 1;
      }
    } else if (!strcmp(argv[i], "--sm-load")) {
      flags |= FL_SILENT;
      silent = 1;
      silent_tray = 1;
      if (argc > (i + 1)) {
        i++;
        silent_load = (int)strtol(argv[i], NULL, 0);
        //              std::cout << "tray load speed: " << silent_load << "\n";
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--sm-eject")) {
      flags |= FL_SILENT;
      silent = 1;
      silent_tray = 1;
      if (argc > (i + 1)) {
        i++;
        silent_eject = (int)strtol(argv[i], NULL, 0);
        //              std::cout << "tray load speed: " << silent_load << "\n";
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--sm-access")) {
      flags |= FL_SILENT;
      silent = 1;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "fast"))
          silent_access = SILENT_ACCESS_FAST;
        else if (!strcmp(argv[i], "slow"))
          silent_access = SILENT_ACCESS_SLOW;
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--sm-cd-rd")) {
      flags |= FL_SILENT;
      silent = 1;
      silent_cd = 1;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "48"))
          silent_cd_rd = SILENT_CD_RD_48X;
        else if (!strcmp(argv[i], "40"))
          silent_cd_rd = SILENT_CD_RD_40X;
        else if (!strcmp(argv[i], "32"))
          silent_cd_rd = SILENT_CD_RD_32X;
        else if (!strcmp(argv[i], "24"))
          silent_cd_rd = SILENT_CD_RD_24X;
        else if (!strcmp(argv[i], "8"))
          silent_cd_rd = SILENT_CD_RD_8X;
        else if (!strcmp(argv[i], "4"))
          silent_cd_rd = SILENT_CD_RD_4X;
        else
          std::cout << "invalid --sm-cd-rd parameter: " << argv[i] << "\n";
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--sm-cd-wr")) {
      flags |= FL_SILENT;
      silent = 1;
      silent_cd = 1;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "48"))
          silent_cd_wr = SILENT_CD_WR_48X;
        else if (!strcmp(argv[i], "32"))
          silent_cd_wr = SILENT_CD_WR_32X;
        else if (!strcmp(argv[i], "24"))
          silent_cd_wr = SILENT_CD_WR_24X;
        else if (!strcmp(argv[i], "16"))
          silent_cd_wr = SILENT_CD_WR_16X;
        else if (!strcmp(argv[i], "8"))
          silent_cd_wr = SILENT_CD_WR_8X;
        else if (!strcmp(argv[i], "4"))
          silent_cd_wr = SILENT_CD_WR_4X;
        else
          std::cout << "invalid --sm-cd-wr parameter: " << argv[i] << "\n";
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--sm-dvd-rd")) {
      flags |= FL_SILENT;
      silent = 1;
      silent_dvd = 1;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "16"))
          silent_dvd_rd = SILENT_DVD_RD_16X;
        else if (!strcmp(argv[i], "12"))
          silent_dvd_rd = SILENT_DVD_RD_12X;
        else if (!strcmp(argv[i], "8"))
          silent_dvd_rd = SILENT_DVD_RD_8X;
        else if (!strcmp(argv[i], "5"))
          silent_dvd_rd = SILENT_DVD_RD_5X;
        else if (!strcmp(argv[i], "2"))
          silent_dvd_rd = SILENT_DVD_RD_2X;
        else
          std::cout << "invalid --sm-dvd-rd parameter: " << argv[i] << "\n";
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    }
    /*
            else if(!strcmp(argv[i],"--sm-dvd-wr")) {
                flags |= FL_SILENT; silent = 1; silent_dvd = 1;
                if (argc>(i+1)) {
                    i++;
                    if      (!strcmp(argv[i],"18")) silent_dvd_wr =
       SILENT_DVD_WR_18X; else if (!strcmp(argv[i],"16")) silent_dvd_wr =
       SILENT_DVD_WR_12X; else if (!strcmp(argv[i],"12")) silent_dvd_wr =
       SILENT_DVD_WR_12X; else if (!strcmp(argv[i], "8")) silent_dvd_wr =
       SILENT_DVD_WR_8X; else if (!strcmp(argv[i], "6")) silent_dvd_wr =
       SILENT_DVD_WR_6X; else if (!strcmp(argv[i], "4")) silent_dvd_wr =
       SILENT_DVD_WR_4X; else std::cout << "invalid --sm-dvd-wr parameter: " <<
       argv[i] << "\n"; } else { needs_parameter(argv, i); return 5;
                }
            }
    */
    else if (!strcmp(argv[i], "--sm-nosave"))
      silent_save = 0;
    else if (!strcmp(argv[i], "--destruct")) {
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "quick")) {
          flags |= FL_PXERASER;
          pxeraser = PLEXERASER_QUICK;
        }
        if (!strcmp(argv[i], "full")) {
          flags |= FL_PXERASER;
          pxeraser = PLEXERASER_FULL;
        }
      } else {
        needs_parameter(argv, i);
        return 5;
      }
    } else if (!strcmp(argv[i], "--amqr")) {
      flags |= FL_YMH_AMQR;
      if (argc > (i + 1)) {
        int amqr = 0;
        i++;
        if (!strcmp(argv[i], "off"))
          amqr = 0;
        else if (!strcmp(argv[i], "on"))
          amqr = 1;
      }
    } else if (!strcmp(argv[i], "--forcespeed")) {
      flags |= FL_YMH_FORCESPEED;
      if (argc > (i + 1)) {
        int forcespeed = 0;
        i++;
        if (!strcmp(argv[i], "off"))
          forcespeed = 0;
        else if (!strcmp(argv[i], "on"))
          forcespeed = 1;
      }

      //  std::cout << "\t--pio-limit [on|off]         limit (or not) read speed
      //  by 24x for CD and 8x for DVD\n"; std::cout << "\t--pio-quiet
      //  [quiet|perf|std] select QuietMode setting: Quiet, Performance or
      //  Standard (default: quiet)\n"; std::cout << "\t--pio-nosave don't save
      //  settings to drive (changes will be lost after reboot)\n";

    } else if (!strcmp(argv[i], "--pio-nosave")) {
      silent_save = 0;
    } else if (!strcmp(argv[i], "--pio-limit")) {
      flags |= FL_PIOLIMIT;
      if (argc > (i + 1)) {
        i++;
        if (!strcmp(argv[i], "off"))
          piolimit = 0;
        else if (!strcmp(argv[i], "on"))
          piolimit = 1;
      }
    } else if (!strcmp(argv[i], "--pio-quiet")) {
      flags |= FL_PIOQUIET;
      if (argc > (i + 1)) {
        char piosilent = PIO_SILENT_QUIET;
        i++;
        if (!strcmp(argv[i], "quiet"))
          piosilent = PIO_SILENT_QUIET;
        else if (!strcmp(argv[i], "perf"))
          piosilent = PIO_SILENT_PERF;
        else if (!strcmp(argv[i], "std"))
          piosilent = PIO_SILENT_STD;
        else
          std::cout << "invalid --pio-quiet parameter: " << argv[i] << "\n";
      } else {
        needs_parameter(argv, i);
        return 5;
      }

    } else if (!strcmp(argv[i], "-v"))
      flags |= FL_VERBOSE;
    else if (!strcmp(argv[i], "--verbose"))
      flags |= FL_VERBOSE;
    else {
      std::cout << "unknown option: " << argv[i] << "\n";
      return 6;
    }
  }
  if (flags & FL_HELP) {
    usage(argv[0]);
    print_opts();
    return 0;
  }
  if (!flags) {
    usage(argv[0]);
    print_opts();
    return 1;
  }

  if (flags & FL_SCAN) {
    int drvcnt = 0;
    drvcnt = scanbus(DEV_PLEXTOR | DEV_YAMAHA);
    if (!drvcnt)
      std::cout << "ERR: no drives found!\n";
    return 2;
  }
  if (!(flags & FL_DEVICE)) {
    std::cout << "** ERR: no device selected\n";
    return 3;
  }
  //  std::cout << "____________________________\n";
  std::cout << "Device : " << device << "\n";
  dev = new drive_info(device);
  if (dev->err) {
    std::cout << argv[0] << ": can't open device: " << device << "\n";
    delete dev;
    return 4;
  }

  inquiry(dev);
  //  convert_to_ID(dev);
  std::cout << "Vendor : '" << dev->ven << "\n";
  std::cout << "Model  : '" << dev->dev << "'";
  if (isPlextor(dev)) {
    plextor_get_TLA(dev);
    std::cout << " (TLA#" << dev->TLA << ")";
  }
  std::cout << "\nF/W    : '" << dev->fw << "'\n";

  if (!(flags & FL_VERBOSE))
    dev->silent++;
  if (get_drive_serial_number(dev))
    std::cout << "Serial#: " << dev->serial << "\n";

  if (flags) {
    //  if (flags & FL_VERBOSE) {
    std::cout << "\nCDVD Control flags : ";
    if (flags & FL_VERBOSE)
      std::cout << " VERBOSE";
    if (flags & FL_DEVICE)
      std::cout << " DEVICE";
    if (flags & FL_HELP)
      std::cout << " HELP";
    if (flags & FL_CURRENT)
      std::cout << " CURRENT";
    if (flags & FL_SUPPORTED)
      std::cout << " SUPPORTED";
    if (flags & FL_SCAN)
      std::cout << " SCAN";
    if (flags & FL_LOCK)
      std::cout << " LOCK";
    if (flags & FL_HCDR)
      std::cout << " HCDR";
    if (flags & FL_SSS)
      std::cout << " SSS";
    if (flags & FL_SPDREAD)
      std::cout << " SPDREAD";
    if (flags & FL_POWEREC)
      std::cout << " POWEREC";
    if (flags & FL_GIGAREC)
      std::cout << " GIGAREC";
    if (flags & FL_VARIREC_CD)
      std::cout << " VARIREC_CD";
    if (flags & FL_VARIREC_DVD)
      std::cout << " VARIREC_DVD";
    if (flags & FL_SECUREC)
      std::cout << " SECUREC";
    if (flags & FL_BOOK_R)
      std::cout << " BITSET_R";
    if (flags & FL_BOOK_RDL)
      std::cout << " BITSET_RDL";
    if (flags & FL_TESTWRITE)
      std::cout << " DVD+TESTWRITE";
    if (flags & FL_AS)
      std::cout << " AS";
    if (flags & FL_SILENT)
      std::cout << " SILENT";
    if (flags & FL_YMH_AMQR)
      std::cout << " AMQR";
    if (flags & FL_YMH_FORCESPEED)
      std::cout << " FORCESPEED";
    if (flags & FL_PIOLIMIT)
      std::cout << " PIOLIMIT";
    if (flags & FL_PIOQUIET)
      std::cout << " PIOQUIET";
    if (flags & FL_LOEJ)
      std::cout << " LOEJ";
    if (flags_as & FLAS_RETR)
      std::cout << " AS_RETR";
    if (flags_as & FLAS_STOR)
      std::cout << " AS_STOR";
    if (flags_as & FLAS_CREATE)
      std::cout << " AS_CREATE";
    if (flags_as & FLAS_DEL)
      std::cout << " AS_DEL";
    if (flags_as & FLAS_VIEW)
      std::cout << " AS_VIEW";
    if (flags_as & FLAS_ACT)
      std::cout << " AS_ACT";
    if (flags_as & FLAS_DEACT)
      std::cout << " AS_DEACT";
    if (flags_as & FLAS_CLEAR)
      std::cout << " AS_CLEAR";
    std::cout << "\n\n";
  }
  //  std::cout << "____________________________\n";
  if (flags & FL_LOCK) {
    //      dev->silent++;
    switch (lock) {
    case 0:
      std::cout << "Unlocking media...\n";
      dev->parms.status &= (~STATUS_LOCK);
      break;
    case 1:
      std::cout << "Locking media...\n";
      dev->parms.status |= STATUS_LOCK;
      break;
    case 2:
      std::cout << "Toggle media lock state...\n";
      get_lock(dev);
      dev->parms.status ^= STATUS_LOCK;
      break;
    }
    set_lock(dev);
    std::cout << "Media is "
              << ((dev->parms.status & STATUS_LOCK) ? "" : " NOT")
              << " locked\n";
    //      dev->silent--;
  }

  if (flags & FL_LOEJ) {
    //      std::cout << "loej_immed: " << loej_immed << "\n";
    if (eject == 2) {
      load_eject(dev, loej_immed);
    } else {
      load_eject(dev, !eject, loej_immed);
    }
  }

  // PLEXTOR features
  if (flags & FL_POWEREC) {
    //      std::cout << "Set PoweREC...\n";
    dev->plextor.powerec_state = powerec;
    plextor_set_powerec(dev);
  }
  /*
      if (flags & FL_YMH_AMQR) {
          dev->yamaha.amqr = amqr;
          yamaha_set_amqr(dev);
      }
      if (flags & FL_YMH_FORCESPEED) {
          dev->yamaha.forcespeed = forcespeed;
          yamaha_set_forcespeed(dev);
      }
  */
  if (flags & FL_GIGAREC) {
    //      std::cout << "Set GigaREC...\n";
    dev->plextor.gigarec = gigarec;
    plextor_set_gigarec(dev);
  }
  if (flags & FL_VARIREC_CD) {
    //      std::cout << "Set VariREC CD...\n";
    //      std::cout << "PWR = %02X   STR = %02X\n",varirec_cd_pwr,
    //      varirec_cd_str);
    dev->plextor.varirec_state_cd = !(flags & FL_VARIREC_CD_OFF);
    dev->plextor.varirec_pwr_cd = varirec_cd_pwr;
    dev->plextor.varirec_str_cd = varirec_cd_str;
    plextor_set_varirec(dev, VARIREC_CD);
  }
  if (flags & FL_VARIREC_DVD) {
    //      std::cout << "Set VariREC DVD...\n";
    //      std::cout << "PWR = %02X   STR = %02X\n",varirec_dvd_pwr,
    //      varirec_dvd_str);
    dev->plextor.varirec_state_dvd = !(flags & FL_VARIREC_DVD_OFF);
    dev->plextor.varirec_pwr_dvd = varirec_dvd_pwr;
    dev->plextor.varirec_str_dvd = varirec_dvd_str;
    plextor_set_varirec(dev, VARIREC_DVD);
  }
  if (flags & FL_NOSECUREC) {
    plextor_set_securec(dev, 0, NULL);
  } else if (flags & FL_SECUREC) {
    int pwdlen = (char)strlen(passwd);
    if ((pwdlen >= 4) && (pwdlen <= 10))
      plextor_set_securec(dev, pwdlen, passwd);
    else
      std::cout << "Invalid SecuRec password length! must be 4..10\n";
  }
  if (flags & FL_HCDR) {
    plextor_set_hidecdr(dev, hcdr);
  }
  if (flags & FL_SSS) {
    plextor_set_singlesession(dev, sss);
  }
  if (flags & FL_SPDREAD) {
    plextor_set_speedread(dev, spdread);
  }
  if (flags & FL_BOOK_R) {
    dev->book_plus_r = bookr;
    plextor_set_bitset(dev, PLEX_BITSET_R);
  }
  if (flags & FL_BOOK_RDL) {
    dev->book_plus_rdl = bookrdl;
    plextor_set_bitset(dev, PLEX_BITSET_RDL);
  }
  if (flags & FL_AS) {
    dev->astrategy.state = as;
    plextor_set_autostrategy(dev);
  }

  if (flags & FL_MQCK) {
    if (dev->media.type & (DISC_CDROM | DISC_DDCD_ROM | DISC_DVDROM |
                           DISC_BD_ROM | DISC_HDDVD_ROM)) {
      std::cout << "Can't run MQCK on stamped media!\n";
      return 5;
    }
    detect_speeds(dev);

    std::cout << "Available write speeds:\n";
    std::cout << "WR speed max: "
              << (((float)dev->parms.max_write_speed_kb) /
                  dev->parms.speed_mult)
              << "X (" << dev->parms.max_write_speed_kb << " kB/s)\n";
    for (int i = 0; i < speed_tbl_size && dev->parms.wr_speed_tbl_kb[i] > 0;
         i++) {
      std::cout << "  speed #" << i << ":"
                << (((float)dev->parms.wr_speed_tbl_kb[i]) /
                    ((float)dev->parms.speed_mult))
                << "X"
                << "(" << dev->parms.wr_speed_tbl_kb[i] << " kB/s)\n";
    }
    if (as_mqck >= 0) {
      dev->parms.max_write_speed_kb =
          (int)(as_mqck_spd * dev->parms.speed_mult),
      set_rw_speeds(dev);
      get_rw_speeds(dev);
      if (!(dev->media.type & (DISC_DVD))) {
        std::cout << "MQCK: Media Quality Check supported on DVD media only!\n";
        return 5;
      }
      std::cout << "Starting media check at speed "
                << (((float)dev->parms.max_write_speed_kb) /
                    dev->parms.speed_mult)
                << " X...\n";
      if (plextor_media_check(dev, as_mqck)) {
        if ((dev->err & 0x0FFF00) == 0x023A00)
          std::cout << "MQCK: No media found\n";
        else
          std::cout << "MQCK: Error starting Media Check\n";
        return 5;
      }
      if (!dev->rd_buf[0x11]) {
        if ((dev->rd_buf[0x10] & 0xFF) == 0xFF)
          std::cout << "MQCK: Cant' run Media Check: AUTOSTRATEGY is OFF\n";
        else
          std::cout << "MQCK: Media is GOOD for writing at selected speed\n";
      } else {
        switch (dev->rd_buf[0x10]) {
        case 0x01:
          std::cout << "MQCK: Write error may occur\n";
          break;
        case 0x03:
          std::cout
              << "MQCK: Drive may not write correctly at selected speed\n";
          break;
        default:
          std::cout << "MQCK: Unknown MQCK error: 0x" << dev->rd_buf[0x10]
                    << " X\n";
        }
      }
    }
  }

  dev->silent--;
  if (flags_as & FLAS_VIEW) {
    plextor_get_autostrategy_db_entry_count(dev);
    plextor_get_autostrategy_db(dev);
  }
  if (flags_as & FLAS_CLEAR) {
    std::cout << "deleting all AutoStrategy entries...\n";
    plextor_clear_autostrategy_db(dev);
  }
  if (flags_as & FLAS_ACT) {
    std::cout << "activating strategy #" << as_idx_act << "\n";
    plextor_modify_autostrategy_db(dev, as_idx_act, ASDB_ENABLE);
  }
  if (flags_as & FLAS_DEACT) {
    std::cout << "DEactivating strategy #" << as_idx_deact << "\n";
    plextor_modify_autostrategy_db(dev, as_idx_deact, ASDB_DISABLE);
  }
  if (flags_as & FLAS_DEL) {
    std::cout << "DELETING strategy #" << as_idx_del << "\n";
    plextor_modify_autostrategy_db(dev, as_idx_del, ASDB_DELETE);
  }
  if (flags_as & FLAS_CREATE) {
    std::cout << "AS: Creating new strategy, mode: "
              << ((ascre & ASDB_CRE_FULL) == ASDB_CRE_FULL ? "FULL" : "QUICK")
              << ", action: "
              << ((ascre & ASDB_ADD) == ASDB_ADD ? "ADD" : "REPLACE")
              << "...\n";
    plextor_create_strategy(dev, ascre);
  }
  if (flags_as & FLAS_RETR) {
    unsigned char hdr[9];
    memset(hdr, 0, 9);
    std::cout << "AS RETR...\n";
    plextor_get_strategy(dev);
    std::cout << "Saving AS DB...\n";
    asf = fopen(assfn, "wb");
    if (!asf) {
      std::cout << "can't create asdb file!\n";
      return 6;
    }
    memcpy(hdr, "ASDB ", 5);
    hdr[5] = dev->astrategy.dbcnt;
    fwrite((void *)hdr, 1, 8, asf);
    for (i = 0; i < dev->astrategy.dbcnt; i++) {
      fwrite((void *)&dev->astrategy.entry[i], 1, 0x20, asf);
      for (int j = 0; j < 7; j++)
        fwrite((void *)&dev->astrategy.entry_data[i][j], 1, 0x20, asf);
    }
    fclose(asf);
  }
  if (flags_as & FLAS_STOR) {
    unsigned char hdr[9];
    memset(hdr, 0, 9);
    std::cout << "AS STOR...\n";

    std::cout << "Loading AS DB...\n";
    asf = fopen(aslfn, "rb");
    if (!asf) {
      std::cout << "can't open asdb file!\n";
      return 6;
    }
    if (fread((void *)hdr, 1, 8, asf) < 8) {
      std::cout << "error reading asdb file!\n";
      return 6;
    }

    if (!strncmp((char *)hdr, "ASDB ", 5))
      dev->astrategy.dbcnt = hdr[5];
    i = 0;
    while (!feof(asf) && i < dev->astrategy.dbcnt) {
      if (fread((void *)&dev->astrategy.entry[i], 1, 0x20, asf) < 0x20) {
        std::cout << "error reading asdb file!\n";
        return 6;
      }
      for (int j = 0; j < 7; j++)
        if (fread((void *)&dev->astrategy.entry_data[i][j], 1, 0x20, asf) <
            0x20) {
          std::cout << "error reading asdb file!\n";
          return 6;
        }

      i++;
    }
    fclose(asf);
    if (dev->astrategy.dbcnt != i) {
      std::cout << "Can't read all strategies! File corrupted!\n";
      return 5;
    }
    std::cout << dev->astrategy.dbcnt
              << " strategies loaded, sending to drive...\n";
    plextor_add_strategy(dev);
  }
  if (flags_as & (FLAS_ACT | FLAS_DEACT | FLAS_DEL | FLAS_CREATE | FLAS_STOR |
                  FLAS_CLEAR)) {
    std::cout << "AutoStrategy DB modified...\n";
    plextor_get_autostrategy_db_entry_count(dev);
    plextor_get_autostrategy_db(dev);
  }
  dev->silent++;
  if (flags & FL_TESTWRITE) {
    dev->plextor.testwrite_dvdplus = testwrite;
    plextor_set_testwrite_dvdplus(dev);
  }
  if (flags & FL_SILENT) {
    plextor_get_silentmode(dev);
    if (!silent)
      plextor_set_silentmode_disable(dev, silent_save);
    else {
      if (!dev->plextor_silent.state) {
        silent_cd = 1;
        silent_dvd = 1;
        silent_tray = 1;
      }
      if (silent_cd) {
        dev->plextor_silent.access = silent_access;
        dev->plextor_silent.rd = silent_cd_rd;
        dev->plextor_silent.wr = silent_cd_wr;
        plextor_set_silentmode_disc(dev, SILENT_CD, silent_save);
      }
      if (silent_dvd) {
        dev->plextor_silent.access = silent_access;
        dev->plextor_silent.rd = silent_dvd_rd;
        //              dev->plextor_silent.wr      = silent_dvd_wr;
        plextor_set_silentmode_disc(dev, SILENT_DVD, silent_save);
      }
      if (silent_tray) {
        dev->plextor_silent.load = silent_load;
        dev->plextor_silent.eject = silent_eject;
        plextor_set_silentmode_tray(dev, SILENT_CD, silent_save);
      }
    }
  }
  if (flags & FL_PXERASER) {
    dev->plextor.plexeraser = pxeraser;
    plextor_plexeraser(dev);
  }

  // PIONEER features
  if (flags & FL_PIOQUIET) {
    pioneer_set_silent(dev, silent, silent_save);
  }
  if (flags & FL_PIOLIMIT) {
    pioneer_set_spdlim(dev, piolimit, silent_save);
  }

  get_device_info(dev);
  if (!(flags & FL_VERBOSE))
    dev->silent--;
  delete dev;
  return 0;
}
