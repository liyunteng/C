/*
 * test.c - test
 *
 * Date   : 2020/04/15
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <pthread.h>

#include <agent.h>
#include <gio/gnetworking.h>

static GMainLoop *gloop;
static guint stream_id;
static GIOChannel *io_stdin;

static gboolean g_controlling_mode = FALSE;
typedef enum {
    MODE_STUN_UDP = 0,
    MODE_STUN_TCP = 1,
    MODE_TURN_UDP = 2,
    MODE_TURN_TCP = 3,
    MODE_ALL = 4,
} mode_e;
static mode_e g_mode = MODE_STUN_UDP;

static const gchar *candidate_type_name[] = {
    "host",
    "srflx",
    "prflx",
    "relay"
};

static const gchar *state_name[] =
{
    "disconnected",
    "gathering",
    "connecting",
    "connected",
    "ready",
    "failed"
};

struct user_info {
    gchar *username;
    gchar *passwd;
};

struct recv_args {
    NiceAgent *agent;
    guint stream_id;
    guint component_id;
};

static int
print_local_data(NiceAgent *agent, guint _stream_id, guint componet_id)
{
    int result = -1;
    gchar *local_ufrag = NULL;
    gchar *local_passwd = NULL;
    gchar ipaddr[INET6_ADDRSTRLEN];
    GSList *cands = NULL, *item;

    if (!nice_agent_get_local_credentials(agent, _stream_id, &local_ufrag, &local_passwd)) {
        goto end;
    }

    cands = nice_agent_get_local_candidates(agent, _stream_id, componet_id);
    if (cands == NULL) {
        goto end;
    }
    /* printf("stream_id: %u componet_id: %u\n", _stream_id, componet_id); */

    printf("%s %s ", local_ufrag, local_passwd);

    for (item = cands; item; item = item->next) {
        NiceCandidate *c = (NiceCandidate *) item->data;
#if 0
        assert(c->type == NICE_CANDIDATE_TYPE_HOST);
        assert (c->transport == NICE_CANDIDATE_TRANSPORT_TCP_ACTIVE ||
                c->transport == NICE_CANDIDATE_TRANSPORT_TCP_PASSIVE);
#endif

        nice_address_to_string(&c->addr, ipaddr);

        const char *p;
        switch (c->transport) {
        case NICE_CANDIDATE_TRANSPORT_UDP:
            p = "udp";
            break;
        case NICE_CANDIDATE_TRANSPORT_TCP_ACTIVE:
            p = "tcp_active";
            break;
        case NICE_CANDIDATE_TRANSPORT_TCP_PASSIVE:
            p = "tcp_passive";
            break;
        case NICE_CANDIDATE_TRANSPORT_TCP_SO:
            p = "tcp_so";
            break;
        default:
            printf("!! invalid transport !!\n");
            break;
        }
        printf(" %s,%u,%s,%u,%s,%s",
               c->foundation,
               c->priority,
               ipaddr,
               nice_address_get_port(&c->addr),
               candidate_type_name[c->type],
               p
            );
    }
    printf("\n");
    result = 0;

end:
    if (local_ufrag)
        g_free(local_ufrag);
    if (local_passwd)
        g_free(local_passwd);
    if (cands)
        g_slist_free_full(cands, (GDestroyNotify)&nice_candidate_free);
    return result;
}

