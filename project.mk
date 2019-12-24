# #############################################################################
# Project Customization
# #############################################################################

PROJECT = modfx_flanger_1

UCSRC = $(wildcard ../user/lib/*.c)

UCXXSRC = ../user/flanger.cpp

# NOTE: Relative path from `Makefile` that refer this file.
UINCDIR = ../user/lib

UDEFS =

ULIB = 

ULIBDIR =
