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
int NewRate = DefaultRate + (rate * 10);
if (NewRate > 250)
	{
NewRate = 250;
}
else if (NewRate < 0)
	{
NewRate = 0;
}
return NewRate;
}

//convert SAPI pitch to ECI pitch
static int SAPI2ECIPitch(int pitch, int BassPitch)
{
int NewPitch = BassPitch + (pitch * 2);
if (NewPitch > 100)
	{
NewPitch = 100;
}
else if (NewPitch < 0)
	{
NewPitch = 0;
}
return NewPitch;
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
if (!speaking)
{
return eciDataNotProcessed;
}
if (gpOutputSite->GetActions() & SPVES_ABORT)
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

    if( SUCCEEDED( hr ) )
    {
        //Initialize ECI
        engine = eciNew();
        eciRegisterCallback(engine, callback, NULL);
        eciSetOutputBuffer(engine, 4096, buffer);
        eciSetParam(engine, eciInputType, 1);
        speaking = false;

        //Load settings from the token
        m_cpToken->GetDWORD( L"Language", &m_lang);
        m_cpToken->GetDWORD( L"Voice", &m_voice);
        m_cpToken->GetDWORD( L"SampleRate", &m_samplerate);

        //Set ECI language
        //Their is Probably a much more efficient way of doing this
        switch(m_lang)
        {
            default:
            eciSetParam(engine, eciLanguageDialect, NODEFINEDCODESET);
            break;
            case 1:
            eciSetParam(engine, eciLanguageDialect, eciGeneralAmericanEnglish);
            break;
            case 2:
            eciSetParam(engine, eciLanguageDialect, eciBritishEnglish);
            break;
            case 3:
            eciSetParam(engine, eciLanguageDialect, eciCastilianSpanish);
            break;
            case 4:
            eciSetParam(engine, eciLanguageDialect, eciMexicanSpanish);
            break;
            case 5:
            eciSetParam(engine, eciLanguageDialect, eciStandardFrench);
            break;
            case 6:
            eciSetParam(engine, eciLanguageDialect, eciCanadianFrench);
            break;
            case 7:
            eciSetParam(engine, eciLanguageDialect, eciStandardGerman);
            break;
            case 8:
            eciSetParam(engine, eciLanguageDialect, eciStandardItalian);
            break;
            case 9:
            eciSetParam(engine, eciLanguageDialect, eciMandarinChinese);
            break;
            case 10:
            eciSetParam(engine, eciLanguageDialect, eciMandarinChineseGB);
            break;
            case 11:
            eciSetParam(engine, eciLanguageDialect, eciMandarinChinesePinYin);
            break;
            case 12:
            eciSetParam(engine, eciLanguageDialect, eciMandarinChineseUCS);
            break;
            case 13:
            eciSetParam(engine, eciLanguageDialect, eciTaiwaneseMandarin);
            break;
            case 14:
            eciSetParam(engine, eciLanguageDialect, eciTaiwaneseMandarinBig5);
            break;
            case 15:
            eciSetParam(engine, eciLanguageDialect, eciTaiwaneseMandarinZhuYin);
            break;
            case 16:
            eciSetParam(engine, eciLanguageDialect, eciTaiwaneseMandarinPinYin);
            break;
            case 17:
            eciSetParam(engine, eciLanguageDialect, eciTaiwaneseMandarinUCS);
            break;
            case 18:
            eciSetParam(engine, eciLanguageDialect, eciBrazilianPortuguese);
            break;
            case 19:
            eciSetParam(engine, eciLanguageDialect, eciStandardJapanese);
            break;
            case 20:
            eciSetParam(engine, eciLanguageDialect, eciStandardJapaneseSJIS);
            break;
            case 21:
            eciSetParam(engine, eciLanguageDialect, eciStandardJapaneseUCS);
            break;
            case 22:
            eciSetParam(engine, eciLanguageDialect, eciStandardFinnish);
            break;
            case 23:
            eciSetParam(engine, eciLanguageDialect, eciStandardKorean);
            break;
        }

        //Check if we are in a valid range for m_voice
        if (m_voice > 8)
        {
            m_voice = 8;
        }
        else if (m_voice < 1)
        {
            m_voice = 1;
        }

        //Copy voice parameters to active voice
        eciCopyVoice(engine, m_voice, 0);

        //Set the sample rate
        if (m_samplerate == 1)
        {
            eciSetParam(engine, eciSampleRate, 0);
        }
        else if (m_samplerate == 2)
        {
            eciSetParam(engine, eciSampleRate, 2);
        }
        else
        {
            eciSetParam(engine, eciSampleRate, 1);
        }

    }

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
        while(pTextFragList != NULL)
        {
            if( pOutputSite->GetActions() & SPVES_ABORT )
            {
                return hr;
            }

            //set ECI voice parameters
            signed long rate;
            signed long pitch;
            unsigned short volume;
            pOutputSite->GetRate(&rate);
            pitch = pTextFragList->State.PitchAdj.MiddleAdj;
            pOutputSite->GetVolume(&volume);
            eciSetVoiceParam(engine, 0, eciSpeed, SAPI2ECIRate(rate));
            signed long DefaultECIPitch = eciGetVoiceParam(engine, m_voice, eciPitchBaseline);
            eciSetVoiceParam(engine, 0, eciPitchBaseline, SAPI2ECIPitch(pitch, DefaultECIPitch));
            eciSetVoiceParam(engine, 0, eciVolume, volume);

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

                case SPVA_SpellOut:
                {
                }
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

    unsigned long ECIRate = eciGetParam(engine, eciSampleRate);
    if (ECIRate == 0)
    {
    hr = SpConvertStreamFormatEnum(SPSF_8kHz16BitMono, pDesiredFormatId, ppCoMemDesiredWaveFormatEx);
    }
    else if (ECIRate == 2)
    {
    hr = SpConvertStreamFormatEnum(SPSF_22kHz16BitMono, pDesiredFormatId, ppCoMemDesiredWaveFormatEx);
    }
    else
    {
    hr = SpConvertStreamFormatEnum(SPSF_11kHz16BitMono, pDesiredFormatId, ppCoMemDesiredWaveFormatEx);
    }

    return hr;
} /* CTTSEngObj::GetVoiceFormat */
