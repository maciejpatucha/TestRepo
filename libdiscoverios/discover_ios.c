#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include <discover_ios.h>
#include <device_properties.h>

typedef enum {
    DEVICETYPE,
    DEVICENAME,
    OSVERSION
} propertytype_t;

typedef struct {
    propertytype_t propertyType;
    char propertyValue[256];
} property_t;

static iosprops_t *GetDeviceProps(char *udid);
static property_t *ParseOutput(char *buffer);
static iosprops_t *ParseOutputWithSIMCard(char *buffer, char *udid);
static iosprops_t *ParseOutputWithoutSIMCard(char *buffer, char *udid);
static iosdevice_t ParseDeviceTypeString(char *deviceTypeString);

devicelist_t *GetConnectediOSDevices()
{
    int fd[2];

    if (pipe(fd) == -1)
    {
        printf("pipe() failed\n");
        return NULL;
    }

    pid_t iDevicePID = fork();

    if (iDevicePID == 0)
    {
        close(fd[0]);
        dup2(fd[1],1);
        dup2(fd[1],2);
        close(fd[1]);
        execl("/usr/local/bin/idevice_id", "idevice_id", "-l", NULL);
    }

    close(fd[1]);
    int status;

    waitpid(iDevicePID, &status, 0);

    if (WEXITSTATUS(status) != 0)
    {
        printf("Couldn't find any connected iOS device\n");
        return NULL;
    }

    char buffer[41];
    FILE *input = fdopen(fd[0], "r");
    if (input == NULL)
    {
        printf("fdopen() failed: %s\n", strerror(errno));
        return NULL;
    }
    int deviceCount = 0;
    char **udidList = (char **)calloc(100, sizeof(char *));

    while(fgets(buffer, 41, input))
    {
        if (buffer[0] == '\n')
        {
            continue;
        }

        if (deviceCount < 100)
        {
            udidList[deviceCount++] = strdup(buffer);
        }
        else
        {
            printf("Reached max amount of devices connected\n");
            break;
        }
    }
    fclose(input);

    devicelist_t *iosDeviceList = CreateDeviceList();
    if (iosDeviceList == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < deviceCount; i++)
    {
        iosprops_t *props = GetDeviceProps(udidList[i]);
        if (AddElementToDeviceList(iosDeviceList, (void *)props) == false)
        {
            printf("Failed to add element to list\n");
            return NULL;
        }

        free(udidList[i]);
    }

    free(udidList);

    return iosDeviceList;
}

static iosprops_t *GetDeviceProps(char *udid)
{
    int fd[2];

    if (pipe(fd) == -1)
    {
        printf("pipe() failed\n");
        return NULL;
    }

    pid_t idevicePID = fork();

    if(idevicePID == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        dup2(fd[1], 2);
        close(fd[1]);
        execl("/usr/local/bin/ideviceinfo", "ideviceinfo", "-s", "-u", udid, NULL);
    }

    close(fd[1]);
    FILE *input = fdopen(fd[0], "r");
    char ideviceBuffer[1024];
    iosprops_t *newProps = (iosprops_t *) calloc(1, sizeof(iosprops_t));

    strncpy(newProps->udid, udid, sizeof(newProps->udid));

    while(fgets(ideviceBuffer, 1024, input))
    {
        property_t *prop = ParseOutput(ideviceBuffer);

        if (prop != NULL)
        {
            switch (prop->propertyType)
            {
            case DEVICENAME:
                strncpy(newProps->deviceName, prop->propertyValue, sizeof(newProps->deviceName));
                free(prop);
                break;
            case DEVICETYPE:
                newProps->deviceType = ParseDeviceTypeString(prop->propertyValue);
                free(prop);
                break;
            case OSVERSION:
                strncpy(newProps->osVersion, prop->propertyValue, sizeof(newProps->osVersion));
                free(prop);
                break;
            }
        }
    }
    fclose(input);

    return newProps;
}

static property_t *ParseOutput(char *buffer)
{
    property_t *prop = (property_t *)calloc(1, sizeof(property_t));

    if (strncmp(buffer, "DeviceName:", strlen("DeviceName:")) == 0)
    {
        
        prop->propertyType = DEVICENAME;
        sscanf(
               buffer,
               "%*s %[0-9a-zA-Z,.:- ]s",
               prop->propertyValue
               );
        return prop;
    }
    else if (strncmp(buffer, "ProductType:", strlen("ProductType:")) == 0)
    {
        prop->propertyType = DEVICETYPE;
        sscanf(
               buffer,
               "%*s %s",
               prop->propertyValue
               );
        return prop; 
    }
    else if (strncmp(buffer, "ProductVersion:", strlen("ProductVersion:")) == 0)
    {
        prop->propertyType = OSVERSION;
        sscanf(
               buffer,
               "%*s %s",
               prop->propertyValue
               );
        return prop;
    }
    
    free(prop);

    return NULL;
}

