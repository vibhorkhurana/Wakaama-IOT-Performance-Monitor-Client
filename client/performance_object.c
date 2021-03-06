/*
 * This is a new code to implement the performance statistics of the IOT Device
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define PRV_SYSTEM_TEMP       37
#define PRV_ERROR_CODE        0
#define PRV_CPU_LOAD          10
#define PRV_AVG_LOAD_1MIN     50
#define PRV_AVG_LOAD_5MIN     50
#define PRV_AVG_LOAD_15MIN    50

#define PRV_OFFSET_MAXLEN   7 //+HH:MM\0 at max
#define PRV_TLV_BUFFER_SIZE 128

// Resource Id's:
#define RES_O_SYSTEM_TEMP           5850
#define RES_M_ERROR_CODE            11
#define RES_O_CPU_LOAD              5851
#define RES_O_AVG_LOAD_1MIN         5852
#define RES_O_AVG_LOAD_5MIN         5853
#define RES_O_AVG_LOAD_15MIN        5854

typedef struct
{
    int64_t system_temperature;
    int64_t error;
    int64_t cpu_load;
    int64_t avg_load1; //cpu load averaged over 1 min
    int64_t avg_load2; //cpu load averaged over 5 min
    int64_t avg_load3; //cpu load averaged over 15 min

} device_data_t;


static uint8_t prv_set_value(lwm2m_data_t * dataP,
                             device_data_t * devDataP)
{
    // a simple switch structure is used to respond at the specified resource asked
    printf("DataP id %d",dataP->id);
    switch (dataP->id)
    {
    case RES_O_SYSTEM_TEMP:
        lwm2m_data_encode_int(devDataP->system_temperature, dataP);
        return COAP_205_CONTENT;

    case RES_O_CPU_LOAD:
        lwm2m_data_encode_int(devDataP->cpu_load, dataP);
        return COAP_205_CONTENT;

    case RES_O_AVG_LOAD_1MIN:
        lwm2m_data_encode_int(devDataP->avg_load1, dataP);
        return COAP_205_CONTENT;
    case RES_O_AVG_LOAD_5MIN:
        lwm2m_data_encode_int(devDataP->avg_load2, dataP);
        return COAP_205_CONTENT;

    case RES_O_AVG_LOAD_15MIN:
        lwm2m_data_encode_int(devDataP->avg_load3, dataP);
        return COAP_205_CONTENT;

    case RES_M_ERROR_CODE:
    {
        lwm2m_data_t * subTlvP;

        subTlvP = lwm2m_data_new(1);

        subTlvP[0].id = 0;
        lwm2m_data_encode_int(devDataP->error, subTlvP);

        lwm2m_data_encode_instances(subTlvP, 1, dataP);

        return COAP_205_CONTENT;
    }        

    default:
        return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_device_read(uint16_t instanceId,
                               int * numDataP,
                               lwm2m_data_t ** dataArrayP,
                               lwm2m_object_t * objectP)
{
    uint8_t result;
    int i;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_O_SYSTEM_TEMP,
                RES_O_CPU_LOAD,
                RES_O_AVG_LOAD_1MIN,
                RES_M_ERROR_CODE
        };
        int nbRes = sizeof(resList)/sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {
        result = prv_set_value((*dataArrayP) + i, (device_data_t*)(objectP->userData));
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_device_discover(uint16_t instanceId,
                                   int * numDataP,
                                   lwm2m_data_t ** dataArrayP,
                                   lwm2m_object_t * objectP)
{
    uint8_t result;
    int i;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
        //    RES_O_BATTERY_LEVEL,
            RES_O_SYSTEM_TEMP,
            RES_O_CPU_LOAD,
            RES_O_AVG_LOAD_1MIN,
            RES_O_AVG_LOAD_5MIN,
            RES_O_AVG_LOAD_15MIN,
            RES_M_ERROR_CODE
            };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
            case RES_O_SYSTEM_TEMP:
            case RES_O_CPU_LOAD:
            case RES_O_AVG_LOAD_1MIN:
            case RES_O_AVG_LOAD_5MIN:
            case RES_O_AVG_LOAD_15MIN:
            case RES_M_ERROR_CODE:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_device_write(uint16_t instanceId,
                                int numData,
                                lwm2m_data_t * dataArray,
                                lwm2m_object_t * objectP)
{
    int i;
    uint8_t result;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    i = 0;

    do
    {
        switch (dataArray[i].id)
        {            
        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

void display_performance_object(lwm2m_object_t * object)
{
#ifdef WITH_LOGS
    device_data_t * data = (device_data_t *)object->userData;
    fprintf(stdout, "  /%u: Device object:\r\n", object->objID);
    if (NULL != data)
    {
        fprintf(stdout, "    time: %lld, time_offset: %s\r\n",
                (long long) data->time, data->time_offset);
    }
#endif
}

lwm2m_object_t * get_object_performance()
{
    /*
     * The get_object_performance function create the object itself and return a pointer to the structure that represent it.
     */
    lwm2m_object_t * deviceObj;

    deviceObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != deviceObj)
    {
        memset(deviceObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns his unique ID
         * The 3349 is the new ID for the object "Performance Statistics".
         */
        deviceObj->objID = TEST_OBJECT_ID;

        /*
         * and its unique instance
         *
         */
        deviceObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        if (NULL != deviceObj->instanceList)
        {
            memset(deviceObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(deviceObj);
            return NULL;
        }
        
        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        deviceObj->readFunc     = prv_device_read;
        deviceObj->discoverFunc = prv_device_discover;
        deviceObj->writeFunc    = prv_device_write;
        deviceObj->userData = lwm2m_malloc(sizeof(device_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables 
         */
        if (NULL != deviceObj->userData)
        {
            ((device_data_t*)deviceObj->userData)->cpu_load = PRV_CPU_LOAD;
            ((device_data_t*)deviceObj->userData)->system_temperature = PRV_SYSTEM_TEMP;
            ((device_data_t*)deviceObj->userData)->avg_load1 = PRV_AVG_LOAD_1MIN;
            ((device_data_t*)deviceObj->userData)->avg_load2 = PRV_AVG_LOAD_5MIN;
            ((device_data_t*)deviceObj->userData)->avg_load3 = PRV_AVG_LOAD_15MIN;
            ((device_data_t*)deviceObj->userData)->error = PRV_ERROR_CODE;
        }
        else
        {
            lwm2m_free(deviceObj->instanceList);
            lwm2m_free(deviceObj);
            deviceObj = NULL;
        }
    }

    return deviceObj;
}

void free_object_performance(lwm2m_object_t * objectP)
{
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    if (NULL != objectP->instanceList)
    {
        lwm2m_free(objectP->instanceList);
        objectP->instanceList = NULL;
    }

    lwm2m_free(objectP);
}

uint8_t performance_change(lwm2m_data_t * dataArray,
                      lwm2m_object_t * objectP)
{
    uint8_t result;
    printf("\nds %d ds %d\n",dataArray->value,dataArray->id);
    switch (dataArray->id)
    
    {
        case RES_M_ERROR_CODE:
            if (1 == lwm2m_data_decode_int(dataArray, &((device_data_t*)(objectP->userData))->error))
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        case RES_O_SYSTEM_TEMP:
        {
            int64_t value;
            if (1 == lwm2m_data_decode_int(dataArray, &value))
            {
                if ((0 <= value) && (100000 >= value))
                    {
                ((device_data_t*)(objectP->userData))->system_temperature = value;
                result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                    }
                
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
        }
            break;
        case RES_O_CPU_LOAD:
        {
            int64_t value;
            if (1 == lwm2m_data_decode_int(dataArray, &value))
            {
                if ((0 <= value) && (100000 >= value))
                    {
                ((device_data_t*)(objectP->userData))->cpu_load = value;
                result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                    }
                
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
        }
            break;
        case RES_O_AVG_LOAD_1MIN:
        {
            int64_t value;
            if (1 == lwm2m_data_decode_int(dataArray, &value))
            {
                if ((0 <= value) && (100 >= value))
                    {
                ((device_data_t*)(objectP->userData))->avg_load1 = value;
                result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                    }
                
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
        }
            break;
        case RES_O_AVG_LOAD_5MIN:
        {
            int64_t value;
            if (1 == lwm2m_data_decode_int(dataArray, &value))
            {
                if ((0 <= value) && (100 >= value))
                    {
                ((device_data_t*)(objectP->userData))->avg_load2 = value;
                result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                    }
                
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
        }
            break;  
        case RES_O_AVG_LOAD_15MIN:
        {
            int64_t value;
            if (1 == lwm2m_data_decode_int(dataArray, &value))
            {
                if ((0 <= value) && (100 >= value))
                    {
                ((device_data_t*)(objectP->userData))->avg_load3 = value;
                result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                    }
                
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
        }
            break;          
        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
            break;
        }
    
    return result;
}
