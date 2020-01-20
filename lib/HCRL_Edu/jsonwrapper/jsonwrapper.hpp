#include "ArduinoJson.h"
/*
================================   BASIC USAGE  ====================================
    use operator [] to access key in parent object
    
    for JsonArray operator [] will work like normal array (return value from index)
    for JsonObject operator [] will return value or next object which is nested in


    This header use ArduinoJson version 6.13.0
====================================================================================
*/

/*

================================    Wrapper Function   ================================

*/

/* Insert all element in array as 1 list
   if keyname not exist , create new key and add data.
*/
template <typename T, typename D>
void array_add(D *doc, const char *keyname, T data, size_t size)
{
    if (doc->containsKey(keyname))
    {

        for (uint8_t i = 0; i < size; i++)
        {
            doc->operator[](keyname).add(data[i]);
        }
    }
    else
    {
        auto arr = doc->createNestedArray(keyname);
        for (uint8_t i = 0; i < size; i++)
        {
            arr.add(data[i]);
        }
    }
}

// Create object data from array with specific key.
template <typename H, typename D>
void object_add(H *host, D *data, const char *keyname, size_t keysize, char **key)
{

    if (host->containsKey(keyname))
    {
        for (size_t j = 0; j < keysize; j++)
        {
            host->operator[](keyname)[key[j]] = data[j];
        }
    }
    else
    {
        auto a = host->createNestedObject(keyname);
        for (size_t j = 0; j < keysize; j++)
        {
            a[key[j]] = data[j];
        }
    }
}

// Insert data array nested in host array (like 2 dimension array)
template <typename H, typename D>
void nested_array(H *host, D *data, size_t size, size_t size_per_data)
{
    for (uint8_t i = 0; i < size; i++)
    {
        auto arr = host->createNestedArray();
        for (uint8_t j = 0; j < size_per_data; j++)
        {
            arr.add(*((data + i * size_per_data) + j));
        }
    }
}

// insert object into array (array of object)
template <typename H, typename D>
void nested_object(H *host, D *data, size_t datasize, size_t keysize, char **key)
{
    for (size_t i = 0; i < datasize; i++)
    {
        auto a = host->createNestedObject();
        for (size_t j = 0; j < keysize; j++)
        {
            a[key[j]] = data[i][j];
        }
    }
}