static NiceCandidate *
parse_candidate(NiceAgent *agent, char *scand, guint _stream_id, const gchar *username, const gchar *passwd)
{
    NiceCandidate *cand = NULL;
    NiceCandidateType ntype;
    gchar **tokens = NULL;
    guint i;
    tokens = g_strsplit(scand, ",", 6);
    for (i = 0; tokens[i]; i++);
    if (i != 6)
        goto end;

    for (i = 0; i < G_N_ELEMENTS(candidate_type_name); i++) {
        if (strcmp(tokens[4], candidate_type_name[i]) == 0) {
            ntype = i;
            break;
        }
    }

    if (i == G_N_ELEMENTS(candidate_type_name))
        goto end;

    cand = nice_candidate_new(ntype);
    cand->component_id = 1;
    cand->stream_id = _stream_id;
    cand->username = g_malloc(32);
    cand->password = g_malloc(32);
    strncpy(cand->username, username, strlen(username));
    strncpy(cand->password, passwd, strlen(passwd));
    strncpy(cand->foundation, tokens[0], NICE_CANDIDATE_MAX_FOUNDATION);
    cand->foundation[NICE_CANDIDATE_MAX_FOUNDATION - 1] = 0;
    cand->priority = atoi(tokens[1]);

    if (!nice_address_set_from_string(&cand->addr, tokens[2])) {
        printf("failed to parse addr: %s\n", tokens[2]);
        nice_candidate_free(cand);
        cand = NULL;
        goto end;
    }
    nice_address_set_port(&cand->addr, atoi(tokens[3]));

    for (i = 0; i < sizeof(candidate_type_name)/sizeof(candidate_type_name[0]); i ++) {
        if (!strcmp(tokens[4], candidate_type_name[i])) {
            cand->type = i;
        }
    }
    if (!strcmp(tokens[5], "udp")) {
        cand->transport = NICE_CANDIDATE_TRANSPORT_UDP;
    } else {
        if (!strcmp(tokens[5], "tcp_active")) {
            cand->transport = NICE_CANDIDATE_TRANSPORT_TCP_ACTIVE;
        } else if (!strcmp(tokens[5], "tcp_passive")){
            cand->transport = NICE_CANDIDATE_TRANSPORT_TCP_PASSIVE;
        } else if (!strcmp(tokens[5], "tcp_so")) {
            cand->transport = NICE_CANDIDATE_TRANSPORT_TCP_SO;
        }

    }

end:
    g_strfreev(tokens);
    return cand;
}

static int
parse_remote_data(NiceAgent *agent, guint _stream_id, guint component_id, char *line)
{
    GSList *cands = NULL;
    gchar **line_argv = NULL;
    const gchar *ufrag = NULL;
    const gchar *passwd = NULL;
    int result = -1;
    int i;

    line_argv = g_strsplit_set(line, " \t\n", 0);
    for (i = 0; line_argv && line_argv[i]; i++) {
        if (strlen(line_argv[i]) == 0)
            continue;

        if (!ufrag) {
            ufrag = line_argv[i];
        } else if (!passwd) {
            passwd = line_argv[i];
        } else {
            NiceCandidate *c = parse_candidate(agent, line_argv[i], _stream_id, ufrag, passwd);

            if (c == NULL) {
                printf("failed to parse candidate: %s\n", line_argv[i]);
                goto end;
            }
            printf("candidate: type: %d transport: %d stream_id: %d component_id: %d priority: %d\n",
                   c->type, c->transport, c->stream_id, c->component_id, c->priority);
            cands = g_slist_append(cands, c);
        }
    }

    if (ufrag == NULL || passwd == NULL || cands == NULL) {
        printf("line must have at least ufrag, passwd, and one candidate\n");
        goto end;
    }

    if (nice_agent_set_remote_credentials(agent, _stream_id, ufrag, passwd) <= 0) {
        printf("failed to set remote credentials\n");
        goto end;
    }


    if (nice_agent_set_remote_candidates(agent, _stream_id, component_id, cands) <= 0) {
        printf("failed to set remote candidates\n");
        goto end;
    }
    result = 0;
end:
    if (line_argv != NULL) {
        g_strfreev(line_argv);
    }
    if (cands != NULL) {
        g_slist_free_full(cands, (GDestroyNotify)&nice_candidate_free);
    }
    return result;
}

static gboolean
stdin_remote_info_cb(GIOChannel *source, GIOCondition cond, gpointer data)
{
    NiceAgent *agent = data;
    gchar *line = NULL;
    int rval;
    gboolean rc = TRUE;

    if (g_io_channel_read_line(source, &line, NULL, NULL, NULL) ==
        G_IO_STATUS_NORMAL) {

        rval = parse_remote_data(agent, stream_id, 1, line);
        if (rval == 0) {
            rc = FALSE;
            printf("waiting for state READY or FAILED signal ...\n");
        } else {
            printf("ERROR: failed to parse remote data");
            printf("Enter remote data: ");
            printf("> ");
            fflush(stdout);
        }
        g_free(line);
    }
    return rc;
}

