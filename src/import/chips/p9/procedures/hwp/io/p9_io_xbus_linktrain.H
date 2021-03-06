/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/io/p9_io_xbus_linktrain.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2016                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
///
/// @file p9_io_xbus_linktrain.H
/// @brief I/O Link Training on the Xbus Links.
///
///-----------------------------------------------------------------------------
/// *HWP HWP Owner        : Chris Steffen <cwsteffen@us.ibm.com>
/// *HWP HWP Backup Owner : Gary Peterson <garyp@us.ibm.com>
/// *HWP FW Owner         : Jamie Knight <rjknight@us.ibm.com>
/// *HWP Team             : IO
/// *HWP Level            : 2
/// *HWP Consumed by      : FSP:HB
///-----------------------------------------------------------------------------
#ifndef _P9_IO_XBUS_TRAIN_H
#define _P9_IO_XBUS_TRAIN_H

//-----------------------------------------------------------------------------
//  System Header Include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  fapi2 Includes
//-----------------------------------------------------------------------------
#include <fapi2.H>

typedef fapi2::ReturnCode (*p9_io_xbus_linktrain_FP_t)(
    const fapi2::Target < fapi2::TARGET_TYPE_XBUS >&,
    const uint8_t&,
    const fapi2::Target < fapi2::TARGET_TYPE_XBUS >&,
    const uint8_t& );

extern "C"
{

    /**
     * @brief A HWP that runs on every instance of the XBUS(EDI+)
     * @param[in] i_mtarget Reference to the Master Target
     * @param[in] i_mgroup  Master Target Clock Group
     * @param[in] i_starget Reference to the Slave Target
     * @param[in] i_sgroup  Slave Target Clock Group
     * @retval    ReturnCode
     */
    fapi2::ReturnCode p9_io_xbus_linktrain(
        const fapi2::Target < fapi2::TARGET_TYPE_XBUS >& i_mtarget,
        const uint8_t&                                   i_mgroup,
        const fapi2::Target < fapi2::TARGET_TYPE_XBUS >& i_starget,
        const uint8_t&                                   i_sgroup );

} // extern "C"

#endif // _P9_IO_XBUS_TRAIN_H
