@echo off
regsvr32 /s ttseng.dll
if exist .\enu.syn (
goto enu
)
else (
echo %language was not found. Skipping.
)
:checkeng
if exist .\eng.syn (
goto eng
)
else (
echo %language was not found. Skipping.
)
:checkesp
if exist .\esp.syn (
goto esp
)
else (
echo %language was not found. Skipping.
)
:checkesm
if exist .\esm.syn (
goto esm
)
else (
echo %language was not found. Skipping.
)
:checkfra
if exist .\fra.syn (
goto fra
)
else (
echo %language was not found. Skipping.
)
:checkfrc
if exist .\frc.syn (
goto frc
)
else (
echo %language was not found. Skipping.
)
:checkdeu
if exist .\deu.syn (
goto deu
)
else (
echo %language was not found. Skipping.
)
:checkita
if exist .\ita.syn (
goto ita
)
else (
echo %language was not found. Skipping.
)
:checkchs
if exist .\chs.syn (
goto chs
)
else (
echo %language was not found. Skipping.
)
:checkptb
if exist .\ptb.syn (
goto ptb
)
else (
echo %language was not found. Skipping.
)
:checkjpn
if exist .\jpn.syn (
goto jpn
)
else (
echo %language was not found. Skipping.
)
:checkfin
if exist .\fin.syn (
goto fin
)
else (
echo %language was not found. Skipping.
)
:checkkor
if exist .\kor.syn (
goto kor
)
else (
echo %language was not found. Skipping.
)
:enu
set language=enu
regsvr32 /s enu.syn
reg import enu.reg /reg:32
goto checkeng
:eng
set language=eng
regsvr32 /s eng.syn
reg import eng.reg /reg:32
goto checkesp
:esp
set language=esp
regsvr32 /s esp.syn
reg import esp.reg /reg:32
goto checkesm
:esm
set language=esm
regsvr32 /s esm.syn
reg import esm.reg /reg:32
goto checkfra
:fra
set language=fra
regsvr32 /s fra.syn
reg import fra.reg /reg:32
goto checkfrc
:frc
set language=frc
regsvr32 /s frc.syn
reg import frc.reg /reg:32
goto checkdeu
:deu
set language=deu
regsvr32 /s deu.syn
reg import deu.reg /reg:32
goto checkita
:ita
set language=ita
regsvr32 /s ita.syn
reg import ita.reg /reg:32
goto checkchs
:chs
set language=chs
regsvr32 /s chs.syn
regsvr32 /s chsrom.dll
reg import chs.reg /reg:32
goto checkptb
:ptb
set language=ptb
regsvr32 /s ptb.syn
reg import ptb.reg /reg:32
goto checkjpn
:jpn
set language=jpn
regsvr32 /s jpn.syn
regsvr32 /s jpnrom.dll
reg import jpn.reg /reg:32
goto checkfin
:fin
set language=fin
regsvr32 /s fin.syn
reg import fin.reg /reg:32
goto checkkor
:kor
set language=kor
regsvr32 /s kor.syn
regsvr32 /s korrom.dll
reg import kor.reg /reg:32
echo Installation finished. Press enter to continue.
pause > nul
