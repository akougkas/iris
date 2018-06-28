//
// Created by anthony on 1/23/17.
//

#include "hybrid.h"
#include "hyperdex.h"
#include <hyperdex/client.h>
#include <hyperdex/admin.h>
#include <cstring>
#include <inttypes.h>


long hybrid::replay(std::string trace, std::string pvfs_file, int rank) {
  if(rank==0) printf("\n################ HYBRID MODE ################\n");
  /*Initialization of stuff*/
  FILE* trace_fh = nullptr;
  FILE* file_fh = nullptr;
  long request_size=0,bytes_read=0,bytes_written=0;
  long offset=0,total_size=0;
  char key[32];
  int lineNumber=1;
  std::size_t len=0;
  ssize_t readsize;
  char* line = (char*) malloc(128);
  char* word;
  enum operation_type oper = NONE;
  char * write_buffer = nullptr;
  void * read_buffer = nullptr;
  struct hyperdex_client* client;
  client = hyperdex_client_create(coordinator.c_str(),
                                  coordinator_port[rank%NUM_CORD]);
  /* make sure the client is OK */
  if (!client) {
    fprintf(stderr, "Cannot create Hyperdex client: %d.\n", rank);
    return -1;
  }

  /*Opening trace*/
  trace_fh = fopen(trace.c_str(), "r");
  if(trace_fh == nullptr) return -1;

  /*While loop to read each line from the trace and create I/O*/
  while ((readsize = getline(&line, &len, trace_fh))!= -1) {
    if (readsize < 3) break;

    enum hyperdex_client_returncode op_status, loop_status;
    int64_t op_id=0, loop_id=0;

    word = strtok(line, ",");
    if (strcmp(word, "WRITE") == 0) {
      oper = WRITE_OPER;
    } else if (strcmp(word, "READ") == 0) {
      oper = READ_OPER;
    } else if (strcmp(word, "LSEEK") == 0) {
      oper = SEEK_OPER;
    } else if (strcmp(word, "FOPEN") == 0) {
      oper = OPEN_OPER;
    } else if (strcmp(word, "FCLOSE") == 0) {
      oper = CLOSE_OPER;
    }
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    request_size = atol(word);
    sprintf(key,"%ld_%d",(offset + request_size), rank);
    std::size_t keySize = std::strlen(key);

    switch (oper) {
      case OPEN_OPER:
        file_fh = std::fopen(pvfs_file.c_str(), "r+");
        if (file_fh == nullptr) {
          fprintf(stderr, "%s -> %p\n", pvfs_file.c_str
              (), file_fh);
          return -2;
        }
        break;
      case CLOSE_OPER:
        std::fclose(file_fh);
        break;
      case SEEK_OPER:
        std::fseek(file_fh, request_size, SEEK_SET);
        break;
      case WRITE_OPER:
        if(request_size<THRESHOLD) {
          write_buffer = randstring(request_size);
          struct hyperdex_client_attribute attribute;
          attribute.attr = "data";
          attribute.datatype = HYPERDATATYPE_STRING;
          attribute.value = write_buffer;
          attribute.value_sz = (size_t) request_size;

          /* put one object per rank into hyperdex */
          op_id = hyperdex_client_put(
              client,
              space,
              key,
              keySize,
              &attribute,
              1,
              &op_status);
          loop_id = hyperdex_client_loop(client, -1, &loop_status);
          if (loop_id != op_id || loop_status != HYPERDEX_CLIENT_SUCCESS) {
            fprintf(stderr, "Put failed for client %d!!\nOp_id: %ld  "
                "Op_status:%d "
                "Loop_id: %ld\n", rank, op_id, op_status, loop_id);
          }
        }
          else{
          write_buffer = randstring(request_size);
          long curOffset=ftell(file_fh) <= request_size ?0:-request_size;
          std::fseek(file_fh,  curOffset, SEEK_CUR);
          bytes_written = std::fwrite(write_buffer, sizeof(char), request_size,
                                      file_fh);
          if(bytes_written<request_size) fprintf(stderr, "File write failed\n");
          //else printf("Written %ld bytes\n", bytes_written);
        }
        total_size +=request_size;
        if(write_buffer) free(write_buffer);
        break;
      case READ_OPER:
        if(request_size<THRESHOLD) {
          const struct hyperdex_client_attribute *attributes;
          size_t attributes_sz;
          attributes = 0;
          attributes_sz = 0;
          op_id = hyperdex_client_get(client,
                                      space,
                                      key,
                                      keySize,
                                      &op_status,
                                      &attributes,
                                      &attributes_sz);
          loop_id = hyperdex_client_loop(client, -1, &loop_status);
          if (op_id == loop_id && loop_status == HYPERDEX_CLIENT_SUCCESS &&
              attributes_sz != 0) {
            read_buffer = malloc(attributes[0].value_sz);
            memcpy(read_buffer, attributes[0].value, attributes[0].value_sz);
            total_size += attributes[0].value_sz;
            /*printf("Get object %s with size: %ld\n", key,
                   attributes[0].value_sz);*/
          } //else fprintf(stderr, "Get of %s failed\n", key);
          hyperdex_client_destroy_attrs(attributes, attributes_sz);
        }
        else{
          long curOffset=ftell(file_fh) <= request_size ?0:-request_size;
          read_buffer = malloc((size_t) request_size);
          std::fseek(file_fh,  curOffset, SEEK_CUR);
          bytes_read = std::fread(read_buffer, sizeof(char), request_size,
                                  file_fh);
          if (bytes_read < request_size) fprintf(stderr, "File read failed\n");
          //else printf("Read %ld bytes\n", bytes_read);
          total_size += request_size;
        }
        if(read_buffer) free(read_buffer);
        break;
      default:
        fprintf(stderr, "Operation not allowed\n");
        break;
    }

  lineNumber++;
}
if (line) free(line);
std::fclose(trace_fh);
return total_size;
}

