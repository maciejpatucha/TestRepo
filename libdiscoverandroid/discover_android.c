#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "discover_android.h"
#include "discover_android_threads.h"

static char *ParseBuffer(char *buffer);

devicelist_t *GetConnectedDevices()
{
    StartADBDaemon();
    int fd[2];

    if (pipe(fd) == -1)
    {
        printf("pipe() failed\n");
        return NULL;
    }

    pid_t adbPID = fork();

    if (adbPID == 0)
    {
        close(fd[0]);
        dup2(fd[1],1);
        dup2(fd[1],2);
        close(fd[1]);
        execl("/usr/bin/adb", "adb", "devices", NULL);
    }
    char buffer[1024];
    close(fd[1]);
    FILE *input = fdopen(fd[0], "r");

    char **serialList = (char **) calloc(100, sizeof(char *));

    if (serialList == NULL)
    {
        printf("serialList: calloc() failed\n");
        return NULL;
    }

    int deviceCount = 0;

    while (fgets(buffer, 1024, input))
    {
        if (deviceCount >= 100)
        {
            printf("Reached max device count!!!\n");
            break;
        }

        char *serial = ParseBuffer(buffer);
        
        if (serial == NULL)
        {
            continue;
        }

        serialList[deviceCount++] = serial;
    }

    if (deviceCount == 0)
    {
        return NULL;
    }

    devicelist_t *deviceList = CreateDeviceList();

    if (deviceList == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < deviceCount; i++)
    {
        androidprops_t *props = GetDeviceInfo(serialList[i]);
        
        if (AddElementToDeviceList(deviceList, (void *)props) == false)
        {
            printf("Failed to element to deviceList\n");
            return NULL;
        }
    }

    return deviceList;
}

static char *ParseBuffer(char *buffer)
{
    if ((strncmp(buffer, "List of devices", 15) == 0) || (buffer[0] == '\n'))
    {
        return NULL;
    }
    
    char *serial = calloc(16, sizeof(char));
    sscanf(buffer, "%s %*s", serial);
    memset(buffer, 0, 1024);

    return serial;
}
