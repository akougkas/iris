/*******************************************************************************
* File return_codes.h
*
* Goal: This is the all the error codes from IRIS calls.
*
* Created: December 10th, 2016  - Anthony Kougkas
 * Updated:
* Illinois Institute of Technology - SCS Lab
* (C) 2016
******************************************************************************/
#ifndef IRIS_RETURN_CODES_H
#define IRIS_RETURN_CODES_H
/******************************************************************************
*error codes enum
******************************************************************************/
typedef enum iris_return_codes{
  OPERATION_SUCCESSFUL               = 7777,
  NO_PREFETCHING_REQUIRED            = 7778,

  /* Error codes*/
  METADATA_CREATION_FAILED           = 7800,
  FP_DOES_NOT_EXIST                  = 7801,
  FILENAME_DOES_NOT_EXIST            = 7802,
  METADATA_UPDATE_FAILED__OPEN       = 7803,
  METADATA_UPDATE_FAILED__CLOSE      = 7804,
  METADATA_UPDATE_FAILED__READ       = 7805,
  METADATA_UPDATE_FAILED__WRITE      = 7806,
  UPDATE_FILE_POINTER_FAILED         = 7807,
  PREFETCH_ENGINE_FAILED             = 7808,
  FILE_SEEK_FAILED                       = 7809,
  FH_DOES_NOT_EXIST                  = 7810,
  FILE_READ_FAILED                   = 7811,
  FILE_WRITE_FAILED                  = 7812,

  CONTAINER_NOT_VALID                = 7821,
  OBJECT_NOT_FOUND                   = 7822,

  HYPERDEX_ADMIN_CREATION_FAILED     = 7900,
  HYPERDEX_CLIENT_CREATION_FAILED    = 7901,
  HYPERDEX_GET_OPERATION_FAILED      = 7902,
  HYPERDEX_PUT_OPERATION_FAILED      = 7903,

  MONGO_CLIENT_CREATION_FAILED       = 7910,
  MONGO_DB_CONNECTION_FAILED         = 7911,
  MONGO_COLLECTION_CREATION_FAILED   = 7913,
  MONGO_GET_OPERATION_FAILED         = 7914,
  MONGO_PUT_OPERATION_FAILED         = 7915,
  MONGO_GET_NOT_FOUND                = 7916,

  NO_CACHED_DATA_FOUND               = 7950
} iris_returncode;

#endif //IRIS_RETURN_CODES_H
