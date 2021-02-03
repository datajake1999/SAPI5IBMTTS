/******************************************************************************
* TtsEngObj.h *
*-------------*
*  This is the header file for the sample CTTSEngObj class definition.
*------------------------------------------------------------------------------
*  Copyright (c) Microsoft Corporation. All rights reserved.
*
******************************************************************************/
#ifndef TtsEngObj_h
#define TtsEngObj_h

//--- Additional includes
#ifndef __TtsEng_h__
#include "ttseng.h"
#endif
#include <stdio.h>
#include "eci.h"

#ifndef SPDDKHLP_h
#include <spddkhlp.h>
#endif

#ifndef SPCollec_h
#include <spcollec.h>
#endif

#include "resource.h"

//=== Constants ====================================================

//=== Class, Enum, Struct and Union Declarations ===================

//=== Enumerated Set Definitions ===================================

//=== Function Type Definitions ====================================

//=== Class, Struct and Union Definitions ==========================

/*** CTTSEngObj COM object ********************************
*/
class ATL_NO_VTABLE CTTSEngObj : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CTTSEngObj, &CLSID_SampleTTSEngine>,
	public ISpTTSEngine,
    public ISpObjectWithToken
{
  /*=== ATL Setup ===*/
  public:
    DECLARE_REGISTRY_RESOURCEID(IDR_SAMPLETTSENGINE)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CTTSEngObj)
	    COM_INTERFACE_ENTRY(ISpTTSEngine)
	    COM_INTERFACE_ENTRY(ISpObjectWithToken)
    END_COM_MAP()

  /*=== Methods =======*/
  public:
    /*--- Constructors/Destructors ---*/
    HRESULT FinalConstruct();
    void FinalRelease();

  /*=== Interfaces ====*/
  public:
    //--- ISpObjectWithToken ----------------------------------
    STDMETHODIMP SetObjectToken( ISpObjectToken * pToken );
    STDMETHODIMP GetObjectToken( ISpObjectToken ** ppToken )
        { return SpGenericGetObjectToken( ppToken, m_cpToken ); }


    //--- ISpTTSEngine --------------------------------------------
    STDMETHOD(Speak)( DWORD dwSpeakFlags,
                      REFGUID rguidFormatId, const WAVEFORMATEX * pWaveFormatEx,
                      const SPVTEXTFRAG* pTextFragList, ISpTTSEngineSite* pOutputSite );
    STDMETHOD(GetOutputFormat)( const GUID * pTargetFormatId, const WAVEFORMATEX * pTargetWaveFormatEx,
                                GUID * pDesiredFormatId, WAVEFORMATEX ** ppCoMemDesiredWaveFormatEx );


  /*=== Member Data ===*/
  private:
    CComPtr<ISpObjectToken> m_cpToken;

};

#endif //--- This must be the last line in the file