long hybrid::prepare(std::string trace, int rank) {
  if(rank==0) printf("\n################ PREPARING DATA ################\n");
  /*Initialization of stuff*/
  FILE* trace_fh = nullptr;
  long offset=0,value=0,total_size=0;
  char key[32];
  int lineNumber=1;
  std::size_t len=0;
  ssize_t readsize;
  char* line = (char*) malloc(128);
  char* word;
  enum operation_type oper = NONE;
  char * write_buffer = nullptr;
  void * read_buffer = nullptr;
  struct hyperdex_client* client;
  client = hyperdex_client_create(coordinator.c_str(),
                                  coordinator_port[rank%NUM_CORD]);
  /* make sure the client is OK */
  if (!client) {
    fprintf(stderr, "Cannot create Hyperdex client: %d.\n", rank);
    return -1;
  }

  /*Opening trace*/
  trace_fh = fopen(trace.c_str(), "r");
  if(trace_fh == nullptr) return -1;

  /*While loop to read each line from the trace and create I/O*/
  while ((readsize = getline(&line, &len, trace_fh))!= -1) {
    if (readsize < 3) break;
    enum hyperdex_client_returncode op_status, loop_status;
    int64_t op_id=0, loop_id=0;

    //printf("Line: %d\n", lineNumber);
    word = strtok(line, ",");
    if (strcmp(word, "WRITE") == 0) {
      oper = WRITE_OPER;
    } else if (strcmp(word, "READ") == 0) {
      oper = READ_OPER;
    } else if (strcmp(word, "LSEEK") == 0) {
      oper = SEEK_OPER;
    } else if (strcmp(word, "FOPEN") == 0) {
      oper = OPEN_OPER;
    } else if (strcmp(word, "FCLOSE") == 0) {
      oper = CLOSE_OPER;
    }
    word = strtok(NULL, ",");
    offset = atol(word);
    word = strtok(NULL, ",");
    value = atol(word);
    sprintf(key,"%ld_%d",(offset + value), rank);
    std::size_t keySize = std::strlen(key);

    if(value < THRESHOLD) {
      switch (oper) {
        case OPEN_OPER:
          break;
        case CLOSE_OPER:
          break;
        case SEEK_OPER:
          break;
        case WRITE_OPER:
          break;
        case READ_OPER:
          write_buffer = randstring(value);
          struct hyperdex_client_attribute attribute;
          attribute.attr = "data";
          attribute.datatype = HYPERDATATYPE_STRING;
          attribute.value = write_buffer;
          attribute.value_sz = (size_t) value;
          //printf("Putting key %s with value: %ld\n", key, value);
          /* put one object per rank into hyperdex */
          op_id = hyperdex_client_put(
              client,
              space,
              key,
              keySize,
              &attribute,
              1,
              &op_status);
          loop_id = hyperdex_client_loop(client, -1, &loop_status);
          if (loop_id != op_id || loop_status != HYPERDEX_CLIENT_SUCCESS) {
            fprintf(stderr,
                    "Put failed for client %d!!\nOp_id: %ld  Op_status:%d "
                        "Loop_id: %ld\n", rank, op_id, op_status, loop_id);
          } else {
            total_size += value;
            //printf("Putting key %s Total size: %ld\n", key, total_size);
          }
          break;
        default:
          fprintf(stderr, "Operation not allowed\n");
          break;
      }
    }
    lineNumber++;
  }
  if (line) free(line);
  std::fclose(trace_fh);
  return total_size;
}

