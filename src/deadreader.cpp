/*
 *
 * DeadDiscReader
 * Copyright (C) 2006-2009, Gennady "ShultZ" Kozlov <qpxtool@mail.ru>
 * it uses QPxTool SCSI transport library
 *
 */

#define MAX_THREADS 8
#define _FILE_OFFSET_BITS 64

#include "kbhit.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
//#include <sys/stat.h>
#include "qpx_mmc.hpp"

#include "imgwriter.hpp"
#include "reader.hpp"
#include "sectmap.hpp"
#define VERSION "1.0"

int init_drives(drive_info **dev, char **dev_n) {
  std::cout << "Initialising drives...\n";
  int i = 0;
  int n = 0;
  while ((dev_n[i]) && (i < MAX_THREADS)) {
    dev[n] = new drive_info(dev_n[i]);
    if (!inquiry(dev[n])) {
      dev[n]->silent++;
      detect_capabilities(dev[n]);
      std::cout << n << ": [" << dev[n]->device << "] " << dev[n]->ven << dev[n]->dev << dev[n]->fw << "\n";
      n++;
    } else {
      std::cout << "Can't open device '" << dev_n[i] << "!\n";
    }
    i++;
  }
  std::cout << n << " devices found\n";
  return n;
}

int sort_drives(drive_info **dev) {
  bool swap = 1;
  int i;
  int devcnt = 0;
  while (swap) {
    swap = 0;
    for (i = 0; i < (MAX_THREADS - 1); i++)
      if ((!dev[i]) && (dev[i + 1])) {
        dev[i] = dev[i + 1];
        dev[i + 1] = NULL;
        swap = 1;
      }
  }
  for (i = 0; i < MAX_THREADS; i++)
    if (dev[i])
      devcnt++;
  return devcnt;
}

int read_multi(cdvdreader **reader, int devcnt, smap *map, int pass,
               int retry = 4) {
  keyboard kb;
  char c;
  int i;

  //    if (verbose) printf("Starting threads...\n");
  for (i = 0; i < devcnt; i++) {
    reader[i]->set_pass(pass);
    reader[i]->set_retry(retry);
    //      printf("Starting thread %d\n", i);
    reader[i]->start();
  }

  while (reader[0]->running()) {
    msleep(1);
    for (; kb.kb_hit();) {
      c = kb.kb_getch();
      switch (c) {
      case 'w':
      case 'W':
        map->lock();
        map->save();
        map->unlock();
        break;
      case 'q':
      case 'Q':
        for (i = 0; i < devcnt; i++)
          reader[i]->stop();
        break;
      default:
        break;
      }
    }
  }
  std::cout << "\nWaiting for reader threads...\n";
  for (i = 0; i < devcnt; i++)
    reader[i]->wait();
  return 0;
}

void usage(char *av0) {
  std::cout << "usage: " << av0 << "[device_list] [options]\n";
  std::cout << "device_list - you may specify multiple reader devices:\n";
  std::cout << "\t-d /dev/dvd0 [-d /dev/dvd1 [-d /dev/dvd2[..]]]\n";
  std::cout << "options:\n";
  std::cout << "\t-o <image_file>\tsave image to this file\n";
  std::cout << "\t-s <speed>\tset read speed (-1 = maximum)\n";
  std::cout << "\t-v, -vv\t\tbe verbose\n";
  std::cout << "\t-l,-s\t\tscan IDE/SCSI bus and exit\n";
  std::cout << "\t-h\t\tshow this help and exit\n";
  std::cout << "\ncontrol keys:\n";
  std::cout << "\t q  - stop reading and exit\n";
  std::cout << "\t w  - save current sector map and continue\n\n";
}

#if defined(_WIN32)
BOOL WINAPI sigint_handler(DWORD) {
  printf("\nSIGINT\n");
  return true;
}
#endif

