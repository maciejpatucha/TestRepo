#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <discover_android.h>
#include <device_properties.h>

static char *ParseBuffer(char *buffer);
static androidprops_t *GetDeviceProps(char *deviceID);
static void StartADBDaemon();
static char *GetDeviceManufacturer(char *deviceSerialNo);
static char *GetDeviceModel(char *deviceSerialNo);
static char *GetDeviceOSVersion(char *deviceSerialNo);

devicelist_t *GetConnectedAndroidDevices()
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
        execl("/usr/local/bin/adb", "adb", "devices", "-l", NULL);
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
    fclose(input);

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
        androidprops_t *props = GetDeviceProps(serialList[i]);
        
        if (AddElementToDeviceList(deviceList, (void *)props) == false)
        {
            printf("Failed to element to deviceList\n");
            return NULL;
        }

        free(serialList[i]);
    }

    free(serialList);

    return deviceList;
}

static androidprops_t *GetDeviceProps(char *deviceID)
{
    char *manufacturer, *model, *version;

    if ((manufacturer = GetDeviceManufacturer(deviceID)) == NULL)
    {
        return (void *) -1;
    }

    if ((model = GetDeviceModel(deviceID)) == NULL)
    {
        return (void *) -1;
    }

    if ((version = GetDeviceOSVersion(deviceID)) == NULL)
    {
        return (void *) -1;
    }

    androidprops_t *newProp;
    
    if ((newProp = CreateAndroidProperties(deviceID, manufacturer, model, version)) == NULL)
    {
        printf("Failed to create new property.\n");
        return (void *) -2;
    }
    
    free(manufacturer);
    free(model);
    free(version);

    return (void *) newProp;
}

static char *ParseBuffer(char *buffer)
{
    if ((strncmp(buffer, "List of devices", 15) == 0) || (buffer[0] == '\n'))
    {
        return NULL;
    }
    
    if (strstr(buffer, "device") == NULL)
    {
        return NULL;
    }

    char *serial = calloc(32, sizeof(char));
    sscanf(buffer, "%s %*s", serial);
    memset(buffer, 0, 1024);

    return serial;
}

static void StartADBDaemon(void)
{
    pid_t daemon = fork();

    if (daemon == 0)
    {
        execl("/usr/local/bin/adb", "adb", "start-server", NULL);
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
        execl("/usr/local/bin/adb", "adb", "-s", deviceSerialNo, "shell", "getprop ro.product.manufacturer", NULL);
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
    fclose(input);
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
        execl("/usr/local/bin/adb", "adb", "-s", deviceSerialNo, "shell", "getprop ro.product.model", NULL);
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
    fclose(input);

    model[strlen(model) - 2] = '\0';

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
        execl("/usr/local/bin/adb", "adb", "-s", deviceSerialNo, "shell", "getprop ro.build.version.release", NULL);
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
    fclose(input);

    version[strlen(version) - 2] = '\0';

    return version;
   
}
