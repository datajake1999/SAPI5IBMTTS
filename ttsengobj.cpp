/*******************************************************************************
* TtsEngObj.cpp *
*---------------*
*   Description:
*       This module is the main implementation file for the CTTSEngObj class.
*-------------------------------------------------------------------------------
*  Creation Date: 03/24/99
*  Copyright (c) Microsoft Corporation. All rights reserved.
*  All Rights Reserved
*
*******************************************************************************/

//--- Additional includes
#include "stdafx.h"
#include "TtsEngObj.h"

//--- Local

//convert SAPI rate to ECI rate
static int SAPI2ECIRate(int rate)
{
int DefaultRate = 60;
return DefaultRate + (rate * 10);
}

// Global pointer to OutputSite
static ISpTTSEngineSite *gpOutputSite;

//Handle to ECI
static ECIHand engine;

//check if we are speaking or not
static bool speaking;

//Input buffer
static char *text2speak;

//Output buffer
static short buffer[4096];

// ECI callback
static ECICallbackReturn callback(ECIHand hEngine, enum ECIMessage Msg, long lParam, void *pData)
{
if (!speaking || (gpOutputSite->GetActions() & SPVES_ABORT))
{
return eciDataAbort;
}
if (Msg == eciWaveformBuffer && lParam > 0)
{
gpOutputSite->Write(buffer, lParam*2, NULL);
}
return eciDataProcessed;
}

// ECI synthesis loop
static void SynthLoop()
{
int SpeakState = eciSpeaking(engine);
while (speaking == true)
{
if (eciSpeaking(engine) != SpeakState) speaking = false;
}
}

/*****************************************************************************
* CTTSEngObj::FinalConstruct *
*----------------------------*
*   Description:
*       Constructor
*****************************************************************************/
HRESULT CTTSEngObj::FinalConstruct()
{
    SPDBG_FUNC( "CTTSEngObj::FinalConstruct" );
    HRESULT hr = S_OK;

    //Initialize ECI
    engine = eciNew();
    eciRegisterCallback(engine, callback, NULL);
    eciSetOutputBuffer(engine, 4096, buffer);
eciSetParam(engine, eciInputType, 1);
speaking = false;

    return hr;
} /* CTTSEngObj::FinalConstruct */

/*****************************************************************************
* CTTSEngObj::FinalRelease *
*--------------------------*
*   Description:
*       destructor
*****************************************************************************/
void CTTSEngObj::FinalRelease()
{
    SPDBG_FUNC( "CTTSEngObj::FinalRelease" );

    //Shutdown ECI
    engine = eciDelete(engine);
    engine = NULL;

} /* CTTSEngObj::FinalRelease */

//
//=== ISpObjectWithToken Implementation ======================================
//

/*****************************************************************************
* CTTSEngObj::SetObjectToken *
*----------------------------*
*   Description:
*       This function performs the majority of the initialization of the voice.
*   Once the object token has been provided, the filenames are read from the
*   token key and the files are mapped.
*****************************************************************************/
STDMETHODIMP CTTSEngObj::SetObjectToken(ISpObjectToken * pToken)
{
    SPDBG_FUNC( "CTTSEngObj::SetObjectToken" );
    HRESULT hr = SpGenericSetObjectToken(pToken, m_cpToken);
    return hr;
} /* CTTSEngObj::SetObjectToken */

//
//=== ISpTTSEngine Implementation ============================================
//

