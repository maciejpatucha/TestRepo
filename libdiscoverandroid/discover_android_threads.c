#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "discover_android_threads.h"

static char *GetDeviceManufacturer(char *deviceSerialNo);
static char *GetDeviceModel(char *deviceSerialNo);
static char *GetDeviceOSVersion(char *deviceSerialNo);

void* GetInfo(void *arg)
{
    char *serialNo = (char *) arg;
    char *manufacturer, *model, *version;

    if ((manufacturer = GetDeviceManufacturer(serialNo)) == NULL)
    {
        return (void *) -1;
    }

    if ((model = GetDeviceModel(serialNo)) == NULL)
    {
        return (void *) -1;
    }

    if ((version = GetDeviceOSVersion(serialNo)) == NULL)
    {
        return (void *) -1;
    }

    androidprops_t *newProp;
    
    if ((newProp = CreateAndroidProperties(serialNo, manufacturer, model, version)) == NULL)
    {
        printf("Failed to create new property.\n");
        return (void *) -2;
    }
    
    free(manufacturer);
    free(model);
    free(version);

    return (void *) newProp;
}

androidprops_t *GetDeviceInfo(char *deviceSerialNo)
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

    return (androidprops_t *) result;
}

void StartADBDaemon(void)
{
    pid_t daemon = fork();

    if (daemon == 0)
    {
        execl("/usr/bin/adb", "adb", "start-server", NULL);
    }

    printf("Starting adb server. Please wait...\n");

    int status;

    waitpid(daemon, &status, 0);
}

static char *GetDeviceManufacturer(char *deviceSerialNo)
{
    int fd[2];

    pipe(fd);

    pid_t adbShell = fork();

    if (adbShell == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        dup2(fd[1], 2);
        close(fd[1]);
        execl("/usr/bin/adb", "adb", "-s", deviceSerialNo, "shell", "getprop ro.product.manufacturer", NULL);
    }

    char *manufacturer = calloc(16, sizeof(char));

    if (manufacturer == NULL)
    {
        printf("Failed to allocate memory for manufacturer...\n");
        return NULL;
    }

    close(fd[1]);
    FILE *input = fdopen(fd[0], "r");
    while (fgets(manufacturer, 16, input));

    return manufacturer;
}

static char *GetDeviceModel(char *deviceSerialNo)
{
    int fd[2];

    pipe(fd);

    pid_t adbShell = fork();

    if (adbShell == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        dup2(fd[1], 2);
        close(fd[1]);
        execl("/usr/bin/adb", "adb", "-s", deviceSerialNo, "shell", "getprop ro.product.model", NULL);
    }

    char *model = calloc(64, sizeof(char));

    if (model == NULL)
    {
        printf("Failed to allocate memory for model...\n");
        return NULL;
    }

    close(fd[1]);
    FILE *input = fdopen(fd[0], "r");
    while (fgets(model, 64, input));

    return model;
}

static char *GetDeviceOSVersion(char *deviceSerialNo)
{
    int fd[2];

    pipe(fd);

    pid_t adbShell = fork();

    if (adbShell == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        dup2(fd[1], 2);
        close(fd[1]);
        execl("/usr/bin/adb", "adb", "-s", deviceSerialNo, "shell", "getprop ro.build.version", NULL);
    }

    char *version = calloc(8, sizeof(char));

    if (version == NULL)
    {
        printf("Failed to allocate memory for version...\n");
        return NULL;
    }

    close(fd[1]);
    FILE *input = fdopen(fd[0], "r");
    while (fgets(version, 8, input));

    return version;
   
}
