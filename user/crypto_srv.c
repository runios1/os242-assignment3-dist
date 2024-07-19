#include "kernel/types.h"
#include "user/user.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/fcntl.h"

#include "kernel/crypto.h"

int main(void) {
  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  printf("crypto_srv: starting\n");

  // 2.2
  if(getpid() != 2){
    exit(1);
  }

// int remove_shared_memory_request(void* addr, uint64 size); - from hw pages
// initialize
  void* addr;
  uint64 size;
  int result;

// infinite loop
  while(1)
  {
    // 2.3
    result = take_shared_memory_request(&addr, &size);

    // request in the shared memory
    if(result != -1)
    {
      // 2.4
        struct crypto_op* crypto_op = addr;
        /*
          since this is user-provided data, check that the sizes are within reasonable limits.
          When first receiving the request, check that the state field is set to CRYPTO_OP_STATE_INIT
          and that type is set to either CRYPTO_OP_TYPE_ENCRYPT or CRYPTO_OP_TYPE_DECRYPT. If an error is detected, change the state field to
          CRYPTO_OP_STATE_ERROR to indicate to the client that the request was invalid.
        */
       if(crypto_op->state == CRYPTO_OP_STATE_INIT && (crypto_op->type == CRYPTO_OP_TYPE_ENCRYPT || crypto_op->type == CRYPTO_OP_TYPE_DECRYPT) && crypto_op->data_size > 0 && crypto_op->key_size > 0)
       {
            /*
              ----------------------------------------------------------------
              | uint8 type | uint state | uint64 key_size | uint64 data_size |
              ----------------------------------------------------------------
              |                      char key[key_size]                      |
              ----------------------------------------------------------------
              |                      char data[data_size]                    |
              ----------------------------------------------------------------
            */

            for(uint64 cell = 0; cell < crypto_op->data_size; cell++)
            {
              crypto_op->payload[cell + crypto_op->key_size] = crypto_op->payload[cell + crypto_op->key_size] ^ crypto_op->payload[cell % crypto_op->key_size];
            }

            // 2.5, 2.6
            asm volatile ("fence rw,rw" : : : "memory");
            crypto_op->state = CRYPTO_OP_STATE_DONE;

            if(remove_shared_memory_request(addr, size) == -1)
            {
              exit(1);
            }
       }
       else
       {
          /* 
            Since we want to use memory to prevent
            the client process from accessing the data before the processing is complete, we need to establish a happens-before relationship between the data
            writes and the change to the state field. These properties can be achieved
            by using the following statement in the server process before any change
            to the state field:
            asm volatile ("fence rw,rw" : : : "memory");
          */
          // 2.6
          // error
          asm volatile ("fence rw,rw" : : : "memory");
          crypto_op->state = CRYPTO_OP_STATE_ERROR;
       }
    }
  }

  exit(0);
}
