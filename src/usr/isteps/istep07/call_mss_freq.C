/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/istep07/call_mss_freq.C $                      */
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
/**
 *  @file call_mss_freq.C
 *  Contains the wrapper for istep 7.3
 */

/******************************************************************************/
// Includes
/******************************************************************************/
#include    <stdint.h>

#include    <trace/interface.H>
#include    <initservice/taskargs.H>
#include    <errl/errlentry.H>

#include    <isteps/hwpisteperror.H>
#include    <errl/errludtarget.H>

#include    <initservice/isteps_trace.H>

//  targeting support
#include    <targeting/common/commontargeting.H>
#include    <targeting/common/utilFilter.H>

#include    <config.h>
#include    <fapi2.H>
#include    <fapi2/plat_hwp_invoker.H>


// SBE
#include    <sbeif.H>

// HWP
#include    <p9_mss_freq.H>
#include    <p9_mss_freq_system.H>
// #include <p9_xip_customize.H> // RTC:138226
namespace   ISTEP_07
{

using   namespace   ISTEP;
using   namespace   ISTEP_ERROR;
using   namespace   ERRORLOG;
using   namespace   TARGETING;

//
//  Wrapper function to call mss_freq
//
void*    call_mss_freq( void *io_pArgs )
{
    IStepError l_StepError;
    errlHndl_t l_err = NULL;
    TRACDCOMP( ISTEPS_TRACE::g_trac_isteps_trace, "call_mss_freq entry" );

    TARGETING::TargetHandleList l_membufTargetList;
    getAllChiplets(l_membufTargetList, TYPE_MCS);

    for (const auto & l_membuf_target : l_membufTargetList)
    {
        TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                "p9_mss_freq HWP target HUID %.8x",
                TARGETING::get_huid(l_membuf_target));

        //  call the HWP with each target   ( if parallel, spin off a task )
        fapi2::Target <fapi2::TARGET_TYPE_MCS> l_fapi_membuf_target
            (l_membuf_target);

        FAPI_INVOKE_HWP(l_err, p9_mss_freq, l_fapi_membuf_target);

        //  process return code.
        if ( l_err )
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
            "ERROR 0x%.8X:  p9_mss_freq HWP on target HUID %.8x",
            l_err->reasonCode(), TARGETING::get_huid(l_membuf_target) );

            // capture the target data in the elog
            ErrlUserDetailsTarget(l_membuf_target).addToLog( l_err );

            // Create IStep error log and cross reference to error that occurred
            l_StepError.addErrorDetails( l_err );

            // Commit Error
            errlCommit( l_err, HWPF_COMP_ID );

        }
        else
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                     "SUCCESS :  mss_freq HWP");
        }
    } // End memBuf loop




    if(l_StepError.getErrorHandle() == NULL)
    {
        TARGETING::TargetHandleList l_mcbistTargetList;
        getAllChiplets(l_mcbistTargetList, TYPE_MCBIST);
        std::vector< fapi2::Target<fapi2::TARGET_TYPE_MCBIST> > l_fapi2_mcbistTargetList;


        for (const auto & l_mcbist_target : l_mcbistTargetList)
        {
            //  call the HWP with each target   ( if parallel, spin off a task )
            fapi2::Target <fapi2::TARGET_TYPE_MCBIST> l_fapi_mcbist_target(l_mcbist_target);
            l_fapi2_mcbistTargetList.push_back(l_fapi_mcbist_target);
        }


        FAPI_INVOKE_HWP(l_err, p9_mss_freq_system, l_fapi2_mcbistTargetList);

        //  process return code.
        if ( l_err )
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
            "ERROR 0x%.8X:  p9_mss_freq_system HWP while running on mcbist targets");

            // Create IStep error log and cross reference to error that occurred
            l_StepError.addErrorDetails( l_err );

            // Commit Error
            errlCommit( l_err, HWPF_COMP_ID );

        }
        else
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                    "SUCCESS :  mss_freq_system HWP");
        }
    }
    else
    {
        TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                   "WARNING skipping p9_mss_freq_system HWP due to error detected in p9_mss_freq HWP. An error should have been committed.");
    }

/*  TODO RTC: 157659 Trigger SBE update if nest frequency changed
    // Check to see if the nest frequency changed
    TARGETING::targetService().getTopLevelTarget( l_sys );
    l_newNest = l_sys->getAttr<TARGETING::ATTR_NEST_FREQ_MHZ>();
    l_originalNest = l_sys->getAttr<TARGETING::ATTR_PREV_NEST_FREQ_MHZ>();

    // Trigger sbe update if the nest frequency changed.

    if( l_newNest != l_originalNest )
    {
        l_err = SBE::updateProcessorSbeSeeproms();

        if( l_err )
        {
            TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                     "call_mss_freq.C - Error calling updateProcessorSbeSeeproms");
        }
    }
*/

    // TODO RTC:138226
    // 3c) FW examines current synchronous mode nest freq and will customize the
    // SBE and reboot if necessary on the master only
    // (slaves get data via mbox scratch registers)
    /* FAPI_INVOKE_HWP(l_err, p9_xip_customize,
            const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_proc_target,
            const fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>& i_system_target,
            void* io_image);
    */

    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace, "call_mss_freq exit" );


    return l_StepError.getErrorHandle();
}

};   // end namespace