static void
cb_candidate_gathering_done(NiceAgent *agent, guint _stream_id, gpointer data)
{

    printf("SIGNAL: candidate gathering done\n");
    print_local_data(agent, _stream_id, 1);

    printf("Enter remote data: ");
    g_io_add_watch(io_stdin, G_IO_IN, stdin_remote_info_cb, agent);
    printf("> ");
    fflush(stdout);
}

static void
cb_new_selected_pair(NiceAgent *agent, guint _stream_id, guint component_id,
                     gchar *lfoundation, gchar *rfoundation, gpointer data)
{
    printf("SIGNAL: selected pair %s %s\n", lfoundation, rfoundation);
}

static void
send_av(NiceAgent *agent)
{
    gchar buf[4096];
    int nread, nsend, total_send;
    FILE *fp = fopen("abc.mp4", "rb");
    assert(fp != NULL);
    while ((nread = fread(buf, 1, sizeof(buf),fp)) > 0) {
        total_send = 0;
        while (total_send < nread) {
            nsend = nice_agent_send(agent, stream_id, 1, (nread - total_send), buf+total_send);
            printf("send %d data, nread: %d , total_send: %d\n", nsend, nread, total_send);
            if (nsend < 0) {
                continue;
            }
            total_send += nsend;
            usleep(1000 * 10);
        }
    }
    printf("\nav send done\n");
    fclose(fp);
}

static gboolean
stdin_send_data_cb(GIOChannel *source, GIOCondition cond, gpointer data)
{
    NiceAgent *agent = data;
    gchar *line = NULL;
    gint rc;
    if (g_io_channel_read_line(source, &line, NULL, NULL, NULL) ==
        G_IO_STATUS_NORMAL) {
        if (!strcmp(line, "send\n")) {
            send_av(agent);
        } else {
            rc = nice_agent_send(agent, stream_id, 1, strlen(line), line);
            if (rc <= 0) {
                printf("component state: %s\n", nice_component_state_to_string(nice_agent_get_component_state(agent, stream_id, 1)));
                printf("send failed: %d\n", rc);
            }
            g_free(line);
            printf("> ");
            fflush(stdout);
        }
    } else {
        printf("read line failed\n");
        nice_agent_send(agent, stream_id, 1, 1, "\0");
        g_main_loop_quit(gloop);
    }
    return TRUE;
}


static void
cb_component_state_changed(NiceAgent *agent, guint _stream_id,
                           guint component_id, guint state,
                           gpointer data)
{
    printf("SIGNAL: state changed %d %d %s[%d]\n",
           _stream_id, component_id, state_name[state], state);
    fflush(stdout);

    if (state == NICE_COMPONENT_STATE_CONNECTED) {
        NiceCandidate *local, *remote;

        if (nice_agent_get_selected_pair(agent, _stream_id, component_id,
                                         &local, &remote)) {
            gchar ipaddr[INET6_ADDRSTRLEN];

            nice_address_to_string(&local->addr, ipaddr);
            printf("Negotitaion complete: ([%s]:%d,", ipaddr, nice_address_get_port(&local->addr));
            nice_address_to_string(&remote->addr, ipaddr);
            printf(" [%s]:%d)\n", ipaddr, nice_address_get_port(&remote->addr));
            printf("locale type: %d transport: %d turn: %p\n", local->type, local->transport, local->turn);
            printf("remote type: %d transport: %d turn: %p\n", remote->type, remote->transport, remote->turn);

        }

        printf("Send lines to remote (Ctrld-D to quit):\n");
        g_io_add_watch(io_stdin, G_IO_IN, stdin_send_data_cb, agent);
        printf("> ");
        fflush(stdout);

    } else if (state == NICE_COMPONENT_STATE_FAILED){
        printf("!!!FAILED!!!\n");
        g_main_loop_quit(gloop);
    }
}
#if 0
static gboolean thread_running = FALSE;
static void*
recv_thread(void *args)
{
    struct recv_args *a = (struct recv_args *)args;
    gssize nread;
    gssize nwrite, total_write = 0;
    guint8 buf[4096];

    FILE *fp = fopen("recv.data", "wb");
    assert(fp != NULL);

    while (thread_running) {
        nread = nice_agent_recv(a->agent,
                                a->stream_id,
                                a->component_id,
                                buf,
                                sizeof(buf),
                                NULL,
                                NULL);
        if (nread > 0) {
            nwrite = 0;
            total_write = 0;
            while (total_write < nread) {
                nwrite = fwrite(buf + total_write, 1, (nread - total_write), fp);
                printf("recv %ld data, total_write: %ld  nwrite: %ld\n", nread, total_write, nwrite);
                fflush(stdout);
                if (nwrite < 0) {
                    continue;
                }
                total_write += nwrite;
                fflush(fp);
            }
        } else {
            printf("recv failed: %ld\n", nread);
            fflush(stdout);
            break;
        }
    }
    return (void *)0;
}
#endif

