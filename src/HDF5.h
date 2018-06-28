//
// Created by anthony on 12/10/16.
//

#ifndef IRIS_HDF5_H
#define IRIS_HDF5_H

#include "API.h"
#include "hdf5.h"



/* File Interface */

IRIS_FORWARD_DECL(H5Fopen, hid_t, (const char *name, unsigned flags, hid_t fapl_id));
IRIS_FORWARD_DECL(H5Dopen2,hid_t, (hid_t loc_id, const char *name, hid_t dapl_id));
IRIS_FORWARD_DECL(H5Fcreate, hid_t, (const char *name, unsigned flags, hid_t fcpl_id, hid_t fapl_id));

IRIS_FORWARD_DECL(H5Screate_simple, hid_t, (int
    rank,
    const hsize_t *current_dims,
    const hsize_t *maximum_dims));

IRIS_FORWARD_DECL(H5Dcreate2, hid_t, (hid_t
    loc_id,
    const char *name, hid_t
    dtype_id,
        hid_t
    space_id, hid_t
    lcpl_id, hid_t
    dcpl_id, hid_t
    dapl_id));

IRIS_FORWARD_DECL(H5Dwrite, hid_t, (hid_t
    dataset_id, hid_t
    mem_type_id, hid_t
    mem_space_id,
        hid_t
    file_space_id, hid_t
    xfer_plist_id,
    const void *buf));

IRIS_FORWARD_DECL(H5Dread, hid_t, (hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id,
    hid_t file_space_id, hid_t xfer_plist_id, void *buf));
IRIS_FORWARD_DECL(H5Dclose, herr_t, (hid_t dataset_id));
IRIS_FORWARD_DECL(H5Sclose, herr_t, (hid_t space_id));
IRIS_FORWARD_DECL(H5Fclose, herr_t, (hid_t file_id));
hid_t H5Fopen(const char *name, unsigned flags, hid_t fapl_id);
hid_t H5Dopen2(hid_t loc_id, const char *name, hid_t dapl_id);

hid_t H5Fcreate(const char *name, unsigned flags, hid_t fcpl_id, hid_t fapl_id);

hid_t H5Screate_simple(int rank, const hsize_t *current_dims,
                       const hsize_t *maximum_dims);

hid_t H5Dcreate2(hid_t loc_id, const char *name, hid_t dtype_id,
                 hid_t space_id, hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id);

herr_t H5Dwrite(hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id,
                hid_t file_space_id, hid_t xfer_plist_id, const void *buf);

herr_t H5Dread(hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id,
               hid_t file_space_id, hid_t xfer_plist_id, void *buf);

herr_t H5Dclose(hid_t dataset_id);

herr_t H5Sclose(hid_t space_id);

herr_t H5Fclose(hid_t file_id);

#endif //IRIS_HDF5_H
