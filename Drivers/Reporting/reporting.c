#include "reporting.h"

#include <stddef.h>
#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "lwip/altcp.h"
#include "lwip/apps/http_client.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"

#define REPORT_SERVER_IP   "192.168.1.50"  // Replace with your laptop's IP
#define REPORT_SERVER_PORT 8080

static bool report_pending;
static char report_path[128];

static err_t discard_http_body(void *arg, struct altcp_pcb *connection,
                               struct pbuf *body, err_t err)
{
    (void)arg;

    if (body != NULL) {
        altcp_recved(connection, body->tot_len);
        pbuf_free(body);
    }

    return err;
}

static void report_finished(void *arg, httpc_result_t result,
                            u32_t received_bytes, u32_t http_status,
                            err_t err)
{
    (void)arg;
    (void)received_bytes;

    report_pending = false;

    printf("Flask reply: HTTP %lu, lwIP result %d, error %d\n",
           (unsigned long)http_status, (int)result, (int)err);
}

static const httpc_connection_t report_settings = {
    .result_fn = report_finished
};

void report(bool door_closed, bool ambient_ok, bool password_ok)
{
    ip_addr_t server;

    if (!ipaddr_aton(REPORT_SERVER_IP, &server)) {
        printf("Invalid report server IP\n");
        return;
    }

    cyw43_arch_lwip_begin();

    if (report_pending) {
        cyw43_arch_lwip_end();
        return;
    }

    int length = snprintf(report_path, sizeof report_path,
                          "/log/group3/%d.0/%d.0/%d.0/1",
                          door_closed ? 1 : 0,
                          ambient_ok ? 1 : 0,
                          password_ok ? 1 : 0);

    if (length < 0 || (size_t)length >= sizeof report_path) {
        cyw43_arch_lwip_end();
        printf("Could not format HTTP report path\n");
        return;
    }

    report_pending = true;
    err_t err = httpc_get_file(&server, REPORT_SERVER_PORT, report_path,
                               &report_settings, discard_http_body,
                               NULL, NULL);

    if (err != ERR_OK) {
        report_pending = false;
    }

    cyw43_arch_lwip_end();

    if (err != ERR_OK) {
        printf("Could not start HTTP report: %d\n", (int)err);
    }
}