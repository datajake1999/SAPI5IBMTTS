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
int CTTSEngObj::SAPI2ECIRate(int rate)
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
int CTTSEngObj::SAPI2ECIPitch(int pitch, int BassPitch)
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

//convert SAPI range to ECI range
int CTTSEngObj::SAPI2ECIRange(int range, int BassRange)
{
int NewRange = BassRange + (range * 4);
if (NewRange > 100)
	{
NewRange = 100;
}
else if (NewRange < 0)
	{
NewRange = 0;
}
return NewRange;
}

// ECI callback
ECICallbackReturn CTTSEngObj::callback(ECIHand hEngine, enum ECIMessage Msg, long lParam, void *pData)
{
CTTSEngObj *SAPI = (CTTSEngObj*)pData;
if (SAPI->m_OutputSite == NULL)
{
return eciDataAbort;
}
if (SAPI->m_OutputSite->GetActions() & SPVES_ABORT)
{
return eciDataAbort;
}
if (Msg == eciWaveformBuffer && lParam > 0)
{
SAPI->m_OutputSite->Write(SAPI->buffer, lParam*2, NULL);
SAPI->m_AudioOffset += lParam*2;
}
if (Msg == eciIndexReply && lParam < 0x7fffffff && SAPI->m_pCurrFrag != NULL)
{
                CSpEvent Event;
                Event.eEventId             = SPEI_WORD_BOUNDARY;
                Event.elParamType          = SPET_LPARAM_IS_UNDEFINED;
                Event.ullAudioStreamOffset = SAPI->m_AudioOffset;
                Event.lParam               = SAPI->m_pCurrFrag->ulTextSrcOffset;
                Event.wParam               = SAPI->m_pCurrFrag->ulTextLen;
                SAPI->m_OutputSite->AddEvents( &Event, 1 );
                SAPI->m_pCurrFrag = SAPI->m_pCurrFrag->pNext;
}
if (Msg == eciIndexReply && lParam == 0x7fffffff)
{
                CSpEvent Event;
                Event.eEventId             = SPEI_SENTENCE_BOUNDARY;
                Event.elParamType          = SPET_LPARAM_IS_UNDEFINED;
                Event.ullAudioStreamOffset = 0;
                Event.lParam               = 0;
                Event.wParam               = SAPI->m_TotalLen;
                SAPI->m_OutputSite->AddEvents( &Event, 1 );
}
return eciDataProcessed;
}

