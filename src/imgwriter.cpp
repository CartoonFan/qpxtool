/*
 *
 * image writer class for DeadDiscReader
 * Copyright (C) 2007, 2010, Gennady "ShultZ" Kozlov <qpxtool@mail.ru>
 *
 */

#define _FILE_OFFSET_BITS 64

#include <cstdio>
#include <inttypes.h>
#include <iostream>
#include <unistd.h>

#include "imgwriter.hpp"
#include "sectmap.hpp"

#ifndef HAVE_FOPEN64
#define fopen64 fopen
#endif

imgwriter::imgwriter(char *fn, smap *map) {
  mutex = new Mutex();
  fname = fn;

#if !(defined(HAVE_FSEEKO) && defined(OFFT_64BIT)) && !defined(HAVE_FSEEK64)
  std::cout << "Warning! No 64-bit file offset. Image size limits to 2GiB\n";
#endif
  if (!(iso = fopen64(fname, "r+"))) {
    std::cout << "can't open image file, creating new one!\n";
    if (!(iso = fopen64(fname, "w+"))) {
      std::cout << "can't create image file!\n";
    }
  } else {
    std::cout << "image opened: '" << fname << "'\n";
    map->load();
  }
  if (iso)
    fclose(iso);
}

imgwriter::~imgwriter() { delete mutex; }

int imgwriter::write(int lba, int scnt, int ssz, void *buff) {
  int res = 0;
#if defined(HAVE_FSEEKO) && defined(OFFT_64BIT)
  off_t offs = ssz * (off_t)lba;
#else
  int64_t offs = ssz * (int64_t)lba;
#endif

  mutex->lock();
  iso = fopen64(fname, "r+");

  if (iso) {
#if defined(HAVE_FSEEKO) && defined(OFFT_64BIT)
    if (fseeko(iso, offs, SEEK_SET))
#elif defined(HAVE_FSEEK64)
    if (fseek64(iso, offs, SEEK_SET))
#else
    if (fseek(iso, offs, SEEK_SET))
#endif
    {
      std::cout << "\nseek() failed! Offs: " << offs << " (" << offs << ")\n";
      mutex->unlock();
      return 0;
    }
    res = fwrite(buff, ssz, scnt, iso);
    //		printf("\nwrote: %ld of %ld\n", res, scnt);
    fclose(iso);
  }
  mutex->unlock();
  return res;
}

// void	imgwriter::set_file(char* fn) { fname=fn; }

// int	imgwriter::open(){}

// int	imgwriter::close(){}
