@echo off
echo Installing
regsvr32 /s ttseng.dll
if exist .\enu.syn (
regsvr32 /s enu.syn
reg import enu-install.reg /reg:32 2> nul
)
if exist .\eng.syn (
regsvr32 /s eng.syn
reg import eng-install.reg /reg:32 2> nul
)
if exist .\esp.syn (
regsvr32 /s esp.syn
reg import esp-install.reg /reg:32 2> nul
)
if exist .\esm.syn (
regsvr32 /s esm.syn
reg import esm-install.reg /reg:32 2> nul
)
if exist .\fra.syn (
regsvr32 /s fra.syn
reg import fra-install.reg /reg:32 2> nul
)
if exist .\frc.syn (
regsvr32 /s frc.syn
reg import frc-install.reg /reg:32 2> nul
)
if exist .\deu.syn (
regsvr32 /s deu.syn
reg import deu-install.reg /reg:32 2> nul
)
if exist .\ita.syn (
regsvr32 /s ita.syn
reg import ita-install.reg /reg:32 2> nul
)
if exist .\chs.syn (
regsvr32 /s chs.syn
regsvr32 /s chsrom.dll
reg import chs-install.reg /reg:32 2> nul
)
if exist .\ptb.syn (
regsvr32 /s ptb.syn
reg import ptb-install.reg /reg:32 2> nul
)
if exist .\jpn.syn (
regsvr32 /s jpn.syn
regsvr32 /s jpnrom.dll
reg import jpn-install.reg /reg:32 2> nul
)
if exist .\fin.syn (
regsvr32 /s fin.syn
reg import fin-install.reg /reg:32 2> nul
)
if exist .\kor.syn (
regsvr32 /s kor.syn
regsvr32 /s korrom.dll
reg import kor-install.reg /reg:32 2> nul
)
echo Installation finished. Press enter to continue.
pause > nul
