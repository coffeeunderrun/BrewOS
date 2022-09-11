#ifndef PSF_FONT_LIB_H
#define PSF_FONT_LIB_H

#include <Uefi.h>

/*** PSF VERSION 1 ***/

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

#define PSF1_MODE512 0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE 0x05

#define PSF1_SEPARATOR 0xFFFF
#define PSF1_START 0xFFFE

typedef struct Psf1Header
{
    UINT8 magic[2];
    UINT8 mode;
    UINT8 glyphSize;
} Psf1Header;

/*** PSF VERSION 2 ***/

#define PSF2_MAGIC0 0x72
#define PSF2_MAGIC1 0xB5
#define PSF2_MAGIC2 0x4A
#define PSF2_MAGIC3 0x86

#define PSF2_HAS_UNICODE_TABLE 0x01

#define PSF2_MAXVERSION 0

#define PSF2_SEPARATOR 0xFF
#define PSF2_STARTSEQ 0xFE

typedef struct Psf2Header
{
    UINT8 magic[4];
    UINT32 version;
    UINT32 headerSize;
    UINT32 flags;
    UINT32 glyphCount;
    UINT32 glyphSize;
    UINT32 glyphHeight;
    UINT32 glyphWidth;
} Psf2Header;

typedef struct PsfImage
{
    VOID *glyphBuffer;
    UINT32 glyphCount;
    UINT32 glyphSize;
    UINT32 glyphHeight;
    UINT32 glyphWidth;
} PsfImage;

EFI_STATUS LoadPsfImage(CHAR16 *fileName, EFI_HANDLE handle, PsfImage *image);

#endif // PSF_FONT_LIB_H
