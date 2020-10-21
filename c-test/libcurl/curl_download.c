#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
// #include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#include "curl/curl.h"
// #define MLOGE(...)
#define MLOGE printf

typedef struct {
    CURL *curl;
    CURLcode result;
    struct curl_slist *slist;
    uint32_t data_size;
    uint8_t *data;
    FILE *fp;
} curl_ctx_t;

static size_t onRecvData(void *buffer, size_t size, size_t nmemb, void *priv)
{
    curl_ctx_t *ctx = (curl_ctx_t *)priv;
    size_t total_size = size * nmemb;
    /* MLOGE("here recv size: %ld\n", total_size); */
    if (ctx->fp) {
        fwrite(buffer, size, nmemb, ctx->fp);
    } else {
        ctx->data = realloc(ctx->data, ctx->data_size + total_size);
        if (ctx->data == NULL) {
            MLOGE("realloc failed\n");
            return total_size;
        }

        memcpy(ctx->data + ctx->data_size, buffer, total_size);
    }
    ctx->data_size += total_size;
    return total_size;
}

static size_t onProgress(void * priv, double dltotal, double dlnow, double ultotal, double ulnow)
{
    curl_ctx_t *ctx = (curl_ctx_t *)priv;
    (void)ctx;
    if (dltotal > 0 && ultotal > 0) {
        MLOGE("Progress: %d/%d %%%d  %d/%d %%%d\n",
              (int)dlnow, (int)dltotal, (int)(dlnow*100/dltotal),
              (int)ulnow, (int)ultotal, (int)(ulnow*100/ultotal));
    } else {
        MLOGE("Progress: %d/%d  %d/%d\n",
              (int)dlnow, (int)dltotal,
              (int)ulnow, (int)ultotal
            );
    }
    return 0;
}

static void curl_ctx_free(curl_ctx_t *ctx)
{
    if (ctx) {
        if (ctx->curl) {
            curl_easy_cleanup(ctx->curl);
            ctx->curl = NULL;
        }
        if (ctx->slist) {
            curl_slist_free_all(ctx->slist);
            ctx->slist = NULL;
        }

        if (ctx->data) {
            free(ctx->data);
            ctx->data = NULL;
        }
        if (ctx->fp) {
            fclose(ctx->fp);
            ctx->fp = NULL;
        }

        free(ctx);
        ctx = NULL;
    }
}