static void
cb_new_candidate(NiceAgent *agent, guint _stream_id, guint component_id,
                 gchar *foundation, gpointer data)
{
    printf("SIGNAL: new_candidate %d %d %s\n", _stream_id, component_id, foundation);
}

static void
cb_new_remote_candidate(NiceAgent *agent, guint _stream_id, guint component_id,
                        gchar *foundation, gpointer data)
{
    printf("SIGNAL: new_remote_candidate\n");
}
static void
cb_initial_binding_request_received(NiceAgent *agent, guint _stream_id, gpointer data)
{
    printf("SIGNAL: initial_binding_request_received\n");
}

static FILE *recv_fp = NULL;
static void cb_nice_recv (NiceAgent *agent, guint _stream_id, guint component_id, guint len, gchar *buf, gpointer user_data)
{
    /* printf("cb_nice_recv\n"); */

  /* XXX: dear compiler, these are for you: */
    if (recv_fp == NULL) {
        recv_fp = fopen("recv.data", "wb");
    }
  assert(recv_fp != NULL);

  gssize nwrite = 0, written = 0;
  while (written < len) {
      nwrite = fwrite(buf + written, 1, (len - written), recv_fp);
      if (nwrite < 0) {
          continue;
      }
      written += nwrite;
      fflush(recv_fp);
      printf("recv %u data, written: %ld  nwrite: %ld\n", len, written, nwrite);
      fflush(stdout);
  }
}