int main(int argc, char **argv) {
  int spd = -1;
  int devcnt = 0;
  drive_info *dev[MAX_THREADS];
  char *dev_n[MAX_THREADS];
  cdvdreader *reader[MAX_THREADS];
  smap *map = NULL;
  imgwriter *iso = NULL;
  //  FILE	*f_img;
  int32_t capacity;
  int done;
  char *n_img = 0;
  char n_map[1024];
  //    unsigned int  fc, dc, ic;
  //    bool	rfail=0;
  //    bool	rcont=0;
  int i;
  int verbose = 0;

  std::cout << "**  DVD reader v" << VERSION << " (c) 2006-2009  Gennady \"ShultZ\" Kozlov  **\n";
  std::cout << "**  multiple-device DVD reader with CSS support, optimized for "
         "corrupted media\n";

  if (argc < 2) {
    //	usage(argv[0]);
    std::cout << "No option specified!\nUse -h for details\n";
    return 1;
  }

  for (i = 1; i < MAX_THREADS; i++) {
    dev[i] = 0;
    dev_n[i] = 0;
    reader[i] = 0;
  }

  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h")) {
      usage(argv[0]);
      return 0;
    } else if (!strcmp(argv[i], "-l")) {
      scanbus();
      return 0;
    } else if (!strcmp(argv[i], "-vv")) {
      verbose = 2;
    } else if (!strcmp(argv[i], "-v")) {
      verbose = 1;
    } else if (!strcmp(argv[i], "-s")) {
      if (argc > (i + 1)) {
        i++;
        spd = atol(argv[i]);
      } else {
        std::cout << "Option " << argv[i] << " needs a parameter!\n";
      }
    } else if (!strcmp(argv[i], "-o")) {
      if (argc > (i + 1)) {
        i++;
        n_img = argv[i];
      } else {
        std::cout << "Option " << argv[i] << " needs a parameter!\n";
      }
    } else if (!strcmp(argv[i], "-d")) {
      if (argc > (i + 1)) {
        i++;
        if (devcnt < MAX_THREADS) {
          dev_n[devcnt] = argv[i];
          devcnt++;
        } else {
          std::cout << "Maximum devices limit reached: " << MAX_THREADS << "\n";
        }
      } else {
        std::cout << "Option " << argv[i] << " needs a parameter!\n";
      }
    } else {
      std::cout << "Option not recognized: " << argv[i] << "\n";
    }
  }

  if (!devcnt) {
    std::cout << " No devices selected!\n";
    exit(1);
  }
  if (!n_img) {
    std::cout << " No image file selected!\n";
    exit(2);
  }

  strcpy(n_map, n_img);
  strcat(n_map, ".smap");
  std::cout << " image file   : '" << n_img << "'\n";
  std::cout << " bitmap file  : '" << n_map << "'\n";
  std::cout << "\n";

  devcnt = init_drives(dev, dev_n);
  std::cout << " Checking media...\n";
  for (i = 0; i < devcnt; i++) {
    get_rw_speeds(dev[i]);
    determine_disc_type(dev[i]);
    dev[i]->parms.read_speed_kb = spd * 1350;
    set_rw_speeds(dev[i]);

    if (!dev[i]->media.type) {
      std::cout << dev[i]->device << ": NO media!\n";
    } else {
      int mi = 0;
      while (MEDIA[mi].id != 0xFFFFFFFF &&
             (dev[i]->media.type & (~DISC_CDRWSUBT)) != MEDIA[mi].id)
        mi++;
      std::cout << dev[i]->device << ": media type: " << MEDIA[mi].name << "\n";
    }
    if (!(dev[i]->media.type & DISC_DVD)) {
      std::cout << dev[i]->device << ": no DVD found! removing device from list...\n";
      dev[i] = NULL;
      // exit (3);
    } else {
      read_capacities(dev[i]);
    }
  }
  devcnt = sort_drives(dev);
  if (!devcnt) {
    std::cout << " No discs detected!\n";
    exit(3);
  }
  capacity = dev[0]->media.capacity;
  if (devcnt > 1) {
    std::cout << "Using " << dev[0]->device << " as primary device\nChecking capacities (expecting " << capacity << " sectors)...";
    for (i = 1; i < devcnt; i++) {
      std::cout << dev[i]->device << ": " << dev[i]->media.capacity << "  ";
      if (dev[i]->media.capacity == dev[0]->media.capacity) {
        std::cout << "OK\n";
      } else {
        std::cout << "failed! removing device from list...\n";
        dev[i] = NULL;
      }
    }
  }
  devcnt = sort_drives(dev);
  std::cout << "using " << devcnt << " devices\n";

  map = new smap(n_map, capacity);
  iso = new imgwriter(n_img, map);

#if defined(_WIN32)
  SetConsoleCtrlHandler(&sigint_handler, 1);
#endif

  if (verbose)
    std::cout << "Initializing threads...\n";
  for (i = 0; i < devcnt; i++) {
    reader[i] = new cdvdreader(i, 0, dev[i], map, iso);
  }

  map->lock();
  done = map->get_done();
  map->unlock();
  if (!done) {
    std::cout << "Starting first reading pass...\n";
    read_multi(reader, devcnt, map, PASS_FIRST);
  } else {
    if (done < capacity) {
      std::cout << "Continue reading incomplete image...\n";
      read_multi(reader, devcnt, map, PASS_CONT);
    }
  }

  map->lock();
  done = map->get_fail();
  map->unlock();
  if (!done || reader[0]->stoped())
    goto dreader_done;

  std::cout << "There are some corrupted sectors!\nStarting first RECOVERY pass...\n";
  read_multi(reader, devcnt, map, PASS_RECOVER0, 1);

  map->lock();
  done = map->get_fail();
  map->unlock();
  if (!done || reader[0]->stoped())
    goto dreader_done;

  std::cout << "There are hard corrupted sectors!\nStarting long RECOVERY pass...\n";
  read_multi(reader, devcnt, map, PASS_RECOVER1, 4);

dreader_done:
  if (verbose)
    std::cout << "\nDestroying readers...\n";
  for (i = 0; i < devcnt; i++) {
    reader[i]->print_stat();
    delete reader[i];
  }
  std::cout << "Image summary:\n";
  std::cout << " Sectors tot      : " << map->get_tot() << "\n";
  std::cout << " Sectors wait     : " << map->get_wait() << "\n";
  std::cout << " Sectors read     : " << map->get_read()<< "\n";
  std::cout << " Sectors done     : " << map->get_done() << "\n";
  std::cout << " Sectors corrupted: " << map->get_fail() << "\n";
  //    printf("Closing image file...\n");
  //    fclose(f_img);

  //	map->save();

  //    printf("Destroing drive_info*...\n");
  for (i = 0; i < devcnt; i++)
    delete dev[i];
  //    printf("Destroing imgwriter...\n");
  delete iso;
  //    printf("Destroing smap...\n");
  delete map;
  return 0;
}
