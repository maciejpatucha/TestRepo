#include <stdio.h>
#include <stdbool.h>

#include "device_list.h"
#include "device_properties.h"
#include "discover_android.h"

static void DeviceListTest();
static void DevicePropertiesAndroidTest();
static void DevicePropertiesiOSTest();
static void AndroidDiscoveryTest();

int main()
{
    DeviceListTest();
    DevicePropertiesAndroidTest();
    DevicePropertiesiOSTest();
    AndroidDiscoveryTest();
    return 0;   
}

static void DeviceListTest()
{
    printf("\nTesting device list library...\n\n");
    printf("Creating device list...");
    
    devicelist_t *newList = CreateDeviceList();
    
    if (newList == NULL)
    {
        printf("FAILED\n");
        return;
    }

    printf("SUCCESS\n");

    printf("Adding new element to list...");
    char *data = "Some data";
    if (AddElementToDeviceList(newList, (void *)data) == false)
    {
        printf("FAILED\n");
        return;
    }

    printf("SUCCESS\n");

    printf("Cleaning up device list...SUCCESS\n");
}

static void DevicePropertiesAndroidTest()
{
    printf("\nTesting device properties library...\n\n");
    printf("Creating android device property...");
    
    androidprops_t *androidProps = CreateAndroidProperties("dfsdfsdf", "Samsung", "Galaxy S4", "6.6.6");

    if (androidProps == NULL)
    {
        printf("FAILED\n");
        return;
    }

    printf("SUCCESS\n");
    printf("Printing android device properties:...\n");
    PrintProperties(ANDROID, (void *) androidProps);
    printf("Cleaning up android device properties...SUCCESS\n");
    CleanupProperties(ANDROID, (void *) androidProps);
}

static void DevicePropertiesiOSTest()
{
    printf("\nTesting device properties library...\n\n");
    printf("Creating ios devie property...");

    iosprops_t *iosProps = CreateiOSProperties("123124d123", "Test device", IPADMINI3, "8.1.2");

    if (iosProps == NULL)
    {
        printf("FAILED\n");
        return;
    }

    printf("SUCCESS\n");
    printf("Printing ios device properties:...\n");
    PrintProperties(IOS, (void *) iosProps);
    printf("Cleaning up ios device properties...SUCCESS\n");
    CleanupProperties(IOS, (void *) iosProps);
}

static void AndroidDiscoveryTest()
{
    printf("\nTesting android device discovery...\n\n");
    devicelist_t *androidDevices = GetConnectedAndroidDevices();
    if (androidDevices == NULL)
    {
        printf("Retriving android device list...FAILED\n");
        return;
    }
    printf("Retriving andoird device list...SUCCESS\n");

    printf("Number of connected andoird devices...%d\n", androidDevices->elementCount);

    if (androidDevices->elementCount > 0)
    {
        androidprops_t *props = NULL;

        for (int i = 0; i < androidDevices->elementCount; i++)
        {
            devicelistelement_t *element = GetElementByID(androidDevices, i); 
            PrintProperties(ANDROID, (androidprops_t *)element->data);
        }
    }
    else
    {
        printf("No connected devices found\n");
    }
}