int hybrid::init() {
  int i=0;
  int status=0;
  for(i=0;i<NUM_CORD;i++){
    printf("Initializing Hyperdex...\n");
    /* create the admin */
    struct hyperdex_admin* admin=hyperdex_admin_create(coordinator.c_str(), coordinator_port[i]);
    if (!admin) {
      fprintf(stderr, "Cannot create hyperdex ADMIN.\n");
    }
    /* remove the old space */
    enum hyperdex_admin_returncode admin_status, loop_status;
    int64_t admin_id=0, loop_id=0;
    admin_id = hyperdex_admin_rm_space(admin, space, &admin_status);
    loop_id = hyperdex_admin_loop(admin, -1, &loop_status);
    if (loop_id!=admin_id || admin_status!=HYPERDEX_ADMIN_SUCCESS) {
      fprintf(stderr, "Failed to remove old spaces!\n");
    }
    /* add a space */
    admin_id=0; loop_id=0;
    admin_id = hyperdex_admin_add_space(admin, desc, &admin_status);
    loop_id = hyperdex_admin_loop(admin, -1, &loop_status);
    if (loop_id!=admin_id || admin_status!=HYPERDEX_ADMIN_SUCCESS) {
      fprintf(stderr,"Failed to create space!\n");
      status=-1;
    } else {
      /*List existing spaces*//*
      const char * spaces;
      admin_id=0; loop_id=0;
      admin_id = hyperdex_admin_list_spaces(admin,&admin_status, &spaces);
      loop_id = hyperdex_admin_loop(admin, -1, &loop_status);
      if (loop_id!=admin_id || admin_status!=HYPERDEX_ADMIN_SUCCESS){
        fprintf(stderr,"Failed to list space!\n");
      }
      else{
        printf("List Spaces: %s\n", spaces);
      }*/
      admin_id = hyperdex_admin_validate_space(admin, desc, &admin_status);
      loop_id = hyperdex_admin_loop(admin, -1, &loop_status);
      if (admin_status!=HYPERDEX_ADMIN_SUCCESS){
        fprintf(stderr,"Failed to validate space!\n");
      }
      else{
        printf("Success creating spaces: %s!\n", space);
      }
    }
    hyperdex_admin_destroy(admin);
  }
  return status;
}

char *hybrid::randstring(long length) {
  int n;
  static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
  char *randomString = NULL;
  if (length) {
    randomString = (char *) malloc(sizeof(char) * (length + 1));
    if (randomString) {
      for (n = 0; n < length; n++) {
        int key = rand() % (int) (sizeof(charset) - 1);
        randomString[n] = charset[key];
      }
      randomString[length] = '\0';
    }
  }
  return randomString;
}
