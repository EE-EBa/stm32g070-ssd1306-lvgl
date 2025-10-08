#ifndef EBA_DEF_H
#define EBA_DEF_H

/* public typedef */
#define NAME_SIZE (32)

enum eba_error {
  EBA_OK = 0,
  EBA_ERROR = -1,
  EBA_ERR_EMPTY = -2,
  EBA_ERR_FULL = -3,
  EBA_ERR_TIMEOUT = -4,
  EBA_ERR_BUSY = -5,
  EBA_ERR_NO_MEMORY = -6,
  EBA_ERR_IO = -7,
  EBA_ERR_INVALID = -8,
  EBA_ERR_MEM_OVERLAY = -9,
  EBA_ERR_MALLOC = -10,
  EBA_ERR_NOT_ENOUGH = -11,
  EBA_ERR_NO_SYSTEM = -12,
  EBA_ERR_BUS = -13,

};

#endif
