#ifndef COMMON_H
#define COMMON_H

#define RETURN_IF_ERROR_STATUS(status)                                      \
    if (EFI_ERROR(status))                                                  \
    {                                                                       \
        DEBUG((DEBUG_ERROR, "%a %d %d\n", __FUNCTION__, __LINE__, status)); \
        return status;                                                      \
    }

#endif // COMMON_H
