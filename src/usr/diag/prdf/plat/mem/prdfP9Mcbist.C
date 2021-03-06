/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/plat/mem/prdfP9Mcbist.C $                   */
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

/** @file  prdfP9Mcbist.C
 *  @brief Contains plugin code for MCBIST on Hostboot (IPL and runtime).
 */

// Framework includes
#include <iipServiceDataCollector.h>
#include <prdfExtensibleChip.H>
#include <prdfPluginDef.H>
#include <prdfPluginMap.H>

// Platform includes
#include <prdfPlatServices.H>
#include <prdfP9McbistDataBundle.H>

namespace PRDF
{

using namespace PlatServices;

namespace p9_mcbist
{

//##############################################################################
//
//                             Special plugins
//
//##############################################################################

/**
 * @brief  Plugin that initializes the data bundle.
 * @param  i_mcbChip An MCBIST chip.
 * @return SUCCESS
 */
int32_t Initialize( ExtensibleChip * i_mcbChip )
{
    i_mcbChip->getDataBundle() = new McbistDataBundle( i_mcbChip );
    return SUCCESS;
}
PRDF_PLUGIN_DEFINE( p9_mcbist, Initialize );

/**
 * @brief  Plugin function called after analysis is complete but before PRD
 *         exits.
 * @param  i_mcbChip An MCBIST chip.
 * @param  io_sc     The step code data struct.
 * @note   This is especially useful for any analysis that still needs to be
 *         done after the framework clears the FIR bits that were at attention.
 * @return SUCCESS.
 */
int32_t PostAnalysis( ExtensibleChip * i_mcbChip,
                      STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[p9_mcbist::PostAnalysis] "

    #ifndef __HOSTBOOT_RUNTIME

    // Send command complete to MDIA.
    // This must be done in post analysis after attentions have been cleared.

    McbistDataBundle * mcbdb = getMcbistDataBundle( i_mcbChip );

    if ( mcbdb->iv_sendCmdCompleteMsg )
    {
        mcbdb->iv_sendCmdCompleteMsg = false;

        int32_t rc = mdiaSendEventMsg( i_mcbChip->getTrgt(),
                                       MDIA::COMMAND_COMPLETE );
        if ( SUCCESS != rc )
        {
            PRDF_ERR( PRDF_FUNC "mdiaSendEventMsg(COMMAND_COMPLETE) failed" );
        }
    }

    #endif // not __HOSTBOOT_RUNTIME

    return SUCCESS; // Always return SUCCESS for this plugin.

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( p9_mcbist, PostAnalysis );

//##############################################################################
//
//                                 MCBISTFIR
//
//##############################################################################

/**
 * @brief  MCBIST[10] - MCBIST Command Complete.
 * @param  i_mcbChip An MCBIST chip.
 * @param  io_sc     The step code data struct.
 * @return SUCCESS
 */
int32_t McbistCmdComplete( ExtensibleChip * i_mcbChip,
                           STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[p9_mcbist::McbistCmdComplete] "

    // Tell the TD controller there was a command complete attention.
    getMcbistDataBundle(i_mcbChip)->getTdCtlr()->handleCmdComplete( io_sc );

    return SUCCESS; // Always return SUCCESS for this plugin.

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( p9_mcbist, McbistCmdComplete );

} // end namespace p9_mcbist

} // end namespace PRDF

