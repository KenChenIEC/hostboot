/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/memory/lib/eff_config/timing.C $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016                             */
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
#include <fapi2.H>
#include <mss.H>
#include <lib/utils/find.H>
#include <lib/eff_config/timing.H>

namespace mss
{

// Proposed DDR4 Full spec update(79-4A)
// Item No. 1716.78C
// pg.46
// Table 24 - tREFI and tRFC parameters
static const std::vector<std::pair<uint8_t, uint64_t> > TREFI_BASE =
{
    // { density in GBs, tREFI(base) in picoseconds }
    {2, 7800000},
    {4, 7800000},
    {8, 7800000},
    // 16Gb - TBD
};

// Proposed DDR4 Full spec update(79-4A)
// Item No. 1716.78C
// pg.46
// Table 24 - tREFI and tRFC parameters
static const std::vector<std::pair<uint8_t, uint64_t> > TRFC1_MIN =
{
    // { density in GBs, tRFC1(min) in picoseconds }
    {2, 160000},
    {4, 260000},
    {8, 350000},
    // 16Gb - TBD
};

// Proposed DDR4 Full spec update(79-4A)
// Item No. 1716.78C
// pg.46
// Table 24 - tREFI and tRFC parameters
static const std::vector<std::pair<uint8_t, uint64_t> > TRFC2_MIN =
{
    // { density in GBs, tRFC2(min) in picoseconds }
    {2, 110000},
    {4, 160000},
    {8, 260000},
    // 16Gb - TBD
};

// Proposed DDR4 Full spec update(79-4A)
// Item No. 1716.78C
// pg.46
// Table 24 - tREFI and tRFC parameters
static const std::vector<std::pair<uint8_t, uint64_t> > TRFC4_MIN =
{
    // { density in GBs, tRFC4(min) in picoseconds }
    {2, 90000},
    {4, 110000},
    {8, 160000},
    // 16Gb - TBD
};

// Proposed DDR4 3DS Addendum
// Item No. 1727.58A
// pg. 69 - 71
// Table 42 - Refresh parameters by logical rank density
static const std::vector<std::pair<uint8_t, uint64_t> > TRFC_DLR1 =
{
    // { density in GBs, tRFC4(min) in picoseconds }
    {4, 90000},
    {8, 120000},
    // 16Gb - TBD
};

// Proposed DDR4 3DS Addendum
// Item No. 1727.58A
// pg. 69 - 71
// Table 42 - Refresh parameters by logical rank density
static const std::vector<std::pair<uint8_t, uint64_t> > TRFC_DLR2 =
{
    // { density in GBs, tRFC4(min) in picoseconds }
    {4, 55000},
    {8, 90000}
    // 16Gb - TBD
};

// Proposed DDR4 3DS Addendum
// Item No. 1727.58A
// pg. 69 - 71
// Table 42 - Refresh parameters by logical rank density
static const std::vector<std::pair<uint8_t, uint64_t> > TRFC_DLR4 =
{
    // { density in GBs, tRFC4(min) in picoseconds }
    {4, 40000},
    {8, 55000}
    // 16Gb - TBD
};

/// @brief Calculates refresh interval time 1 (tREFI 1)
/// @param[in] i_target FAPI2 target
/// @param[out] o_value timing val in ps
/// @return fapi2::ReturnCode
///
fapi2::ReturnCode calc_trefi1(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                              uint64_t& o_value)
{
    uint8_t  l_quotient = 0;
    uint8_t l_remainder = 0;
    uint64_t l_output = 0;
    uint8_t l_temp_refresh_range = 0;
    uint8_t l_dram_density = 0;
    bool l_found_value = true;

    FAPI_TRY( mss::eff_dram_density(i_target, l_dram_density) );
    FAPI_TRY( mss::mrw_temp_refresh_range(l_temp_refresh_range) );

    l_found_value = mss::find_value_from_key(TREFI_BASE, l_dram_density, l_output);

    switch(l_temp_refresh_range)
    {
        case fapi2::ENUM_ATTR_MRW_TEMP_REFRESH_RANGE_NORMAL:
            FAPI_TRY( check::fail_for_invalid_map(i_target,
                                                  l_found_value,
                                                  l_dram_density,
                                                  l_temp_refresh_range,
                                                  "Could not find sdram density given "
                                                  "normal temp_refresh_range.") );
            break;

        case fapi2::ENUM_ATTR_MRW_TEMP_REFRESH_RANGE_EXTEND:
            FAPI_TRY( check::fail_for_invalid_map(i_target,
                                                  l_found_value,
                                                  l_dram_density,
                                                  l_temp_refresh_range,
                                                  "Could not find sdram density given "
                                                  "extended temp_refresh_range.") );
            l_quotient = l_output / 2;
            l_remainder = l_output % 2;
            o_value = l_quotient + (l_remainder == 0 ? 0 : 1);
            break;

        default:
            // Temperature Refresh Range will be a platform attribute set by the MRW,
            // which they "shouldn't" mess up as long as use "attribute" enums.
            // if openpower messes this up we can at least catch it
            FAPI_ASSERT(false,
                        fapi2::MSS_INVALID_TEMP_REF_RANGE().
                        set_TEMP_REF_RANGE(l_temp_refresh_range),
                        "%s Incorrect Temperature Ref. Range received: %d ",
                        mss::c_str(i_target),
                        l_temp_refresh_range);

            break;
    }

fapi_try_exit:
    return fapi2::current_err;
}

/// @brief Calculates refresh interval time 2 (tREFI 2)
/// @param[in] i_target FAPI2 target
/// @param[out] o_value timing val in ps
/// @return fapi2::ReturnCode
///
fapi2::ReturnCode calc_trefi2(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                              uint64_t& o_value)
{
    uint8_t l_quotient = 0;
    uint8_t l_remainder = 0;
    uint64_t l_output = 0;
    uint8_t l_temp_refresh_range = 0;
    uint8_t l_dram_density = 0;
    bool l_found_value = true;

    FAPI_TRY( mss::mrw_temp_refresh_range(l_temp_refresh_range) );
    FAPI_TRY( mss::eff_dram_density(i_target, l_dram_density) );

    l_found_value = mss::find_value_from_key(TREFI_BASE, l_dram_density, l_output);

    switch(l_temp_refresh_range)
    {
        case fapi2::ENUM_ATTR_MRW_TEMP_REFRESH_RANGE_NORMAL:
            FAPI_TRY( check::fail_for_invalid_map(i_target,
                                                  l_found_value,
                                                  l_dram_density,
                                                  l_temp_refresh_range,
                                                  "Could not find sdram density given "
                                                  "normal temp_refresh_range.") );

            l_remainder = l_output % 2;
            o_value = l_quotient + (l_remainder == 0 ? 0 : 1);
            break;

        case fapi2::ENUM_ATTR_MRW_TEMP_REFRESH_RANGE_EXTEND:
            FAPI_TRY( check::fail_for_invalid_map(i_target,
                                                  l_found_value,
                                                  l_dram_density,
                                                  l_temp_refresh_range,
                                                  "Could not find sdram density given "
                                                  "extended temp_refresh_range.") );
            l_quotient = l_output / 4;
            l_remainder = l_output % 4;
            o_value = l_quotient + (l_remainder == 0 ? 0 : 1);
            break;

        default:
            // Temperature Refresh Range will be a platform attribute set by the MRW,
            // which they "shouldn't" mess up as long as use "attribute" enums.
            // if openpower messes this up we can at least catch it
            FAPI_ASSERT(false,
                        fapi2::MSS_INVALID_TEMP_REF_RANGE().
                        set_TEMP_REF_RANGE(l_temp_refresh_range),
                        "%s Incorrect Temperature Ref. Range received: %d ",
                        mss::c_str(i_target),
                        l_temp_refresh_range);

            break;
    }

fapi_try_exit:
    return fapi2::current_err;
}

/// @brief Calculates refresh interval time 4 (tREFI 4)
/// @param[in] i_target FAPI2 target
/// @param[out] o_value timing val in ps
/// @return fapi2::ReturnCode
///
fapi2::ReturnCode calc_trefi4( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                               uint64_t& o_value)
{
    uint8_t l_quotient = 0;
    uint8_t l_remainder = 0;
    uint64_t l_output = 0;
    uint8_t l_temp_refresh_range = 0;
    uint8_t l_dram_density = 0;
    bool l_found_value = true;

    FAPI_TRY( mss::mrw_temp_refresh_range(l_temp_refresh_range) );
    FAPI_TRY( mss::eff_dram_density(i_target, l_dram_density) );

    l_found_value = mss::find_value_from_key(TREFI_BASE, l_dram_density, l_output);

    switch(l_temp_refresh_range)
    {
        case fapi2::ENUM_ATTR_MRW_TEMP_REFRESH_RANGE_NORMAL:
            FAPI_TRY( check::fail_for_invalid_map(i_target,
                                                  l_found_value,
                                                  l_dram_density,
                                                  l_temp_refresh_range,
                                                  "Could not find sdram density given "
                                                  "normal temp_refresh_range.") );
            l_quotient = l_output / 4;
            l_remainder = l_output % 4;
            o_value = l_quotient + (l_remainder == 0 ? 0 : 1);
            break;

        case fapi2::ENUM_ATTR_MRW_TEMP_REFRESH_RANGE_EXTEND:
            FAPI_TRY( check::fail_for_invalid_map(i_target,
                                                  l_found_value,
                                                  l_dram_density,
                                                  l_temp_refresh_range,
                                                  "Could not find sdram density given "
                                                  "extended temp_refresh_range.") );
            l_quotient = l_output / 8;
            l_remainder = l_output % 8;
            o_value = l_quotient + (l_remainder == 0 ? 0 : 1);
            break;

        default:
            // Temperature Refresh Range will be a platform attribute set by the MRW,
            // which they "shouldn't" mess up as long as use "attribute" enums.
            // if openpower messes this up we can at least catch it
            FAPI_ASSERT(false,
                        fapi2::MSS_INVALID_TEMP_REF_RANGE().
                        set_TEMP_REF_RANGE(l_temp_refresh_range),
                        "%s Incorrect Temperature Ref. Range received: %d ",
                        mss::c_str(i_target),
                        l_temp_refresh_range);

            break;
    }

fapi_try_exit:
    return fapi2::current_err;
}

}// mss
