/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/scom/runtime/handleSpecialWakeup.C $                  */
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
#include <stdint.h>
#include <errl/errlentry.H>
#include <errl/errludtarget.H>

#include <trace/interface.H>

#include <targeting/common/commontargeting.H>
#include <targeting/common/targetservice.H>
#include <targeting/common/utilFilter.H>
#include <targeting/common/util.H>


extern "C"
{

// Trace definition
extern trace_desc_t* g_trac_scom;

using namespace TARGETING;

errlHndl_t handleSpecialWakeup(TARGETING::Target* i_exTarget,
                               bool i_enableDisable)
{
    errlHndl_t l_errl = NULL;

    //@TODO RTC:132413 Add it back once we have fapi2 support
#if 0
    fapi::Target l_fapi_ex_target(TARGET_TYPE_EX_CHIPLET,
                            (const_cast<TARGETING::Target*>(i_exTarget)) );

    PROC_SPCWKUP_OPS l_spcwkupType;
    if(i_enableDisable)
    {
        l_spcwkupType = SPCWKUP_ENABLE;
    }
    else
    {
        l_spcwkupType = SPCWKUP_DISABLE;
    }

    //Using the FSP bit so it doesn't collide with Sapphire.
    //There are 3 independent registers used to trigger a special wakeup
    //(FSP,HOST,OCC). Since this is in HBRT, Opal already owns the HOST
    //register, so we're using the FSP bit here.
    FAPI_INVOKE_HWP(l_errl,
                    p8_cpu_special_wakeup,
                    l_fapi_ex_target,
                    l_spcwkupType,
                    FSP);

    if(l_errl)
    {
        TRACFCOMP( g_trac_scom,
                  "Disable p8_cpu_special_wakeup ERROR :"
                  " Returning errorlog, reason=0x%x",
                   l_errl->reasonCode() );

        // capture the target data in the elog
        ERRORLOG::ErrlUserDetailsTarget(i_exTarget).addToLog( l_errl );
    }
#endif

    return l_errl;
}
}
