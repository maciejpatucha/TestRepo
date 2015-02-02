#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "discover_ios_threads.h"

static iosprops_t *ParseOutput(char *buffer, char *udid);
static iosprops_t *ParseOutputWithSIMCard(char *buffer, char *udid);
static iosprops_t *ParseOutputWithoutSIMCard(char *buffer, char *udid);
static iosdevice_t ParseDeviceTypeString(char *deviceTypeString);

void* GetInfo(void *arg)
{
    char *serialNo = (char *) arg;
    int fd[2];

    if (pipe(fd) == -1)
    {
        printf("pipe() failed.\n");
        return NULL;        
    }

    pid_t idevicePID = fork();

    if (idevicePID == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        dup2(fd[1], 2);
        close(fd[1]);
        execl("/usr/local/bin/ideviceinfo", "ideviceinfo", "-s", "-u", serialNo, NULL);
    }

    close(fd[1]);
    FILE *input = fdopen(fd[0], "r");
    char ideviceBuffer[2048];

    while (fgets(ideviceBuffer, 2048, input));

    return (void *) ParseOutput(ideviceBuffer, serialNo);
}

iosprops_t *GetDeviceInfo(char *deviceSerialNo)
{
    pthread_t getInfoThread;

    if (pthread_create(&getInfoThread, NULL, GetInfo, (void *)deviceSerialNo) == -1)
    {
        printf("Failed to create getInfoThread...\n");
        return NULL;
    }

    void *result;
    if (pthread_join(getInfoThread, &result) == -1)
    {
        printf("Failed to join getInfoThread...\n");
        return NULL;
    }

    return (iosprops_t *) result;
}

static iosprops_t *ParseOutput(char *buffer, char *udid)
{
    if (strncmp(buffer, "BasebandCertId:", 16) == 0)
    {
        return ParseOutputWithSIMCard(buffer, udid);
    }
    else
    {
        return ParseOutputWithoutSIMCard(buffer, udid);
    }

    //Should never get here!!!
    printf("SOMETHING WENT TERRIBLY WRONG!!!!\n");

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
           "%*s %*s"
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
