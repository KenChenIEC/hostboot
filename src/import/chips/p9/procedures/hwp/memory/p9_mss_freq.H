/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/memory/p9_mss_freq.H $     */
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
/// @file p9_mss_freq.H
/// @brief Calculate and save off DIMM frequencies
///
// *HWP HWP Owner: Andre Marin <aamarin@us.ibm.com>
// *HWP HWP Backup: Brian Silver <bsilver@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 1
// *HWP Consumed by: FSP:HB

#ifndef MSS_FREQ_H_
#define MSS_FREQ_H_

#include <fapi2.H>
#include <lib/freq/cas_latency.H>
#include <lib/freq/cycle_time.H>
#include <lib/shared/mss_const.H>
#include <lib/utils/conversions.H>

namespace mss
{

///
/// @brief      Checks for frequency override and sets dimm frequency and timing values
/// @param[in]  i_target mcbist fapi2 target
/// @param[out] o_tCK new cycle time if there is a freq override
/// @return     FAPI2_RC_SUCCESS iff ok
///
inline fapi2::ReturnCode check_for_freq_override(const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target,
        uint64_t& o_tCK)
{
    uint64_t l_freq_override = 0;

    FAPI_TRY(freq_override(i_target, l_freq_override),
             "Failed to override frequency!");

    // If there is no override, don't change anything
    if ( l_freq_override != fapi2::ENUM_ATTR_MSS_FREQ_OVERRIDE_AUTO)
    {
        o_tCK = mss::freq_to_ps(l_freq_override);
        FAPI_DBG( "Override Frequency Detected: %d", l_freq_override);
    }

fapi_try_exit:
    return fapi2::current_err;
}


///
/// @brief      Sets DRAM CAS latency attributes
/// @param[in]  i_target the controller target
/// @param[in]  i_cas_latency final selected CAS ltency
/// @return     FAPI2_RC_SUCCESS iff ok
///
inline fapi2::ReturnCode set_CL_attr(const fapi2::Target<fapi2::TARGET_TYPE_MCS>& i_target,
                                     uint64_t i_cas_latency)
{
    // Declaration of the vector correctly initializes it to the right size
    // in the case the enum for PORTS_PER_MCS changes
    std::vector<uint8_t> l_cls_vect(PORTS_PER_MCS, uint8_t(i_cas_latency) );

    // set CAS latency attribute
    // casts vector into the type FAPI_ATTR_SET is expecting by deduction
    FAPI_TRY( FAPI_ATTR_SET(fapi2::ATTR_EFF_DRAM_CL,
                            i_target,
                            UINT8_VECTOR_TO_1D_ARRAY(l_cls_vect, PORTS_PER_MCS)) ,
              "Failed to set CAS latency attribute");

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief      Sets frequency attributes
/// @param[in]  i_target the controller target
/// @param[in]  i_dimm_freq final selected dimm freq in MT/s
/// @return     FAPI2_RC_SUCCESS iff ok
/// @note       P9 Nimbus will support DMI speeds 16GB and 9.6GB
///
inline fapi2::ReturnCode set_freq_attrs(const fapi2::Target<fapi2::TARGET_TYPE_MCS>& i_target,
                                        uint64_t i_dimm_freq)
{
    // TK - RIT, needsto be corrected
    uint64_t l_nest_freq = 2;
    const auto l_mcbist = mss::find_target<fapi2::TARGET_TYPE_MCBIST>(i_target);

    FAPI_INF("Setting freq attrs for %s", mss::c_str(i_target));

    // TK
    //Update for P9, what do we do w/setting nest freq? - AAM
    // how do we select nest freq if we even have to??
    FAPI_TRY( FAPI_ATTR_SET(fapi2::ATTR_MSS_NEST_CAPABLE_FREQUENCIES, l_mcbist, l_nest_freq),
              "Failed to set nest capable frequencies attribute" );

    FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_MSS_FREQ, l_mcbist, i_dimm_freq),
             "Failed to set mss freq attribute");

fapi_try_exit:
    return fapi2::current_err;
}

}// mss namespace


typedef fapi2::ReturnCode (*p9_mss_freq_FP_t) (const fapi2::Target<fapi2::TARGET_TYPE_MCS>&);

extern "C"
{

    ///
    /// @brief Calculate and save off DIMM frequencies
    /// @param[in] i_target the controller (e.g., MCS)
    /// @return FAPI2_RC_SUCCESS iff ok
    ///
    fapi2::ReturnCode p9_mss_freq( const fapi2::Target<fapi2::TARGET_TYPE_MCS>& i_target);

}

#endif
