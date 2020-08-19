/*
 * This file is part of the QPxTool project.
 * Copyright (C) 2005-2006,2009 Gennady "ShultZ" Kozlov <qpxtool@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include <array>
#include <cstdio>
#include <cstdlib>

#include <qpx_mmc.h>
#include <yamaha_features.h>

#ifdef USE_LIBPNG
#include <png.h>
#endif

#include "version.h"

const uint32_t FL_HELP = 0x00000001;
const uint32_t FL_SCAN = 0x00000002;
const uint32_t FL_DEVICE = 0x00000004;
const uint32_t FL_VERBOSE = 0x00000008;
const uint32_t FL_CURRENT = 0x00000010;
const uint32_t FL_SUPPORTED = 0x00000020;
const uint32_t FL_TATTOO_TEST = 0x00000040;
const uint32_t FL_TATTOO_RAW = 0x00000080;
#ifdef USE_LIBPNG
const uint32_t FL_TATTOO_PNG = 0x00000100;
const uint32_t FL_TATTOO = FL_TATTOO_RAW | FL_TATTOO_PNG | FL_TATTOO_TEST;
#else
const uint32_t FL_TATTOO = FL_TATTOO_RAW | FL_TATTOO_TEST;
#endif

uint32_t flags = 0;

auto get_device_info(drive_info *drive) -> int {
  drive->ven_features = 0;
  drive->chk_features = 0;
  detect_capabilities(drive);
  //	detect_check_capabilities(drive);
  determine_disc_type(drive);
  if (isYamaha(drive) == 0) {
    printf("%s: drive not supported\n", drive->device);
    return 1;
  }
  //	if (!yamaha_check_amqr(drive)) drive->ven_features|=YMH_AMQR;
  //	if (!yamaha_check_forcespeed(drive))
  // drive->ven_features|=YMH_FORCESPEED;
  if (yamaha_f1_get_tattoo(drive) == 0) {
    drive->ven_features |= YMH_TATTOO;
  }

  if ((flags & FL_SUPPORTED) != 0U) {
    printf("\n** Supported features:\n");
    //		printf("AudioMaster Q.R.    : %s\n", drive->ven_features & YMH_AMQR
    //? "YES" : "---"); 		printf("ForceSpeed          : %s\n",
    //drive->ven_features & YMH_FORCESPEED ? "YES" : "---");
    printf("DiscT@2             : %s\n",
           (drive->ven_features & YMH_TATTOO) != 0U ? "YES" : "---");
  }

  if ((flags & FL_CURRENT) != 0U) {
    printf("\n** Current drive settings:\n");
  }
  if ((flags & (FL_CURRENT | FL_TATTOO | FL_TATTOO_TEST)) != 0U &&
      (drive->ven_features & YMH_TATTOO) != 0U) {
    if ((drive->yamaha.tattoo_rows) != 0U) {
      printf("DiscT@2 info:\ninner: %d\nouter: %d\nimage: 3744x%d\n",
             drive->yamaha.tattoo_i, drive->yamaha.tattoo_o,
             drive->yamaha.tattoo_rows);
    } else {
      if ((drive->media.type & DISC_CD) != 0U) {
        printf("Can't write DiscT@2 on inserted disc!\n");
      } else {
        printf("No disc found! Can't get DiscT@2 info!\n");
      }
    }
  }
  return 0;
}

#ifdef USE_LIBPNG
static auto my_png_get_image_width(png_structp png_ptr, png_infop info_ptr)
    -> int {
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  return png_get_image_width(png_ptr, info_ptr);
#else
  return info_ptr->width;
#endif
}

static auto my_png_get_image_height(png_structp png_ptr, png_infop info_ptr)
    -> int {
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  return png_get_image_height(png_ptr, info_ptr);
#else
  return info_ptr->height;
#endif
}

static auto my_png_get_color_type(png_structp png_ptr, png_infop info_ptr)
    -> png_byte {
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  return png_get_color_type(png_ptr, info_ptr);
#else
  return info_ptr->color_type;
#endif
}

static auto my_png_get_valid(png_structp png_ptr, png_infop info_ptr,
                             png_uint_32 flags) -> png_uint_32 {
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  return png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE);
#else
  return (info_ptr->valid & flags);
#endif
}

static auto my_png_get_bit_depth(png_structp png_ptr, png_infop info_ptr)
    -> int {
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  return png_get_bit_depth(png_ptr, info_ptr);
#else
  return info_ptr->bit_depth;
#endif
}

static auto my_png_get_rowbytes(png_structp png_ptr, png_infop info_ptr)
    -> int {
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  return png_get_rowbytes(png_ptr, info_ptr);
#else
  return info_ptr->rowbytes;
#endif
}

auto tattoo_read_png(unsigned char *buf, uint32_t rows, FILE *fp) -> bool {
  const int byte_size = 8;
  png_byte header[byte_size]; // 8 is the maximum size that can be checked
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  uint32_t number_of_passes = 0;
  png_bytep png_row_pointer = nullptr;
  unsigned char *raw_row_pointer = nullptr;
  //	unsigned char *tp = NULL;

  //	int width;
  uint32_t row = 0;
  uint32_t col = 0;
  int c = 0;
  int32_t r = 0;
  int32_t g = 0;
  int32_t b = 0;
  int num_palette = 0;
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  png_colorp palette = nullptr;
#endif

  if (fread(header, 1, byte_size, fp) < byte_size) {
    printf("Error reading PNG header\n");
    fclose(fp);
    return true;
  }
  if (png_sig_cmp(header, 0, byte_size) != 0) {
    printf("File not recognized as a PNG\n");
    fclose(fp);
    return true;
  }
  png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    printf("png_create_read_struct failed!\n");
    fclose(fp);
    return true;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    printf("png_create_info_struct failed!\n");
    fclose(fp);
    return true;
  }
  if (setjmp(png_jmpbuf(png_ptr))) {
    printf("png_jmpbuf failed!\n");
    fclose(fp);
    return true;
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, byte_size);

  png_read_info(png_ptr, info_ptr);

  printf("Image size: %ld x %ld\n", my_png_get_image_width(png_ptr, info_ptr),
         my_png_get_image_height(png_ptr, info_ptr));

  if (my_png_get_image_width(png_ptr, info_ptr) != 3744U ||
      my_png_get_image_height(png_ptr, info_ptr) != rows) {
    printf("Image should be 3744 x %d", rows);
    return true;
  }

  //	width = info_ptr->width;
  //	height = info_ptr->height;
  //	bit_depth = info_ptr->bit_depth;

#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  number_of_passes = png_set_interlace_handling(png_ptr);
#else
  number_of_passes = png_set_interlace_handling(png_ptr);
#endif
  png_read_update_info(png_ptr, info_ptr);

  printf("Color type: [%d] ", my_png_get_color_type(png_ptr, info_ptr));
  switch (my_png_get_color_type(png_ptr, info_ptr)) {
  case PNG_COLOR_TYPE_GRAY:
    printf("PNG_COLOR_TYPE_GRAY\n");
    break;
  case PNG_COLOR_TYPE_PALETTE:
    printf("PNG_COLOR_TYPE_PALETTE\n");
    if ((my_png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE)) == 0U) {
      printf("PNG color type is indexed, but no palette found!");
      goto err_read_png;
    }
    break;
  case PNG_COLOR_TYPE_RGB:
    printf("PNG_COLOR_TYPE_RGB\n");
    break;
  case PNG_COLOR_TYPE_RGB_ALPHA:
    printf("PNG_COLOR_TYPE_RGB_ALPHA\n");
    break;
  case PNG_COLOR_TYPE_GRAY_ALPHA:
    printf("PNG_COLOR_TYPE_GRAY_ALPHA\n");
    break;
  default:
    printf("unlnown PNG color type!\n");
    goto err_read_png;
  }
  printf("Bit depth : %d\n", my_png_get_bit_depth(png_ptr, info_ptr));
  if (my_png_get_bit_depth(png_ptr, info_ptr) != byte_size) {
    printf("Unsupported bit depth!\n");
    goto err_read_png;
  }

#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
  png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
#else
  num_palette = info_ptr->num_palette;
#endif
  if (my_png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE) != 0U) {
    printf("Palette   : %d colors\n", num_palette);
  } else {
    printf("Palette   : NO\n");
  }
  printf("ROW bytes : %ld\n", my_png_get_rowbytes(png_ptr, info_ptr));

  raw_row_pointer = buf;
  png_row_pointer =
      static_cast<png_byte *>(malloc(my_png_get_rowbytes(png_ptr, info_ptr)));
  for (row = 0; row < rows; row++) {
    if (setjmp(png_jmpbuf(png_ptr))) {
      printf("png_jmpbuf failed!\n");
      goto err_read_png;
    }
    png_read_row(png_ptr, png_row_pointer, nullptr);
    if (my_png_get_image_width(png_ptr, info_ptr) < 3744U) {
      memset(raw_row_pointer, 0, 3744);
    }

    switch (my_png_get_color_type(png_ptr, info_ptr)) {
    case PNG_COLOR_TYPE_GRAY:
      for (col = 0; col < my_png_get_image_width(png_ptr, info_ptr); col++) {
        raw_row_pointer[col] = png_row_pointer[col] ^ 0xFF;
        //					memcpy(raw_row_pointer,
        //png_row_pointer, 3744);
      }
      break;
    case PNG_COLOR_TYPE_PALETTE:
      for (col = 0; col < my_png_get_image_width(png_ptr, info_ptr); col++) {
        c = png_row_pointer[col];
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
        r = palette[c].red;
        g = palette[c].green;
        b = palette[c].blue;
#else
        r = info_ptr->palette[c].red;
        g = info_ptr->palette[c].green;
        b = info_ptr->palette[c].blue;
#endif
        c = (r * 11 + g * 16 + b * 5) / 32;
        raw_row_pointer[col] = c ^ 0xFF;
      }
#if PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 4
      png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
#endif
      break;
    case PNG_COLOR_TYPE_RGB:
      for (col = 0; col < my_png_get_image_width(png_ptr, info_ptr); col++) {
        r = png_row_pointer[col * 3];
        g = png_row_pointer[col * 3 + 1];
        b = png_row_pointer[col * 3 + 2];
        c = (r * 11 + g * 16 + b * 5) / 32;
        raw_row_pointer[col] = c ^ 0xFF;
      }
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      for (col = 0; col < my_png_get_image_width(png_ptr, info_ptr); col++) {
        r = png_row_pointer[col * 4];
        g = png_row_pointer[col * 4 + 1];
        b = png_row_pointer[col * 4 + 2];
        c = (r * 11 + g * 16 + b * 5) / 32;
        raw_row_pointer[col] = c ^ 0xFF;
      }
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      for (col = 0; col < my_png_get_image_width(png_ptr, info_ptr); col++) {
        raw_row_pointer[col] = png_row_pointer[col * 2] ^ 0xFF;
      }
      break;
    }
    raw_row_pointer += 3744;
  }

  //	if (tp) free(tp);
  if (png_row_pointer != nullptr) {
    free(png_row_pointer);
  }
  return true;

err_read_png:
  //	if (tp) free(tp);
  if (png_row_pointer != nullptr) {
    free(png_row_pointer);
  }
  return false;
}
#endif

void usage(char *bin) {
  fprintf(stderr, "\nusage: %s [-d device] [optinos]\n", bin);
#ifdef USE_LIBPNG
  printf("PNG support: YES\n");
#else
  printf("PNG support: NO\n");
#endif
  printf("\t-l, --scanbus                list drives (scan IDE/SCSI bus)\n");
  printf("\t-h, --help                   show help\n");
  printf("\t-c, --current                show current drive settings\n");
  printf("\t-s, --supported              show features supported by drive\n");
  printf("\t--tattoo-raw <tattoo_file>   burn selected RAW image as DiscT@2\n");
  printf("\t--tattoo-png <tattoo_file>   burn selected PNG image as DiscT@2\n");
#ifdef USE_LIBPNG
  printf("\t                             WARNING: f1tattoo compiled without "
         "libpng\n");
#endif
  printf("\t--tattoo-test                burn tattoo test image\n");
  printf("\t-v, --verbose                be verbose\n");
}

auto main(int argc, char *argv[]) -> int {
  int i = 0;
  char *device = nullptr;
  char *tattoofn = nullptr;
  unsigned char *tattoobuf = nullptr;
  drive_info *drive = nullptr;

  printf("**  DiscT@2 writer for Yamaha CRW-F1  v%s (c) 2005-2006,2009  "
         "Gennady \"ShultZ\" Kozlov  **\n",
         VERSION);

  for (i = 1; i < argc; i++) {
    //		printf("arg[%02d]: %s\n",i,argv[i]);
    if (strcmp(argv[i], "-d") == 0) {
      if (argc > (i + 1)) {
        i++;
        flags |= FL_DEVICE;
        device = argv[i];
      } else {
        printf("Option %s needs parameter\n", argv[i]);
        exit(1);
      }
    } else if (strcmp(argv[i], "-h") == 0) {
      flags |= FL_HELP;
    } else if (strcmp(argv[i], "--help") == 0) {
      flags |= FL_HELP;
    } else if (strcmp(argv[i], "-c") == 0) {
      flags |= FL_CURRENT;
    } else if (strcmp(argv[i], "--current") == 0) {
      flags |= FL_CURRENT;
    } else if (strcmp(argv[i], "-l") == 0) {
      flags |= FL_SCAN;
    } else if (strcmp(argv[i], "--scanbus") == 0) {
      flags |= FL_SCAN;
    } else if (strcmp(argv[i], "-s") == 0) {
      flags |= FL_SUPPORTED;
    } else if (strcmp(argv[i], "--supported") == 0) {
      flags |= FL_SUPPORTED;
    } else if (strcmp(argv[i], "-v") == 0) {
      flags |= FL_VERBOSE;
    } else if (strcmp(argv[i], "--verbose") == 0) {
      flags |= FL_VERBOSE;
    } else if (strcmp(argv[i], "--tattoo-test") == 0) {
      flags |= FL_TATTOO_TEST;
    } else if (strcmp(argv[i], "--tattoo-raw") == 0) {
      flags |= FL_TATTOO_RAW;
      if (argc > (i + 1)) {
        i++;
        tattoofn = argv[i];
      } else {
        printf("option %s needs parameter!\n", argv[i]);
        return 5;
      }
    } else if (strcmp(argv[i], "--tattoo-png") == 0) {
#ifdef USE_LIBPNG
      flags |= FL_TATTOO_PNG;
      if (argc > (i + 1)) {
        i++;
        tattoofn = argv[i];
      } else {
        printf("option %s needs parameter!\n", argv[i]);
        return 5;
      }
#else
      printf("Can't use PNG as input file: compiled without libpng");
#endif
    } else {
      printf("Illegal option: %s.\nUse -h for details\n", argv[i]);
      return 6;
    }
  }

  if ((flags & FL_HELP) != 0U) {
    usage(argv[0]);
    return 0;
  }
  if (flags == 0U) {
    usage(argv[0]);
    return 1;
  }
  if ((flags & FL_SCAN) != 0U) {
    int drvcnt = 0;
    drvcnt = scanbus(DEV_YAMAHA);
    if (drvcnt == 0) {
      printf("ERR: no drives found!\n");
    }
    return 2;
  }
  if ((flags & FL_DEVICE) == 0U) {
    printf("** ERR: no device selected\n");
    return 3;
  }

  //	printf("____________________________\n");
  printf("Device : %s\n", device);
  drive = new drive_info(device);
  if ((drive->err) != 0) {
    printf("%s: device open error!\n", argv[0]);
    delete drive;
    return 4;
  }
  inquiry(drive);
  //	convert_to_ID(drive);
  printf("Vendor : '%s'\n", drive->ven);
  printf("Model  : '%s'\n", drive->dev);
  printf("F/W    : '%s'\n", drive->fw);
  if ((flags & FL_VERBOSE) == 0U) {
    drive->silent++;
  }
  if (get_drive_serial_number(drive) != 0) {
    printf("Serial#: %s\n", drive->serial);
  }

  if (flags != 0U) {
    //	if (flags & FL_VERBOSE) {
    printf("\nf1tattoo flags : ");
    if ((flags & FL_DEVICE) != 0U) {
      printf(" DEVICE");
    }
    if ((flags & FL_HELP) != 0U) {
      printf(" HELP");
    }
    if ((flags & FL_CURRENT) != 0U) {
      printf(" CURRENT");
    }
    if ((flags & FL_SCAN) != 0U) {
      printf(" SCAN");
    }
    if ((flags & FL_VERBOSE) != 0U) {
      printf(" VERBOSE");
    }
    if ((flags & FL_SUPPORTED) != 0U) {
      printf(" SUPPORTED");
    }
    if ((flags & FL_TATTOO) != 0U) {
      printf(" TATTOO");
    }
    if ((flags & FL_TATTOO_TEST) != 0U) {
      printf(" TATTOO_TEST");
    }
    printf("\n\n");
  }
  get_device_info(drive);
  //	printf("____________________________\n");

  if ((flags & FL_TATTOO) != 0U) {
    FILE *tattoof = nullptr;
    bool fr = false;
    if ((drive->ven_features & YMH_TATTOO) == 0U) {
      printf("Selected device doesn't have DiscT@2 feature!\n");
      delete drive;
      return 1;
    }
    if ((flags & FL_TATTOO_TEST) != 0U) {
      printf("%s: writing T@2 test image...\n", device);
      yamaha_f1_do_tattoo(drive, nullptr, 0);
    } else {
      tattoof = fopen(tattoofn, "r");
      if (tattoof == nullptr) {
        printf("Can't open tattoo file: %s", tattoofn);
      } else {
        printf("Reading tattoo file...\n");
        tattoobuf = static_cast<unsigned char *>(
            malloc(drive->yamaha.tattoo_rows * 3744));
#ifdef USE_LIBPNG
        if ((flags & FL_TATTOO_PNG) != 0U) {
          fr = tattoo_read_png(tattoobuf, drive->yamaha.tattoo_rows, tattoof);
        } else {
#endif
          memset(tattoobuf, 0, drive->yamaha.tattoo_rows * 3744);
          fr = (fread((void *)tattoobuf, 3744, drive->yamaha.tattoo_rows,
                      tattoof) > 0);
#ifdef USE_LIBPNG
        }
#endif
        fclose(tattoof);
        if (fr) {
          yamaha_f1_do_tattoo(drive, tattoobuf, static_cast<int>(fr) * 3744);
        } else {
          printf("Error reading T@2 image!\n");
        }
        free(tattoobuf);
      }
    }
  }
  if ((flags & FL_VERBOSE) == 0U) {
    drive->silent--;
  }
  delete drive;
  return 0;
}
