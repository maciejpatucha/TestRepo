#ifndef DEVICE_LIST_H
#define DEVICE_LIST_H

#include <stdbool.h>

typedef struct _devicelistelement {
    int id;
    void *data;
    struct _devicelistelement *nextElement;
} devicelistelement_t;

typedef struct _devicelist {
    devicelistelement_t *firstElement;
    int elementCount;
    int maxElements;
    devicelistelement_t **elements;
} devicelist_t;

devicelist_t *CreateDeviceList();
bool AddElementToDeviceList(devicelist_t *list, void *data);
devicelistelement_t *GetElementByID(devicelist_t *list, int elementID);
void CleanupDeviceList(devicelist_t *list);

#endif // DEVICE_LIST_H
