#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "bscl_url.h"


 bscl_url_t * bscl_url_new_from_text(const char *text)
{
    int text_len = strlen(text);
    int i;
    int path_len = -1;

    for (i = 0; i < text_len; ++i) {
        if (text[i] == '?') {
            if (path_len < 0)
                path_len = i;
            else {
                return NULL;
            }
        }
    }

    if (path_len <= 0) {
        return NULL;
    }

     bscl_url_t *url = ( bscl_url_t *)malloc(sizeof( bscl_url_t));
    if (!url)
        return NULL;
    memcpy(url->path, text, path_len);
    url->path[path_len] = 0;
    url->query_num = 0;
    const char *key = text + path_len + 1;
    const char *value = NULL;

    for (i = path_len + 1; i < text_len; ++i) {
        if (text[i] == '=') {
            if (key) {
                value = text + i+1;
            }
        } else if (text[i] == '&') {
            if (key && value) {
                url->query[url->query_num] = ( bscl_url_query_t *)malloc(sizeof( bscl_url_query_t));
                if (!url->query[url->query_num]) {
                    return NULL;
                } else {
                    memcpy(url->query[url->query_num]->key, key,
                           value - key - 1);
                    url->query[url->query_num]->key[value - key - 1] = 0;
                    memcpy(url->query[url->query_num]->value, value,
                           text + i - value);
                    url->query[url->query_num]->value[text + i - value] = 0;
                    url->query_num += 1;
                    key = text + i + 1;
                }
            } else {
                return NULL;
            }
        }
    }
    if (key && value) {
        url->query[url->query_num] = ( bscl_url_query_t *)malloc(sizeof( bscl_url_query_t));
        if (!url->query[url->query_num]) {
            return NULL;
        } else {
            memcpy(url->query[url->query_num]->key, key, value - key - 1);
            url->query[url->query_num]->key[value - key - 1] = 0;
            memcpy(url->query[url->query_num]->value, value, text + i - value);
            url->query[url->query_num]->value[text + i - value] = 0;
            url->query_num += 1;
            key = text + i + 1;
        }
    }
    return url;
}
