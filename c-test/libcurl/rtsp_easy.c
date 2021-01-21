/*
 * rtsp_easy.c - rtsp_easy
 *
 * Date   : 2021/01/21
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "curl/curl.h"

#define RTP_PKT_CHANNEL(p)  ((int)((unsigned char)((p)[1])))
#define RTP_PKT_LENGTH(p)   ((((int)((unsigned char)((p)[2]))) << 8) | \
                              ((int)((unsigned char)((p)[3]))))


static int rtp_packet_count = 0;
static FILE *get_fp(char *prefix)
{
    char tmp[512];
    sprintf(tmp, "%s_%d.pkt", prefix, rtp_packet_count);
    return fopen(tmp, "wb");
}

static size_t rtp_write(void *ptr, size_t size, size_t nmemb, void *stream)
{
    char *data = (char *)ptr;
    int channel = RTP_PKT_CHANNEL(data);
    if (channel == 1 || channel == 3) {
        printf("@@@@@@@@rtcp channel\n");
        return size * nmemb;
    }

    int message_size = (int)(size * nmemb) - 4;
    int coded_size = RTP_PKT_LENGTH(data);
    size_t failure = (size * nmemb) ? 0 : 1;

    printf("RTP: message size %d, channel %d\n", message_size, channel);
    if (message_size != coded_size) {
        printf("RTP embedded size (%d) does not match the write size (%d).\n",
               coded_size, message_size);
        return failure;
    }

    data += 4;
    FILE *fp = get_fp("haha");
    fwrite(data, message_size, 1, fp);
    fclose(fp);
    rtp_packet_count++;
    printf("packet count is %d\n", rtp_packet_count);

    return size * nmemb;
}

/* build request url */
static char *suburl(const char *base)
{
    char *tmp = calloc(1, 512);
    strcpy(tmp, base);
    return tmp;
}

int test_rtsp_easy(char *URL, char *fname)
{
    FILE *protofile = NULL;
    protofile = fopen(fname, "wb");
    if (protofile == NULL) {
        printf("Couldn't open the protocol dump file\n");
        return -1;
    }

    CURL *curl;
    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, URL);

    int res;
    char *stream_uri = NULL;
    if ((stream_uri = suburl(URL)) == NULL) {
        res = -1;
        goto cleanup;
    }

    /* range request */
    /* curl_easy_setopt(curl, CURLOPT_RANGE, "100-200"); */
    curl_easy_setopt(curl, CURLOPT_RTSP_STREAM_URI, stream_uri);
    curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_OPTIONS);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "VLC");
    res = curl_easy_perform(curl);
    if (res)
        goto cleanup;

    curl_easy_setopt(curl, CURLOPT_RTSP_STREAM_URI, stream_uri);
    curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_DESCRIBE);
    res = curl_easy_perform(curl);
    if (res)
        goto cleanup;

    curl_easy_setopt(curl, CURLOPT_INTERLEAVEFUNCTION, rtp_write);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, protofile);

    char tmp[1024];
    strcpy(tmp, stream_uri);
    strcat(tmp, "/traceID=65536");
    curl_easy_setopt(curl,  CURLOPT_RTSP_STREAM_URI, tmp);
    free(stream_uri);
    curl_easy_setopt(curl, CURLOPT_RTSP_TRANSPORT, "RTP/AVP/TCP;interleaved=0-1");
    curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_SETUP);
    res = curl_easy_perform(curl);
    if (res)
        goto cleanup;

    char *rtsp_session_id;
    curl_easy_getinfo(curl, CURLINFO_RTSP_SESSION_ID, &rtsp_session_id);
    printf("###Got Session ID: [%s]\n", rtsp_session_id);
    rtsp_session_id = NULL;

    if ((stream_uri = suburl(URL)) == NULL) {
        res = -1;
        goto cleanup;
    }
    curl_easy_setopt(curl, CURLOPT_RTSP_STREAM_URI, stream_uri);
    free(stream_uri);
    curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_PLAY);
    res = curl_easy_perform(curl);
    if (res)
        goto cleanup;

    while (!res && rtp_packet_count < 10000) {
        printf("PLAY LOOP!\n");
        curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_RECEIVE);
        res = curl_easy_perform(curl);
    }

cleanup:
    if (stream_uri)
        free(stream_uri);

    if (protofile)
        fclose(protofile);

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: rtsp_easy <remote url> <local filename>\n");
        return -1;
    }

    return test_rtsp_easy(argv[1], argv[2]);
}

/* Local Variables: */
/* compile-command: "gcc rtsp_easy.c -o rtsp_easy -lcurl" */
/* End: */
