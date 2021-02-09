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

## Known issues

* In some cases, the engine will cut off the ends of speech fragments. A fully stable solution hasn’t been found yet.

