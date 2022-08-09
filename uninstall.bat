@echo off
echo Uninstalling
regsvr32 /u /s ttseng.dll
if exist .\enu.syn (
regsvr32 /u /s enu.syn
reg import enu-uninstall.reg /reg:32 2> nul
)
if exist .\eng.syn (
regsvr32 /u /s eng.syn
reg import eng-uninstall.reg /reg:32 2> nul
)
if exist .\esp.syn (
regsvr32 /u /s esp.syn
reg import esp-uninstall.reg /reg:32 2> nul
)
if exist .\esm.syn (
regsvr32 /u /s esm.syn
reg import esm-uninstall.reg /reg:32 2> nul
)
if exist .\fra.syn (
regsvr32 /u /s fra.syn
reg import fra-uninstall.reg /reg:32 2> nul
)
if exist .\frc.syn (
regsvr32 /u /s frc.syn
reg import frc-uninstall.reg /reg:32 2> nul
)
if exist .\deu.syn (
regsvr32 /u /s deu.syn
reg import deu-uninstall.reg /reg:32 2> nul
)
if exist .\ita.syn (
regsvr32 /u /s ita.syn
reg import ita-uninstall.reg /reg:32 2> nul
)
if exist .\chs.syn (
regsvr32 /u /s chs.syn
regsvr32 /u /s chsrom.dll
reg import chs-uninstall.reg /reg:32 2> nul
)
if exist .\ptb.syn (
regsvr32 /u /s ptb.syn
reg import ptb-uninstall.reg /reg:32 2> nul
)
if exist .\jpn.syn (
regsvr32 /u /s jpn.syn
regsvr32 /u /s jpnrom.dll
reg import jpn-uninstall.reg /reg:32 2> nul
)
if exist .\fin.syn (
regsvr32 /u /s fin.syn
reg import fin-uninstall.reg /reg:32 2> nul
)
if exist .\kor.syn (
regsvr32 /u /s kor.syn
regsvr32 /u /s korrom.dll
reg import kor-uninstall.reg /reg:32 2> nul
)
echo Uninstallation finished. Press enter to continue.
pause > nul