static iosprops_t *ParseOutputWithSIMCard(char *buffer, char *udid)
{
    char deviceName[256];
    char deviceType[16];
    char osVersion[8];

    sscanf(buffer, 
           "%*s %*s"
           "%*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %[0-9a-zA-Z,.:- ]s",
           deviceName);
    sscanf(buffer,
           "%*s %*s"
           "%*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s"
           "%*s %s"
           "%*s %s",
           deviceType,
           osVersion); 

    iosprops_t *newProps = calloc(1, sizeof(iosprops_t));

    if (newProps == NULL)
    {
        printf("calloc() failed\n");
        return NULL;
    }

    strncpy(newProps->udid, udid, sizeof(newProps->udid));
    strncpy(newProps->deviceName, deviceName, sizeof(deviceName));
    strncpy(newProps->osVersion, osVersion, sizeof(osVersion));
    newProps->deviceType = ParseDeviceTypeString(deviceType);

    return newProps;
}

static iosprops_t *ParseOutputWithoutSIMCard(char *buffer, char *udid)
{
    char deviceName[256];
    char deviceType[16];
    char osVersion[8];

    sscanf(buffer,
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %[0-9a-zA-Z,.:- ]s",
           deviceName);
    sscanf(buffer,
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*[0-9a-zA-Z,.:- ]s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s %*s"
           "%*s"
           "%*s %s"
           "%*s %s",
           deviceType,
           osVersion);
    printf("devicename: %s\n"
           "deviceType: %s\n"
           "osVersion: %s\n",
           deviceName,
           deviceType,
           osVersion);
    iosprops_t *newProps = calloc(1, sizeof(iosprops_t));

    if (newProps == NULL)
    {
        printf("calloc() failed\n");
        return NULL;
    }

    strncpy(newProps->udid, udid, sizeof(newProps->udid));
    strncpy(newProps->deviceName, deviceName, sizeof(deviceName));
    strncpy(newProps->osVersion, osVersion, sizeof(osVersion));
    newProps->deviceType = ParseDeviceTypeString(deviceType);

    return newProps;
}

static iosdevice_t ParseDeviceTypeString(char *deviceTypeString)
{
    if ((strncmp(deviceTypeString, "iPhone3,1", 9) == 0) || (strncmp(deviceTypeString, "iPhone3,2", 9) == 0) || (strncmp(deviceTypeString, "iPhone3,3", 9) == 0))
    {
        return IPHONE4;
    }
    else if (strncmp(deviceTypeString, "iPhone4,1", 9) == 0)
    {
        return IPHONE4S;
    }
    else if ((strncmp(deviceTypeString, "iPhone5,1", 9) == 0) || (strncmp(deviceTypeString, "iPhone5,2", 9) == 0))
    {
        return IPHONE5;
    }
    else if ((strncmp(deviceTypeString, "iPhone5,3", 9) == 0) || (strncmp(deviceTypeString, "iPhone5,4", 9) == 0))
    {
        return IPHONE5C;
    }
    else if ((strncmp(deviceTypeString, "iPhone6,1", 9) == 0) || (strncmp(deviceTypeString, "iPhone6,2", 9) == 0))
    {
        return IPHONE5S;
    }
    else if (strncmp(deviceTypeString, "iPhone7,2", 9) == 0)
    {
        return IPHONE6;
    }
    else if (strncmp(deviceTypeString, "iPhone7,1", 9) == 0)
    {
        return IPHONE6PLUS;
    }
    else if (strncmp(deviceTypeString, "iPad1,1", 7) == 0)
    {
        return IPAD;
    }
    else if ((strncmp(deviceTypeString, "iPad2,1", 7) == 0) || (strncmp(deviceTypeString, "iPad2,2", 7) == 0) || (strncmp(deviceTypeString, "iPad2,3", 7) == 0) || (strncmp(deviceTypeString, "iPad2,4", 7) == 0))
    {
        return IPAD2;
    }
    else if ((strncmp(deviceTypeString, "iPad3,1", 7) == 0) || (strncmp(deviceTypeString, "iPad3,2", 7) == 0) || (strncmp(deviceTypeString, "iPad3,3", 7) == 0))
    {
        return IPAD3;
    }
    else if ((strncmp(deviceTypeString, "iPad3,4", 7) == 0) || (strncmp(deviceTypeString, "iPad3,5", 7) == 0) || (strncmp(deviceTypeString, "iPad3,6", 7) == 0))
    {
        return IPAD4;
    }
    else if ((strncmp(deviceTypeString, "iPad4,1", 7) == 0) || (strncmp(deviceTypeString, "iPad4,2", 7) == 0) || (strncmp(deviceTypeString, "iPad4,3", 7) == 0))
    {
        return IPADAIR;
    }
    else if ((strncmp(deviceTypeString, "iPad5,3", 7) == 0) || (strncmp(deviceTypeString, "iPad5,4", 7) == 0))
    {
        return IPADAIR2;
    }
    else if ((strncmp(deviceTypeString, "iPad2,5", 7) == 0) || (strncmp(deviceTypeString, "iPad2,6", 7) == 0) || (strncmp(deviceTypeString, "iPad2,7", 7) == 0))
    {
        return IPADMINI;
    }
    else if ((strncmp(deviceTypeString, "iPad4,4", 7) == 0) || (strncmp(deviceTypeString, "iPad4,5", 7) == 0) || (strncmp(deviceTypeString, "iPad4,6", 7) == 0))
    {
        return IPADMINI2;
    }
    else if ((strncmp(deviceTypeString, "iPad4,7", 7) == 0) || (strncmp(deviceTypeString, "iPad4,8", 7) == 0) || (strncmp(deviceTypeString, "iPad4,9", 7) == 0))
    {
        return IPADMINI3;
    }
    else
    {
        return UNKNOWN;
    }
}