static curl_ctx_t *curl_ctx_get(const char *url, const char **headers, int header_size, const char *path)
{
    int i;
    curl_ctx_t *ctx = NULL;

    ctx = (curl_ctx_t *)malloc(sizeof(curl_ctx_t));
    if (ctx == NULL) {
        MLOGE("malloc failed\n");
        goto cleanUp;
    }

    memset(ctx, 0, sizeof(curl_ctx_t));
    if (path) {
        ctx->fp = fopen(path, "wb");
    }

    ctx->curl = curl_easy_init();
    if (ctx->curl == NULL) {
        MLOGE("curl_easy_init failed\n");
        goto cleanUp;
    }

    if (header_size > 0 && headers) {
        for (i = 0; i < header_size; i++) {
            ctx->slist = curl_slist_append(ctx->slist, headers[i]);
            if (ctx->slist == NULL) {
                MLOGE("curl_slist_append failed headers: %s\n", headers[i]);
                goto cleanUp;
            }
        }
        curl_easy_setopt(ctx->curl, CURLOPT_HTTPHEADER, ctx->slist);
    }

    curl_easy_setopt(ctx->curl, CURLOPT_URL, url);

    curl_easy_setopt(ctx->curl, CURLOPT_TIMEOUT, 90L);
    curl_easy_setopt(ctx->curl, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(ctx->curl, CURLOPT_LOW_SPEED_TIME, 10L);
    curl_easy_setopt(ctx->curl, CURLOPT_LOW_SPEED_LIMIT, 10L);

    curl_easy_setopt(ctx->curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(ctx->curl, CURLOPT_WRITEFUNCTION, onRecvData);
    curl_easy_setopt(ctx->curl, CURLOPT_WRITEDATA, ctx);
    curl_easy_setopt(ctx->curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(ctx->curl, CURLOPT_PROGRESSFUNCTION, onProgress);
    curl_easy_setopt(ctx->curl, CURLOPT_PROGRESSDATA, ctx);
    curl_easy_setopt(ctx->curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(ctx->curl, CURLOPT_SSL_VERIFYPEER, 0);

    curl_easy_setopt(ctx->curl, CURLOPT_VERBOSE, 1L);

    return ctx;

cleanUp:
    curl_ctx_free(ctx);
    return NULL;
}

int curl_PostRequest(const char *url, char *data, int data_size, const char**headers, int header_size, const char *path)
{
    curl_ctx_t *ctx = NULL;
    CURLcode ret = CURLE_OK;
    long httpCode = 0;

    ctx = curl_ctx_get(url, headers, header_size, path);
    if (ctx == NULL) {
        goto cleanUp;
    }

    curl_easy_setopt(ctx->curl, CURLOPT_POST, 1L);
    if (data_size > 0 && data) {
        curl_easy_setopt(ctx->curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(ctx->curl, CURLOPT_POSTFIELDSIZE, data_size);
    }

    ret = curl_easy_perform(ctx->curl);
    if (ret != CURLE_OK) {
        MLOGE("curl_easy_perform failed: %s\n", curl_easy_strerror(ret));
        goto cleanUp;
    }
    ret = curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (ret != CURLE_OK) {
        MLOGE("curl_easy_getinfo failed: %s\n", curl_easy_strerror(ret));
        goto cleanUp;
    }
    if (httpCode != 200) {
        MLOGE("httpCode: %ld\n", httpCode);
        goto cleanUp;
    }

    MLOGE("httpCode: %ld, data_size: %d\n", httpCode, ctx->data_size);
#if 1
    if (ctx->data) {
        MLOGE("%s\n", ctx->data);
    }
#endif

    curl_ctx_free(ctx);
    return 0;

cleanUp:
    curl_ctx_free(ctx);
    return -1;
}

int curl_GetRequest(const char *url, char* user_pwd, const char **headers, int header_size, const char *path)
{
    curl_ctx_t *ctx = NULL;
    CURLcode ret = CURLE_OK;
    long httpCode = 0;

    ctx = curl_ctx_get(url, headers, header_size, path);
    if (ctx == NULL) {
        goto cleanUp;
    }

    if (user_pwd) {
        curl_easy_setopt(ctx->curl, CURLOPT_USERPWD, user_pwd);
    }

    ret = curl_easy_perform(ctx->curl);
    if (ret != CURLE_OK) {
        MLOGE("curl_easy_perform failed: %s\n", curl_easy_strerror(ret));
        goto cleanUp;
    }
    ret = curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (ret != CURLE_OK) {
        MLOGE("curl_easy_getinfo failed: %s\n", curl_easy_strerror(ret));
        goto cleanUp;
    }
    if (httpCode != 200) {
        MLOGE("httpCode: %ld\n", httpCode);
        goto cleanUp;
    }

    MLOGE("httpCode: %ld, data_size: %d\n", httpCode, ctx->data_size);
#if 0
    if (ctx->data) {
        MLOGE("%s\n", ctx->data);
    }
#endif

    curl_ctx_free(ctx);
    return 0;

cleanUp:
    curl_ctx_free(ctx);
    return -1;
}

int curl_init()
{
    CURLcode ret = CURLE_FAILED_INIT;
    ret = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (ret == CURLE_OK) {
        return 0;
    } else {
        return -1;
    }
}

void curl_uninit()
{
    curl_global_cleanup();
}

int read_image(const char *path, char **buf, int *bufLen)
{
#include <sys/stat.h>
    struct stat st;
    FILE *fp = NULL;

    if (stat(path, &st) != 0) {
        printf("fstat %s failed\n", path);
        goto failed;
    }
    if (st.st_size <= 0) {
        printf("st.st_size = %lld\n", st.st_size);
        goto failed;
    }

    *buf = (char *)malloc(st.st_size);
    if (!*buf) {
        printf("malloc failed\n");
        goto failed;
    }
    *bufLen = st.st_size;
    printf("st.st_size: %lld\n", st.st_size);

    fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("fopen %s failed\n", path);
        goto failed;
    }

    if (fread(*buf, st.st_size, 1, fp) != 1) {
        printf("fread failed\n");
        goto failed;
    }


    if (fp) {
        fclose(fp);
        fp = NULL;
    }
    return 0;

failed:
    if (*buf) {
        free(*buf);
        *buf = NULL;
    }
    *bufLen = 0;

    if (fp) {
        fclose(fp);
        fp = NULL;
    }
    return -1;

}

void test_get(int isFile)
{
    int ret;
    const char *headers[10];
    headers[0] = "Content-Type: application/json";
    headers[1] = "User-Agent: GCC";

    if (isFile) {
        ret = curl_GetRequest("http://127.0.0.1:80/download/lyt/abc.jpg", NULL, headers, 2, "abc.jpg");
        assert(ret == 0);

    } else {
        // ret = curl_GetRequest("http://127.0.0.1:8080/CBX_V0.1.19.20200925211024_card.bin", NULL, headers, 2, "CBX_V0.1.19.20200925211024_card.bin");
        // ret = curl_GetRequest("http://www.baidu.com", NULL, headers, 2, NULL);
        ret = curl_GetRequest("http://127.0.0.1:80", NULL, headers, 2, NULL);
        assert(ret == 0);
    }
}

void test_post(int isFile)
{
    int ret;
    const char *headers[10];
    headers[0] = "Content-Type: application/json";
    headers[1] = "User-Agent: GCC";

    if (isFile) {
    // post file
    char *image = NULL;
    int bufLen = 0;
    ret = read_image("/Users/lyt/abc.jpg", &image, &bufLen);
    assert(ret == 0);
    printf("bufLen: %d\n", bufLen);
    ret = curl_PostRequest("http://127.0.0.1:80/upload/lyt/abc.jpg", image, bufLen, headers, 2, NULL);
    assert(ret == 0);

    free(image);
    } else {

    // post string
    char *data = "{\"key1\": \"val1\", \"key2\":\"val2\"}";
    ret = curl_PostRequest("http://127.0.0.1:80", data, strlen(data), headers, 2, NULL);
    assert(ret == 0);
    }

}

int main(void)
{
    int ret = 0;
    int i = 0;
    int count = 1;
    curl_init();

    const char *headers[10];
    headers[0] = "Content-Type: application/json";
    headers[1] = "User-Agent: test";
    for (i = 0; i < count; i ++) {
        test_post(0);
        test_get(0);
    }

    curl_uninit();
    return  ret;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o curl_download curl_download.c -g -lcurl" */
/* End: */
