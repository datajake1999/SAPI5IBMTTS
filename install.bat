@echo off
regsvr32 /s ttseng.dll
if exist %cd%\enu.syn ( goto enu ) else ( goto error )
if exist %cd%\eng.syn ( goto eng ) else ( goto error )
if exist %cd%\esp.syn ( goto esp ) else ( goto error )
if exist %cd%\esm.syn ( goto esm ) else ( goto error )
if exist %cd%\fra.syn ( goto fra ) else ( goto error )
if exist %cd%\frc.syn ( goto frc ) else ( goto error )
if exist %cd%\deu.syn ( goto deu ) else ( goto error )
if exist %cd%\ita.syn ( goto ita ) else ( goto error )
if exist %cd%\chs.syn ( goto chs ) else ( goto error
if exist %cd%\ptb.syn ( goto ptb ) else ( goto error )
if exist %cd%\jpn.syn ( goto jpn ) else ( goto error )
if exist %cd%\fin.syn ( goto fin ) else ( goto error )
if exist %cd%\kor.syn ( goto kor ) else ( goto error )
:enu
set language=enu
regsvr32 /s enu.syn
reg import enu.reg /reg:32
:eng
set language=eng
regsvr32 /s eng.syn
reg import eng.reg /reg:32
:esp
set language=esp
regsvr32 /s esp.syn
reg import esp.reg /reg:32
:esm
set language=esm
regsvr32 /s esm.syn
reg import esm.reg /reg:32
:fra
set language=fra
regsvr32 /s fra.syn
reg import fra.reg /reg:32
:frc
set language=frc
regsvr32 /s frc.syn
reg import frc.reg /reg:32
:deu
set language=deu
regsvr32 /s deu.syn
reg import deu.reg /reg:32
:ita
set language=ita
regsvr32 /s ita.syn
reg import ita.reg /reg:32
:chs
set language=chs
regsvr32 /s chs.syn
regsvr32 /s chsrom.dll
reg import chs.reg /reg:32
:ptb
set language=ptb
regsvr32 /s ptb.syn
reg import ptb.reg /reg:32
:jpn
set language=jpn
regsvr32 /s jpn.syn
regsvr32 /s jpnrom.dll
reg import jpn.reg /reg:32
:fin
set language=fin
regsvr32 /s fin.syn
reg import fin.reg /reg:32
:kor
set language=kor
regsvr32 /s kor.syn
regsvr32 /s korrom.dll
reg import kor.reg /reg:32
goto end
:error
echo %language% was not found. Skipping.
:end
echo Installation finished. Press enter to continue.
pause > nul
exit
