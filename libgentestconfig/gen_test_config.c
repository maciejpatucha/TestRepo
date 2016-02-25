#include <gen_test_config.h>
#include <device_properties.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ANDROID_INITIAL_PORT 4000
#define IOS_INITIAL_PORT 5000

FILE *testConfig = NULL;

static bool OpenConfigFile(const char *outputPath);
static void CloseConfigFile();
static void GenHeader(const char *suiteName, bool parallel);

bool CreateAndroidConfig(devicelist_t *androidDeviceList, 
                         const char *outputPath, 
                         const char *suiteName, 
                         const char *testName,
                         const char *appPath,
                         const char *className,
                         const char **methodsList,
                         int numberOfMethods)
{
    OpenConfigFile(outputPath);

    GenHeader(suiteName, true);

    if (androidDeviceList->elementCount == 0)
    {
        return false;
    }

    androidprops_t *properties = NULL;

    for (int i = 0; i < androidDeviceList->elementCount; i++)
    {
        devicelistelement_t *element = GetElementByID(androidDeviceList, i);
        properties = (androidprops_t *) element->data;

        fprintf(testConfig,
                "<test name=\"%s for %s\" preserve-order=\"true\">\n"
                "   <parameter name=\"platform-name\" value=\"Android\" />\n"
                "   <parameter name=\"platform-version\" value=\"%s\" />\n"
                "   <parameter name=\"device-name\" value=\"%s\" />\n"
                "   <parameter name=\"app\" value=\"%s\" />\n"
                "   <parameter name=\"hub\" value=\"http://127.0.0.1:%d/\" />\n"
                "   <classes>\n"
                "       <class name=\"%s\" >\n"
                "           <methods>\n",
                testName,
                properties->deviceModel,
                properties->androidVersion,
                properties->deviceModel,
                appPath,
                ANDROID_INITIAL_PORT + i,
                className);

        for (int j = 0; j < numberOfMethods; j++)
        {
            fprintf(testConfig,
                "               <include name=\"%s\" />\n",
                methodsList[j]);
        }

        fprintf(testConfig,
                "           </methods>\n"
                "       </class>\n"
                "   </classes>\n"
                "</test>\n\n");
    }

    fprintf(testConfig, "</suite>\n");

    CloseConfigFile();

    return true;
}

bool CreateIOSConfig(devicelist_t *iOSDeviceList, 
                     const char *outputPath, 
                     const char *suiteName, 
                     const char *testName,
                     const char *appPath,
                     const char *className,
                     const char **methodsList,
                     int numberOfMethods)
{
    OpenConfigFile(outputPath);

    GenHeader(suiteName, false);

    if (iOSDeviceList->elementCount == 0)
    {
        return false;
    }

    iosprops_t *properties = NULL;

    for (int i = 0; i < iOSDeviceList->elementCount; i++)
    {
        devicelistelement_t *element = GetElementByID(iOSDeviceList, i);
        properties = (iosprops_t *) element->data;

        fprintf(testConfig,
                "<test name=\"%s for %s\" preserve-order=\"true\">\n"
                "   <parameter name=\"platform-name\" value=\"iOS\" />\n"
                "   <parameter name=\"platform-version\" value=\"%s\" />\n"
                "   <parameter name=\"device-name\" value=\"%s\" />\n"
                "   <parameter name=\"app\" value=\"%s\" />\n"
                "   <parameter name=\"hub\" value=\"http://127.0.0.1:%d/\" />\n"
                "   <parameter name=\"udid\" value=\"%s\" />\n"
                "   <classes>\n"
                "       <class name=\"%s\" >\n"
                "           <methods>\n",
                testName,
                properties->deviceName,
                properties->osVersion,
                properties->deviceName,
                appPath,
                IOS_INITIAL_PORT + i,
                properties->udid,
                className);

        for (int j = 0; j < numberOfMethods; j++)
        {
            fprintf(testConfig,
                "               <include name=\"%s\" />\n",
                methodsList[j]);
        }

        fprintf(testConfig,
                "           </methods>\n"
                "       </class>\n"
                "   </classes>\n"
                "</test>\n\n");
    }

    fprintf(testConfig, "</suite>\n");

    CloseConfigFile();

    return true;
}

static bool OpenConfigFile(const char *outputPath)
{
    testConfig = fopen(outputPath, "wb");

    if (testConfig == NULL)
    {
        return false;
    }

    return true;
}

static void CloseConfigFile()
{
    fclose(testConfig);
}

static void GenHeader(const char *suiteName, bool parallel)
{
    fprintf(testConfig, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<!DOCTYPE suite SYSTEM \"http://testng.org/testng-1.0.dtd\">\n"
                        "<suite name=\"%s\" parallel=\"%s\" verbose=\"1\" thread-count=\"8\">\n\n",
                        suiteName,
                        parallel == true ? "tests" : "false");
}