/*****************************************************************************
* CTTSEngObj::Speak *
*-------------------*
*   Description:
*       This is the primary method that SAPI calls to render text.
*-----------------------------------------------------------------------------
*   Input Parameters
*
*   pUser
*       Pointer to the current user profile object. This object contains
*       information like what languages are being used and this object
*       also gives access to resources like the SAPI master lexicon object.
*
*   dwSpeakFlags
*       This is a set of flags used to control the behavior of the
*       SAPI voice object and the associated engine.
*
*   VoiceFmtIndex
*       Zero based index specifying the output format that should
*       be used during rendering.
*
*   pTextFragList
*       A linked list of text fragments to be rendered. There is
*       one fragement per XML state change. If the input text does
*       not contain any XML markup, there will only be a single fragment.
*
*   pOutputSite
*       The interface back to SAPI where all output audio samples and events are written.
*
*   Return Values
*       S_OK - This should be returned after successful rendering or if
*              rendering was interrupted because *pfContinue changed to FALSE.
*       E_INVALIDARG 
*       E_OUTOFMEMORY
*
*****************************************************************************/
STDMETHODIMP CTTSEngObj::Speak( DWORD dwSpeakFlags,
                                REFGUID rguidFormatId,
                                const WAVEFORMATEX * pWaveFormatEx,
                                const SPVTEXTFRAG* pTextFragList,
                                ISpTTSEngineSite* pOutputSite )
{
    SPDBG_FUNC( "CTTSEngObj::Speak" );
    HRESULT hr = S_OK;

    //--- Check args
    if( SP_IS_BAD_INTERFACE_PTR( pOutputSite ) ||
        SP_IS_BAD_READ_PTR( pTextFragList )  )
    {
        hr = E_INVALIDARG;
    }
    else
    {
gpOutputSite = pOutputSite;
//set ECI voice parameters
signed long rate;
unsigned short volume;
pOutputSite->GetRate(&rate);
pOutputSite->GetVolume(&volume);
eciSetVoiceParam(engine, 0, eciSpeed, SAPI2ECIRate(rate));
eciSetVoiceParam(engine, 0, eciVolume, volume);

        while(pTextFragList != NULL)
        {
            if( pOutputSite->GetActions() & SPVES_ABORT )
            {
                eciStop(engine);
                return hr;
            }
            //--- Do skip?
            if( pOutputSite->GetActions() & SPVES_SKIP )
            {
                long lSkipCnt;
                SPVSKIPTYPE eType;
                hr = pOutputSite->GetSkipInfo( &eType, &lSkipCnt );
                if( SUCCEEDED( hr ) )
                {
                    //--- Notify SAPI how many items we skipped. We're returning zero
                    //    because this feature isn't implemented.
                    hr = pOutputSite->CompleteSkip( 0 );
                }
            }
            switch(pTextFragList->State.eAction)
            {

                case SPVA_Speak:
                {
                    //Convert Unicode text to ANSI for ECI
                    int strsize = WideCharToMultiByte(CP_ACP, 0, pTextFragList->pTextStart, pTextFragList->ulTextLen, NULL, 0, NULL, NULL);
                    if (text2speak) free(text2speak);
                    text2speak = (char *)malloc(strsize+1);
                    text2speak[strsize] = 0;
                    WideCharToMultiByte(CP_ACP, 0, pTextFragList->pTextStart, pTextFragList->ulTextLen, text2speak, strsize, NULL, NULL);
                    if (text2speak) eciAddText(engine, text2speak);
                    break;
                }

                case SPVA_Silence:
                {
                    //Insert a pause
                    if (text2speak) free(text2speak);
                    text2speak = (char *)malloc(32);
                    sprintf(text2speak, "`p%d", pTextFragList->State.SilenceMSecs);
                    if (text2speak) eciAddText(engine, text2speak);
                    break;
                }

          //--- Fire a bookmark event ---------------------------------
          case SPVA_Bookmark:
          {
            //--- The bookmark is NOT a null terminated string in the Item, but we need
            //--- to convert it to one.  Allocate enough space for the string.
            WCHAR * pszBookmark = (WCHAR *)_alloca((pTextFragList->ulTextLen + 1) * sizeof(WCHAR));
            memcpy(pszBookmark, pTextFragList->pTextStart, pTextFragList->ulTextLen * sizeof(WCHAR));
            pszBookmark[pTextFragList->ulTextLen] = 0;
            //--- Queue the event
            SPEVENT Event;
            Event.eEventId             = SPEI_TTS_BOOKMARK;
            Event.elParamType          = SPET_LPARAM_IS_STRING;
            Event.ullAudioStreamOffset = 0;
            Event.lParam               = (LPARAM)pszBookmark;
            Event.wParam               = _wtol(pszBookmark);
            hr = pOutputSite->AddEvents( &Event, 1 );
            break;
          }

            }
            pTextFragList = pTextFragList->pNext;
        }

//Synthesize text
        if (eciSynthesize(engine)) speaking = true;
//wait for synthesis to complete
        SynthLoop();

        //--- S_FALSE just says that we hit the end, return okay
        if( hr == S_FALSE )
        {
            hr = S_OK;
        }
    }

    return hr;
} /* CTTSEngObj::Speak */

/*****************************************************************************
* CTTSEngObj::GetVoiceFormat *
*----------------------------*
*   Description:
*       This method returns the output data format associated with the
*   specified format Index. Formats are in order of quality with the best
*   starting at 0.
*****************************************************************************/
STDMETHODIMP CTTSEngObj::GetOutputFormat( const GUID * pTargetFormatId, const WAVEFORMATEX * pTargetWaveFormatEx,
                                          GUID * pDesiredFormatId, WAVEFORMATEX ** ppCoMemDesiredWaveFormatEx )
{
    SPDBG_FUNC( "CTTSEngObj::GetVoiceFormat" );
    HRESULT hr = S_OK;

    hr = SpConvertStreamFormatEnum(SPSF_11kHz16BitMono, pDesiredFormatId, ppCoMemDesiredWaveFormatEx);

    return hr;
} /* CTTSEngObj::GetVoiceFormat */
