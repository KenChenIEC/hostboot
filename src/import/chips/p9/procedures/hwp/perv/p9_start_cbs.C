/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/perv/p9_start_cbs.C $      */
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
//------------------------------------------------------------------------------
/// @file  p9_start_cbs.C
///
/// @brief Start CBS : Trigger CBS
//------------------------------------------------------------------------------
// *HWP HW Owner        : Abhishek Agarwal <abagarw8@in.ibm.com>
// *HWP HW Backup Owner : Srinivas V Naga <srinivan@in.ibm.com>
// *HWP FW Owner        : sunil kumar <skumar8j@in.ibm.com>
// *HWP Team            : Perv
// *HWP Level           : 2
// *HWP Consumed by     : SE:HB
//------------------------------------------------------------------------------


//## auto_generated
#include "p9_start_cbs.H"
//## auto_generated
#include "p9_const_common.H"

#include <p9_perv_scom_addresses.H>
#include <p9_perv_scom_addresses_fld.H>
#include <p9_perv_scom_addresses_fixes.H>
#include <p9_perv_scom_addresses_fld_fixes.H>

enum P9_START_CBS_Private_Constants
{
    P9_CFAM_CBS_POLL_COUNT = 600, // Observed Number of times CBS read for CBS_INTERNAL_STATE_VECTOR
    CBS_IDLE_VALUE = 0x002, // Read the value of CBS_CS_INTERNAL_STATE_VECTOR
    P9_CBS_IDLE_HW_NS_DELAY = 100000, // unit is nano seconds
    P9_CBS_IDLE_SIM_CYCLE_DELAY = 250000 // unit is sim cycles
};

fapi2::ReturnCode p9_start_cbs(const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>
                               & i_target_chip,
                               const bool i_sbe_start)
{
    fapi2::buffer<uint32_t> l_read_reg ;
    bool l_read_vdn_pgood_status = false;
    bool l_sbe_start_value = false;
    bool l_fsi2pib_status = false;
    fapi2::buffer<uint32_t> l_data32;
    fapi2::buffer<uint32_t> l_data32_cbs_cs;
    int l_timeout = 0;
    FAPI_INF("p9_start_cbs: Entering ...");

    l_sbe_start_value = !i_sbe_start;

    FAPI_DBG("Configuring Prevent SBE start option");
    FAPI_IMP("SBE start value : %d", l_sbe_start_value);
    //Setting CBS_CS register value
    FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_CBS_CS_FSI,
                                    l_data32_cbs_cs));
    //CFAM.CBS_CS.CBS_CS_PREVENT_SBE_START = l_sbe_start_value
    l_data32_cbs_cs.writeBit<3>(l_sbe_start_value);
    FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_CBS_CS_FSI,
                                    l_data32_cbs_cs));

    FAPI_DBG("check for VDN_PGOOD");
    //Getting PERV_CBS_ENVSTAT register value
    FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_CBS_ENVSTAT_FSI,
                                    l_data32));
    l_read_vdn_pgood_status =
        l_data32.getBit<PERV_CBS_ENVSTAT_C4_VDN_GPOOD>();  //l_read_vdn_pgood_status = PERV_CBS_ENVSTAT.PERV_CBS_ENVSTAT_C4_VDN_GPOOD

    FAPI_ASSERT(l_read_vdn_pgood_status,
                fapi2::VDN_PGOOD_NOT_SET(),
                "ERROR:VDN PGOOD OFF, CBS_ENVSTAT BIT 2 NOT SET");

    FAPI_DBG("Resetting CFAM Boot Sequencer (CBS) to flush value");
    //Setting CBS_CS register value
    FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_CBS_CS_FSI,
                                    l_data32_cbs_cs));
    l_data32_cbs_cs.clearBit<0>();  //CFAM.CBS_CS.CBS_CS_START_BOOT_SEQUENCER = 0
    FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_CBS_CS_FSI,
                                    l_data32_cbs_cs));

    // HW319150 - pervSoA:  cbs_start is implemented as pulse 0 -> 1
    FAPI_DBG("Triggering CFAM Boot Sequencer (CBS) to start");
    //Setting CBS_CS register value
    l_data32_cbs_cs.setBit<0>();  //CFAM.CBS_CS.CBS_CS_START_BOOT_SEQUENCER = 1
    FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_CBS_CS_FSI,
                                    l_data32_cbs_cs));

    FAPI_DBG("Check cbs_cs_internal_state_vector");
    l_timeout = P9_CFAM_CBS_POLL_COUNT;

    //UNTIL CBS_CS.CBS_CS_INTERNAL_STATE_VECTOR == CBS_IDLE_VALUE
    while (l_timeout != 0)
    {
        //Getting CBS_CS register value
        FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_CBS_CS_FSI,
                                        l_data32_cbs_cs));
        uint32_t l_poll_data = 0;    //uint32_t l_poll_data = CFAM.CBS_CS.CBS_CS_INTERNAL_STATE_VECTOR
        l_data32_cbs_cs.extractToRight<16, 16>(l_poll_data);

        if (l_poll_data == CBS_IDLE_VALUE)
        {
            break;
        }

        fapi2::delay(P9_CBS_IDLE_HW_NS_DELAY, P9_CBS_IDLE_SIM_CYCLE_DELAY);
        --l_timeout;
    }

    FAPI_DBG("Loop Count :%d", l_timeout);

    FAPI_ASSERT(l_timeout > 0,
                fapi2::CBS_CS_INTERNAL_STATE(),
                "ERROR: CBS_CS_INTERNAL_STATE_VECTOR HAS NOT REACHED IDLE STATE VALUE 0x002 ");

    FAPI_DBG("check for VDD status");
    //Getting FSI2PIB_STATUS register value
    FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_FSI2PIB_STATUS_FSI,
                                    l_data32));
    //l_fsi2pib_status = CFAM.FSI2PIB_STATUS.VDD_NEST_OBSERVE
    l_fsi2pib_status = l_data32.getBit<PERV_FSI2PIB_STATUS_VDD_NEST_OBSERVE>();

    FAPI_ASSERT(l_fsi2pib_status,
                fapi2::VDD_NEST_OBSERVE(),
                "ERROR:VDD OFF, FSI2PIB  BIT 16 NOT SET");

    FAPI_INF("p9_start_cbs: Exiting ...");

fapi_try_exit:
    return fapi2::current_err;

}
