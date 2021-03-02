# SAPI5 IBMTTS

## Introduction

SAPI5 is a standard that allows for text-to-speech and speech recognition capabilities on Windows. It is included out of the box on Windows XP and above but can manually be installed on Windows 2000, 98 and NT4.
IBM offered a text-to-speech solution known as ViaVoice which produces clear speech that has a low disk space and memory footprint, making it suitable for any machine.
A similar product, called ETI-Eloquence, also produces similar speech, and used the same interface. However, it was often not possible to use these voices with many programs because application developers had to access the speech system through its API, which were mainly only used by screen readers for the blind and visually impaired.
This means that, for example, if you wanted to use these voices outside of a screen reader, that you were quite limited in the applications that could use it. While ViaVoice and ETI-Eloquence both did have documented support for SAPI, it was only for version 4, which is no longer supported by Microsoft and many programs do not support it.
This is where this update is useful.

## What is it?

This is an attempt to allow ETI-Eloquence and IBM ViaVoice speech synthesizers to be usable with SAPI5, overcoming the issue of not being able to use these voices with many modern applications. Once installed, you will be able to use these voices with a wide variety of applications, including:
- Screen readers and magnifiers which include speech output
- Book reading applications, for reading text-based books
- Certain computer games which include speech
- Any program which uses the voice selected under "Speech" in Control Panel

## Installation

After building, you will be able to install. It is necessary to do this in two stages, as outlined below.

## Stage 1: Obtain IBM ViaVoice/ETI-Eloquence Files

The first step in setting this up is to get access to the ETI-Eloquence or IBM ViaVoice language files themselves. Once these files are obtained, you should copy the file IBMECI.dll into the same folder where the current dll resides.

## Stage 2
Once you have access to the Eloquence files and have copied the file ibmeci.dll into the same location as TTSEng.dll, perform the following steps:

1. Open Command Prompt as administrator. This is necessary in order to set up new SAPI voices.
2. Change directories (cd) to where the Eloquence/ViaVoice files are located.
3. Type install, and press enter.
	* Note that if you wish to use this with ETI-Eloquence, the file is called ECI.dll. However, once copied, it should be renamed to IBMECI.dll so that it will be loaded properly.

At this point, you should be able to use Eloquence/IBM ViaVoice with whichever languages you have.

## Known issues

* In some cases, the engine will cut off text. A fully stable solution hasn’t been found yet.
	* This is most noticeable with the say all command in some screen readers.
* If you attempt to use this with anything other than ETI-Eloquence 6.1, it is very likely that you will notice more crashes than if you use it with Eloquence 6.1.
	* This seems to be partially due to issues with ViaVoice and other versions of Eloquence receiving text very rapidly.
