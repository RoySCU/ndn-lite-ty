//
//  dummy_face.c
//  riot-forwarder
//
//  Created by UCLA on 10/31/18.
//  Copyright © 2018 UCLA. All rights reserved.
//

#include "dummy-face.h"
#include <stdlib.h>
#include <memory.h>

///////////////////////////////////

typedef struct ndn_dummy_face_extension{
  uint16_t used_len;
  uint8_t buf[NDN_DUMMY_FACE_BUFFER_SIZE];
} ndn_dummy_face_extension_t;

static uint8_t dummy_face_errno;

int
dummy_face_send(struct ndn_face* self, const uint8_t* packet, uint32_t size);

int
dummy_face_close(struct ndn_face* self);

void
dummy_face_destroy(ndn_face_t* self);

const ndn_iface_t ndn_dummy_face_klass = {
  sizeof(ndn_dummy_face_extension_t),
  dummy_face_send,
  dummy_face_close,
  dummy_face_destroy
};

////////////////////////////////////

void
dummy_face_destroy(ndn_face_t* self)
{
  // Useless.
  free(self->extension);
}

void
dummy_face_init(ndn_face_t* self)
{
  self->klass = NDN_KLASS_DUMMY_FACE;
  // NOTE: We don't include dummy face in the real forwarder
  // It's okay to allocate memory in Tests without freeing them.
  self->extension = malloc(NDN_KLASS_DUMMY_FACE->size);
}

int
dummy_face_close(struct ndn_face* self)
{
  self->state = NDN_FACE_CLOSED;
  return 0;
}

int
dummy_face_send(struct ndn_face* self, const uint8_t* packet, uint32_t size)
{
  ndn_dummy_face_extension_t *recvBuf = self->extension;
  if(recvBuf->used_len + size + 2 > NDN_DUMMY_FACE_BUFFER_SIZE){
    return NDN_ERROR_OVERSIZE;
  }
  
  // Record size & packet
  uint8_t *ptr = recvBuf->buf + recvBuf->used_len;
  *(uint16_t*)ptr = (size & 0xFFFF);
  ptr += 2;
  memcpy(ptr, packet, size);
  
  return 0;
}