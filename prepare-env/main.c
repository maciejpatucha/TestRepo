#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <gen_test_config.h>
#include <device_list.h>
#include <discover_android.h>
#include <discover_ios.h>

static void ParseInput(int argc, char **argv);
static void Usage();
static const char **GetMethodsList(char *input, int numberOfMethods);

#define isNull(element) {\
    if (element == NULL) \
    { \
        printf(#element" is missing!!!\n"); \
        Usage(); \
    } \
} 

int main(int argc, char **argv)
{
    ParseInput(argc, argv);

    return 0;   
}

static void ParseInput(int argc, char **argv)
{
    static struct option cliArgs[] = {
        {"output-path",         required_argument, NULL, 'o'},
        {"suite-name",          required_argument, NULL, 's'},
        {"test-name",           required_argument, NULL, 't'},
        {"app-path",            required_argument, NULL, 'a'},
        {"class-name",          required_argument, NULL, 'c'},
        {"methods-list",        required_argument, NULL, 'm'},
        {"number-of-methods",   required_argument, NULL, 'n'},
        {"platform-name",       required_argument, NULL, 'p'},
        {"help",                required_argument, NULL, 'h'},
        {NULL,                  0,                 NULL, 0} 
    };

    int ch;
    const char *outputPath = NULL;
    const char *suiteName = NULL;
    const char *testName = NULL;
    const char *appPath = NULL;
    const char *className = NULL;
    const char **methodsList = NULL;
    char *platformName = NULL;
    int numberOfMethods = 0;
    char *input = NULL;

    if (argc <= 1)
    {
        Usage();
    }

    while ((ch = getopt_long(argc, argv, "o:s:t:a:c:m:n:p:h", cliArgs, NULL)) != -1)
    {
        switch (ch)
        {
        case 'o':
            outputPath = strdup(optarg);
            break;
        case 's':
            suiteName = strdup(optarg);
            break;
        case 't':
            testName = strdup(optarg);
            break;
        case 'a':
            appPath = strdup(optarg);
            break;
        case 'c':
            className = strdup(optarg);
            break;
        case 'm':
            input = strdup(optarg);
            break;
        case 'n':
            numberOfMethods = atoi(optarg);
            break;
        case 'p':
            platformName = strdup(optarg);
            break;
        case 'h':
        default:
            Usage();
        }
    }

    methodsList = GetMethodsList(input, numberOfMethods);

    isNull(outputPath);
    isNull(suiteName);
    isNull(testName);
    isNull(appPath);
    isNull(className);
    isNull(methodsList);
    isNull(platformName);

    if (strncasecmp(platformName, "ios", strlen("ios")) == 0)
    {
        devicelist_t *iosDevices = GetConnectediOSDevices();

        if (iosDevices == NULL)
        {
            printf("Failed to retrive iOS devices...\n");
            _exit(1);
        }

        if (CreateIOSConfig(iosDevices, outputPath, suiteName, testName, appPath, className, methodsList, numberOfMethods))
        {
            printf("Configuration for iOS devices generated to %s\n", outputPath);
        }
        else
        {
            printf("Failed to generate configuration for iOS devices!!!\n");
        }

        CleanupDeviceList(iosDevices);
    }
    else if (strncasecmp(platformName, "android", strlen("android")) == 0)
    {
        devicelist_t *androidDevices = GetConnectedAndroidDevices();

        if (androidDevices == NULL)
        {
            printf("Failed to retrive android devices...\n");
            _exit(1);
        }

        if (CreateAndroidConfig(androidDevices, outputPath, suiteName, testName, appPath, className, (const char **)methodsList, numberOfMethods))
        {
            printf("Configuration for Android devices generated to %s\n", outputPath);
        }
        else
        {
            printf("Failed to generate configuration for Android devices!!!\n");
        }
    }
    else
    {
        printf("UNKNOWN PLATFORM NAME!!!\n");
        Usage();
        _exit(1);
    }
}

static void Usage()
{
    printf("\n");
    printf("Available options"
           "\n"
           "-o, --output-path    - path where to save generated configuration,\n"
           "-s, --suite-name     - name of the test suite,\n"
           "-t, --test-name      - name of the test,\n"
           "-a, --app-path       - path to the application used for testing,\n"
           "-c, --class-name     - name of the class to be executed when testing,\n"
           "-m, --methods-list   - comma separated list of all methods to be executed when testing,\n"
           "-p, --platform-name  - name of the platform (iOS, Android),\n"
           "-h, --help           - print this message.\n\n");

    _exit(1);
}

static const char **GetMethodsList(char *input, int numberOfMethods)
{
    char *tmp = input, *string = NULL;

    const char **methods = (const char **) calloc(numberOfMethods, sizeof(char *));

    if (methods == NULL)
    {
        printf("Failed to allocate memory for methods list...\n");
        _exit(1);
    }

    int i = 0;
    printf("input: %s\n", input);
    while ((methods[i++] = strdup(strsep(&input, ","))) != NULL)
    {
        if (i >= numberOfMethods)
        {
            break;
        }
    }

    printf("methods[0]: %s\n", methods[0]);

    free(tmp);

    return methods;
}
