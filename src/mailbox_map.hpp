#ifndef MAILBOX_MAP_HPP
#define MAILBOX_MAP_HPP
/*********************************************************************
*
*   HEADER:
*       system definitions for mailbox API. enum mbx_index must match
*       global_mailbox in order for mailbox API to function properly.
*
*   Copyright 2025 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include "mailbox_types.hpp"
#include "mailbox_map_types.hpp"

#include "sys_def.h"

#include <array>

/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/
std::array<mailbox_type, (size_t)mbx_index::NUM_MAILBOX > global_mailbox
{{
/* data, type,                     updt_rt,                  flag,               direction,     destination, source       */
{ 0,     data_type::UINT_32_TYPE,  update_rate::RT_ASYNC,    flag_type::NO_FLAG, direction::RX, RPI_MODULE,  PICO_MODULE },  /* EXAMPLE_INT_MSG */
{ 0.0f,  data_type::FLOAT_32_TYPE, update_rate::RT_5_ROUND,  flag_type::NO_FLAG, direction::TX, PICO_MODULE, RPI_MODULE  },  /* EXAMPLE_FLT_MSG */
}};
/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/

/* mailbox_map.hpp */
#endif