int main(int argc, char *argv[])
{
    NiceAgent *agent;
    gchar *stun_addr = "118.89.227.65";
    guint stun_port = 3478;
    gint rc = -1;
    /* struct user_info local; */

    if (argc < 2) {
        printf("%s: 0|1 [stun-udp/stun-tcp/turn-udp/turn-tcp/all]\n", argv[0]);
        return -1;
    }
    g_controlling_mode = argv[1][0] - '0';
    if (g_controlling_mode != 0 && g_controlling_mode != 1) {
        printf("invalid controlling\n");
        return -1;
    }

    if (argc == 3) {
        if (!strcmp(argv[2], "stun-udp")) {
            g_mode = MODE_STUN_UDP;
        } else if (!strcmp(argv[2], "stun-tcp")) {
            g_mode = MODE_STUN_TCP;
        } else if (!strcmp(argv[2], "turn-udp")) {
            g_mode = MODE_TURN_UDP;
        } else if (!strcmp(argv[2], "turn-tcp")) {
            g_mode = MODE_TURN_TCP;
        } else if (!strcmp(argv[2], "all")){
            g_mode = MODE_ALL;
        } else {
            printf("invalid mode\n");
            return -1;
        }
    }

    nice_debug_enable(TRUE);

    g_networking_init();

    gloop = g_main_loop_new(NULL, FALSE);
    io_stdin = g_io_channel_unix_new(fileno(stdin));

    agent = nice_agent_new(g_main_loop_get_context(gloop),
                           NICE_COMPATIBILITY_RFC5245);
    assert(agent != NULL);

    g_object_set(agent, "stun-server", stun_addr, NULL);
    g_object_set(agent, "stun-server-port", stun_port, NULL);
    /* g_object_set(agent, "keepalive-conncheck", tcp, NULL); */
    g_object_set(agent, "controlling-mode", g_controlling_mode, NULL);

    if (g_mode == MODE_STUN_TCP || g_mode == MODE_TURN_TCP) {
        /* g_object_set(agent, "ice-udp", FALSE,  "ice-tcp", TRUE, "force-relay", FALSE, NULL); */
        g_object_set(agent, "ice-udp", FALSE, NULL);
        g_object_set(agent, "ice-tcp", TRUE, NULL);
    } else if (g_mode == MODE_ALL) {
        g_object_set(agent, "ice-udp", TRUE, NULL);
        g_object_set(agent, "ice-tcp", TRUE, NULL);
    }

    if (g_mode == MODE_TURN_UDP || g_mode == MODE_TURN_TCP) {
        g_object_set(agent, "force-relay", TRUE, NULL);
    }
    g_object_set(agent, "upnp", FALSE, NULL);

    g_signal_connect(agent, "candidate-gathering-done",
                     G_CALLBACK(cb_candidate_gathering_done), NULL);
    g_signal_connect(agent, "new-selected-pair",
                     G_CALLBACK(cb_new_selected_pair), NULL);
    g_signal_connect(agent, "component-state-changed",
                     G_CALLBACK(cb_component_state_changed), NULL);
    g_signal_connect(agent, "new-candidate",
                     G_CALLBACK(cb_new_candidate), NULL);
    g_signal_connect(agent, "initial-binding-request-received",
                     G_CALLBACK(cb_initial_binding_request_received), NULL);
    /* g_signal_connect(agent, "reliable-transport-writable", NULL, NULL); */
    g_signal_connect(agent, "new-remote-candidate",
                     G_CALLBACK(cb_new_remote_candidate), NULL);



#if 0
    NiceAddress addr;
    assert (nice_address_set_from_string(&addr, "192.168.31.44") > 0);
    /* assert(rc != 0); */
    gchar str[NICE_ADDRESS_STRING_LEN];
    nice_address_to_string(&addr, str);
    printf("%s\n", str);

    rc = nice_agent_add_local_address(agent, &addr);
    assert(rc != 0);

#endif



    stream_id = nice_agent_add_stream(agent, 1);
    assert(stream_id != 0);

#if 0
    rc = nice_agent_set_local_credentials(agent, stream_id, local.username, local.passwd);
    assert(rc == TRUE);
#endif

    rc = nice_agent_set_stream_name(agent, stream_id, "video");
    assert(rc != 0);
    printf("stream sdp:\n%s\n", nice_agent_generate_local_stream_sdp(agent, stream_id, TRUE));

    if (g_mode == MODE_TURN_UDP || g_mode == MODE_TURN_TCP) {
        rc = nice_agent_set_relay_info(agent,
                                       stream_id,
                                       1,
                                       stun_addr,
                                       stun_port,
                                       "test",
                                       "123456",
                                       NICE_RELAY_TYPE_TURN_TCP);
        assert(rc == TRUE);
    }

#if 0
    GSList *cans = NULL, *i;
    NiceCandidate *cand = NULL;
    cands = nice_agent_get_local_candidates(agent, stream_id, 1);
    cand = cands->data;
    assert(cand->type == NICE_CANDIDATE_TYPE_HOST);
    assert(cand->transport == NICE_CANDIDATE_TRANSPORT_TCP_ACTIVE ||
           cand->transport == NICE_CANDIDATE_TRANSPORT_TCP_PASSIVE);
#endif


    nice_agent_attach_recv(agent, stream_id, 1,
                           g_main_loop_get_context(gloop),
                           cb_nice_recv, NULL);

#if 0
    pthread_t tid;

    struct recv_args args;
    args.agent = agent;
    args.stream_id = stream_id;
    args.component_id = 1;
    thread_running = TRUE;
    if (pthread_create(&tid, NULL, recv_thread, &args) != 0) {
        printf("pthread create failed\n");
        return -1;
    }
#endif

    rc = nice_agent_gather_candidates(agent, stream_id);
    assert (rc != 0);

    g_main_loop_run(gloop);

    g_main_loop_unref(gloop);
    g_object_unref(agent);
    g_io_channel_unref(io_stdin);

    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o test test.c -g `pkg-config --cflags --libs nice` -lpthread" */
/* End: */