// ECI synthesis loop
void CTTSEngObj::SynthLoop()
{
while (eciSpeaking(engine) & 1)
{
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
    HRESULT hr = S_OK;

//Set pointers to NULL
    m_OutputSite = NULL;
    engine = NULL;
    text2speak = NULL;
    memset(buffer, 0, sizeof(buffer));

    //Initialize ECI
    engine = eciNew();
    if (engine == NULL)
    {
        return hr;
    }
    eciRegisterCallback(engine, callback, this);
    eciSetOutputBuffer(engine, 4096, buffer);
    eciSetParam(engine, eciSynthMode, 1);
    eciSetParam(engine, eciInputType, 1);

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

    if (engine == NULL)
    {
        return;
    }

    //Shutdown ECI
    eciDelete(engine);

//Set pointers to NULL
    m_OutputSite = NULL;
    engine = NULL;
    text2speak = NULL;
    memset(buffer, 0, sizeof(buffer));

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
    HRESULT hr = SpGenericSetObjectToken(pToken, m_cpToken);

    if( SUCCEEDED( hr ) )
    {
        if (engine == NULL)
        {
            return hr;
        }
        //Set default settings
        m_lang = 0;
        m_voice = 1;
        m_samplerate = 0;

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
    HRESULT hr = S_OK;

    //--- Check args
    if( SP_IS_BAD_INTERFACE_PTR( pOutputSite ) ||
        SP_IS_BAD_READ_PTR( pTextFragList )  )
    {
        hr = E_INVALIDARG;
    }
    else
    {
        if (engine == NULL)
        {
            return hr;
        }
        m_OutputSite = pOutputSite;
        m_pCurrFrag   = pTextFragList;
        m_IndexNum = 1;
        m_TotalLen = 0;

        while(pTextFragList != NULL)
        {
            if( pOutputSite->GetActions() & SPVES_ABORT )
            {
                eciStop(engine);
                return hr;
            }

            //set ECI voice parameters
            signed long rate = 0;
            unsigned short volume = 100;
            pOutputSite->GetRate(&rate);
            pOutputSite->GetVolume(&volume);
            rate += pTextFragList->State.RateAdj;
            volume = (volume * (unsigned short)pTextFragList->State.Volume) / 100;
            signed long pitch = pTextFragList->State.PitchAdj.MiddleAdj;
            signed long range = pTextFragList->State.PitchAdj.RangeAdj;
            signed long NewECIRate = SAPI2ECIRate(rate);
            signed long DefaultECIPitch = eciGetVoiceParam(engine, m_voice, eciPitchBaseline);
            signed long NewECIPitch = SAPI2ECIPitch(pitch, DefaultECIPitch);
            signed long DefaultECIRange = eciGetVoiceParam(engine, m_voice, eciPitchFluctuation);
            signed long NewECIRange = SAPI2ECIRange(range, DefaultECIRange);
            char params[32];
            sprintf(params, "`vs%d `vb%d `vf%d `vv%d", NewECIRate, NewECIPitch, NewECIRange, volume);
            eciAddText(engine, params);

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
                    eciAddText(engine, "`ts2");
                    //Convert Unicode text to ANSI for ECI
                    int strsize = WideCharToMultiByte(CP_ACP, 0, pTextFragList->pTextStart, pTextFragList->ulTextLen, NULL, 0, NULL, NULL);
                    if (text2speak) free(text2speak);
                    text2speak = (char *)malloc(strsize+1);
                    text2speak[strsize] = 0;
                    WideCharToMultiByte(CP_ACP, 0, pTextFragList->pTextStart, pTextFragList->ulTextLen, text2speak, strsize, NULL, NULL);
                    if (text2speak) eciAddText(engine, text2speak);
                    //Insert an index
                    eciInsertIndex(engine, m_IndexNum);
                    //Increment index number and total length
                    m_IndexNum++;
                    m_TotalLen += pTextFragList->ulTextLen;
                    break;
                }
                case SPVA_Speak:
                {
                    eciAddText(engine, "`ts0");
                    //Convert Unicode text to ANSI for ECI
                    int strsize = WideCharToMultiByte(CP_ACP, 0, pTextFragList->pTextStart, pTextFragList->ulTextLen, NULL, 0, NULL, NULL);
                    if (text2speak) free(text2speak);
                    text2speak = (char *)malloc(strsize+1);
                    text2speak[strsize] = 0;
                    WideCharToMultiByte(CP_ACP, 0, pTextFragList->pTextStart, pTextFragList->ulTextLen, text2speak, strsize, NULL, NULL);
                    if (text2speak) eciAddText(engine, text2speak);
                    //Insert an index
                    eciInsertIndex(engine, m_IndexNum);
                    //Increment index number and total length
                    m_IndexNum++;
                    m_TotalLen += pTextFragList->ulTextLen;
                    break;
                }

                case SPVA_Silence:
                {
                    //Insert a pause
                    char silence[32];
                    sprintf(silence, "`p%d", pTextFragList->State.SilenceMSecs);
                    eciAddText(engine, silence);
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

        //Mark the end of the text
        eciInsertIndex(engine, 0x7fffffff);

//Synthesize text
        eciSynthesize(engine);
//wait for synthesis to complete
        SynthLoop();
        m_OutputSite = NULL;

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
    HRESULT hr = S_OK;

    if (engine == NULL)
    {
        return hr;
    }
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
