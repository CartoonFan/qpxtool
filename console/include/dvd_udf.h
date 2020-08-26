/* -*- c-basic-offset: 2; indent-tabs-mode: nil -*- */
#ifndef DVD_UDF_H_INCLUDED
#define DVD_UDF_H_INCLUDED

/*
 * This code is based on dvdudf by:
 *   Christian Wolff <scarabaeus@convergence.de>.
 *
 * Modifications by:
 *   Billy Biggs <vektor@dumbterm.net>.
 *   Björn Englund <d4bjorn@dtek.chalmers.se>.
 *
 * dvdudf: parse and read the UDF volume information of a DVD Video
 * Copyright (C) 1999 Christian Wolff for convergence integrated media
 * GmbH The author can be reached at scarabaeus@convergence.de, the
 * project's page is at http://linuxtv.org/dvd/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  Or, point your browser to
 * http://www.gnu.org/copyleft/gpl.html
 */

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

//#include "dvd_reader.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

#define DVD_VIDEO_LB_LEN      2048
#define MAX_UDF_FILE_NAME_LEN 2048

/**
 * Looks for a file on the UDF disc/imagefile and returns the block number
 * where it begins, or 0 if it is not found.  The filename should be an
 * absolute pathname on the UDF filesystem, starting with '/'.  For example,
 * '/VIDEO_TS/VTS_01_1.IFO'.  On success, filesize will be set to the size of
 * the file in bytes.
 * This implementation relies on that the file size is less than 2^32
 * A DVD file can at most be 2^30 (-2048 ?).
 */

#include <qpx_mmc.h>

struct udf_t {
    drive_info *dev;
    void*      cache;
};

unsigned int UDFFindFile( udf_t *udf, char *filename, unsigned int *size );

void FreeUDFCache( udf_t *udf, void *cache);

int UDFGetVolumeIdentifier( udf_t *udf,
                            char *volid, unsigned int volid_size);
int UDFGetVolumeSetIdentifier( udf_t *udf,
                               unsigned char *volsetid, unsigned int volsetid_size);

//#ifdef __cplusplus
//};
//#endif
#endif /* DVD_UDF_H_INCLUDED */

