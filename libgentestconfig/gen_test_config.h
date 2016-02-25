#ifndef GEN_TEST_CONFIG_H
#define GEN_TEST_CONFIG_H

#include <device_list.h>

bool CreateAndroidConfig(devicelist_t *androidDeviceList,
                         const char *outputPath, 
                         const char *suiteName, 
                         const char *testName, 
                         const char *appPath, 
                         const char *className,
                         const char **methodsList,
                         int numberOfMethods);
bool CreateIOSConfig(devicelist_t* iOSDeviceList, 
                     const char *outputPath, 
                     const char *suiteName, 
                     const char *testName,
                     const char *appPath,
                     const char *className,
                     const char **methodsList,
                     int numberOfMethods);

#endif
