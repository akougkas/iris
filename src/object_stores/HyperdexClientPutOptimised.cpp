/******************************************************************************
*include files
******************************************************************************/
#include "HyperdexClientPutOptimised.h"

/******************************************************************************
*Initialization of static
******************************************************************************/
std::shared_ptr<HyperdexClientPutOptimised>
    HyperdexClientPutOptimised::instance = nullptr;
/******************************************************************************
*Interface
******************************************************************************/
int HyperdexClientPutOptimised::put(std::shared_ptr<Key> &key) {
#ifdef TIMER
  Timer timer = Timer(); timer.startTime();
#endif
  struct hyperdex_client_attribute attribute;
  enum hyperdex_client_returncode op_status, loop_status;
  int64_t op_id = 0, loop_id = 0;

  /*Prepare parameters for th actual put call on Hyperdex*/
  std::size_t attribute_sz = 1;
  attribute.attr =ATTRIBUTE_NAME;
  attribute.datatype=HYPERDATATYPE_STRING;
  attribute.value = (const char *) key->data;
  attribute.value_sz = key->size;

  op_id = hyperdex_client_put(hyperdexClient,
                              SPACE,
                              key->name,
                              strlen(key->name),
                              &attribute,
                              attribute_sz,
                              &op_status);
  loop_id = hyperdex_client_loop(hyperdexClient, -1, &loop_status);
  if (loop_id != op_id || loop_status != HYPERDEX_CLIENT_SUCCESS) {
#ifdef DEBUG
    fprintf(stderr, "PUT FAILED! \nOP ID: %ld, STAT: %d, LOOP ID: %ld, STAT:"
            " %d\n", op_id, op_status, loop_id, loop_status);
#endif /* DEBUG*/
    return HYPERDEX_PUT_OPERATION_FAILED;
  }
#ifdef DEBUG
  std::cout << "Put operation complete" << std::endl;
    std::cout << "Key: " << key->name << std::endl;
#endif /* DEBUG*/
#ifdef TIMER
  timer.endTime(__FUNCTION__);
#endif
  return OPERATION_SUCCESSFUL;
}
