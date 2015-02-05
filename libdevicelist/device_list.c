#include <stdio.h>
#include <stdlib.h>

#include <device_list.h>

static bool ReallocateArray(devicelistelement_t **array, int arraySize);

devicelist_t *CreateDeviceList()
{
    devicelist_t *newList = (devicelist_t *) calloc(1, sizeof(devicelist_t));

    if (newList == NULL)
    {
        printf("Failed to allocate memory for new list.\n");
        return NULL;
    }

    newList->firstElement = NULL;
    newList->elementCount = 0;
    newList->maxElements = 20;
    newList->elements = (devicelistelement_t **) calloc(20, sizeof(devicelistelement_t*));

    if (newList->elements == NULL)
    {
        printf("Failed to allocate memory for elements array.\n");
        return NULL;
    }

    return newList;
}

bool AddElementToDeviceList(devicelist_t *list, void *data)
{
    if ((list->elementCount + 1) >= list->maxElements)
    {
        if (!ReallocateArray(list->elements, list->elementCount))
        {
            printf("Cannot add new item to list.\n");
            return false;
        }
        list->maxElements += 20;
    }

    devicelistelement_t *element = (devicelistelement_t *) calloc(1, sizeof(devicelistelement_t));
    element->data = data;\
    element->id = list->elementCount;
    element->nextElement = list->firstElement;
    list->firstElement = element;
    list->elements[list->elementCount] = element;
    list->elementCount++;

    return true;
}

void CleanupDeviceList(devicelist_t *list)
{
    free(list->elements);

    while (list->firstElement != NULL)
    {
        struct _devicelistelement *tmp = list->firstElement->nextElement;
        free(list->firstElement->data);
        free(list->firstElement);
        list->firstElement = tmp;
    }

    free(list);
}

devicelistelement_t *GetElementByID(devicelist_t *list, int elementID)
{
    if (elementID >= list->elementCount)
    {
        return NULL;
    }

    return (list->elements[elementID]);
}

static bool ReallocateArray(devicelistelement_t **array, int arraySize)
{
    int newSize = arraySize + 20;

    devicelistelement_t *tmp = (devicelistelement_t *) calloc(newSize, sizeof(devicelistelement_t));

    if (tmp == NULL)
    {
        printf("Failed to resize array\n");
        return false;
    }
    int i;
    for (i = 0; i < arraySize; i++)
    {
        tmp[i] = (*array)[i];
    }

    free(array);

    *array = tmp;

    return true;
}
