[Defines]
  PLATFORM_NAME           = BootPkg
  PLATFORM_GUID           = F55E484A-C251-4E4C-8033-78AD6AE2AFDB
  PLATFORM_VERSION        = 1.0
  DSC_SPECIFICATION       = 0x00010005
  OUTPUT_DIRECTORY        = Build/Boot
  SUPPORTED_ARCHITECTURES = X64
  BUILD_TARGETS           = DEBUG|RELEASE
  SKUID_IDENTIFIER        = DEFAULT

!include MdePkg/MdeLibs.dsc.inc

[PcdsFixedAtBuild]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27

[Components]
  ../BootPkg/Library/ElfLib/ElfLib.inf
  ../BootPkg/Library/GraphicsOutputLib/GraphicsOutputLib.inf
  ../BootPkg/Library/MemoryMapLib/MemoryMapLib.inf
  ../BootPkg/Loader/Loader.inf

[LibraryClasses]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  ElfLib|../BootPkg/Library/ElfLib/ElfLib.inf
  GraphicsOutputLib|../BootPkg/Library/GraphicsOutputLib/GraphicsOutputLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  MemoryMapLib|../BootPkg/Library/MemoryMapLib/MemoryMapLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf

[BuildOptions]
  GCC:RELEASE_*_*_CC_FLAGS = -DMDEPKG_NDEBUG
  GCC:DEBUG_*_*_CC_FLAGS = -O0
