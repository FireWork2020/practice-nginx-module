#define DDEBUG 1

#include "ddebug.h"
#include "ngx_http_print_module.h"
#include <ngx_config.h>

static char *ngx_http_print(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_print_handler(ngx_http_request_t *r);

static ngx_http_module_t ngx_http_print_module_ctx = {
        NULL,/* preconfiguration */
        NULL,/* postconfiguration */
        NULL,/* create main configuration */
        NULL,/* init main configuration */
        NULL,/* create server configuration */
        NULL,/* merge server configuration */
        NULL, /* create location configuration */
        NULL/* merge location configuration */
};

static ngx_command_t ngx_http_print_commands[] = {
        {
                ngx_string("print"),
                NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF
                | NGX_CONF_NOARGS, // 先用一个NO_ARGS, 不接收参数；
                ngx_http_print,
                NGX_HTTP_LOC_CONF_OFFSET,
                0,
                NULL
        },
        ngx_null_command
};


ngx_module_t ngx_http_print_module = {
        NGX_MODULE_V1,
        &ngx_http_print_module_ctx,
        ngx_http_print_commands,
        NGX_HTTP_MODULE,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_http_print_handler(ngx_http_request_t *r) {
    //这里是做什么？ 读取到配置项吗？
    ngx_http_print_loc_conf_t *config;
    config = ngx_http_get_module_loc_conf(r, ngx_http_print_module);

    /*设置 content-type */
    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *) "text/plain";

    ngx_buf_t *buf; //buf
    ngx_chain_t chain[1]; //初始化一个chain
    buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    chain[0].buf = buf;
    chain[0].next = NULL;
//    buf->pos = config->print.data;
//    buf->last = buf->pos + config->print.len;
    buf->pos = "hello";
    buf->last = buf->pos + 5;
    buf->memory = 1;
    buf->last_buf = 1;

    r->headers_out.status = NGX_HTTP_OK;
//    r->headers_out.content_length_n = config->print.len;
    r->headers_out.content_length_n = 5;

    ngx_int_t result_send_header = ngx_http_send_header(r);
    if (result_send_header == NGX_ERROR || result_send_header > NGX_OK || r->header_only) {
        return result_send_header;
    }

    int result = ngx_http_output_filter(r, &chain[0]);
    return result;
}

//set 函数；
static char *ngx_http_print(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    // 定义一个配置块看结构，这里的 clcf 看上去像 loc 级别的，
    //但实际上可以是main 或者 src 或者 loc 级别都行, http{} server{} 内部都有一个 _core_loc_conf_t
    ngx_http_core_loc_conf_t *clcf;

    // clcf 就是其所属的配置块
    // 我的理解：在全部配置中，找到插件所属的配置块（loc/server/http）
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_print_handler;
    return NGX_CONF_OK;
}