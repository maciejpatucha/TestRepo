#ifndef DEVICE_PROPERTIES_H
#define DEVICE_PROPERTIES_H

struct _androidprops
{
    char deviceID[16];
    char deviceManufacturer[16];
    char deviceModel[64];
    char androidVersion[8];
};

typedef struct _androidprops androidprops_t;

enum  _iosdevice{
    IPHONE4,
    IPHONE4S,
    IPHONE5,
    IPHONE5C,
    IPHONE5S,
    IPHONE6,
    IPHONE6PLUS,
    IPAD,
    IPAD2,
    IPAD3,
    IPAD4,
    IPADAIR,
    IPADAIR2,
    IPADMINI,
    IPADMINI2,
    IPADMINI3
};

typedef enum _iosdevice iosdevice_t;

enum _proptype{
    ANDROID,
    IOS
};

typedef enum _proptype proptype_t;

struct _iosprops
{
    char udid[41];
    char deviceName[256];
    iosdevice_t deviceType;
    char osVersion[8];
};

typedef struct _iosprops iosprops_t;

androidprops_t *CreateAndroidProperties(char *deviceID,
                                        char *deviceManufacturer,
                                        char *deviceModel,
                                        char *androidVersion);
iosprops_t *CreateiOSProperties(char *udid,
                                char *deviceName,
                                iosdevice_t deviceType,
                                char *osVersion);
void PrintProperties(proptype_t propType, void *props);
void CleanupProperties(proptype_t propType, void *props);

#endif // DEVICE_PROPERTIES_H
