// ngx_http_route_module.c

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_flag_t enable;
} ngx_http_route_loc_conf_t;

// 変数のインデックス
static ngx_int_t ngx_http_route_handler(ngx_http_request_t *r);
static char *ngx_http_route(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_route_init(ngx_conf_t *cf);
static void *ngx_http_route_create_loc_conf(ngx_conf_t *cf);

// モジュールの設定
static ngx_http_module_t ngx_http_route_module_ctx = {
    NULL,                          /* preconfiguration */
    ngx_http_route_init,                          /* postconfiguration */
    NULL,                          /* create main configuration */
    NULL,                          /* init main configuration */
    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */
    ngx_http_route_create_loc_conf,                          /* create location configuration */ // ここの戻り値がコマンドのconfに渡されるっぽい
    NULL                           /* merge location configuration */
};

// モジュールのコマンド
static ngx_command_t ngx_http_route_commands[] = {
    {
        ngx_string("route"),
        NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
        ngx_http_route,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};

static void *ngx_http_route_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_route_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_route_loc_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }

    return conf;
}

static ngx_int_t ngx_http_route_init(ngx_conf_t *cf)
{
    // ngx_http_core_loc_conf_t  *clcf;
    // ngx_http_handler_pt        *h;

    // clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    // h = ngx_array_push(&clcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
    // if (h == NULL) {
    //     return NGX_CONF_ERROR;
    // }

    // *h = ngx_http_route_handler; 
    // clcf->handler = ngx_http_route_handler;

    ngx_http_handler_pt             *h;
    ngx_http_core_main_conf_t       *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    // CONTENT PHASEだとproxy_passが先に実行されてしまうため、PRECONTENT PHASEに登録
    h = ngx_array_push(&cmcf->phases[NGX_HTTP_PRECONTENT_PHASE].handlers);

    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_route_handler;

    return NGX_OK;

}

static char *
ngx_http_route(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    // ngx_http_core_loc_conf_t  *clcf;
    // ngx_http_handler_pt        *h;

    // clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    // h = ngx_array_push(&clcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
    // if (h == NULL) {
    //     return NGX_CONF_ERROR;
    // }

    // *h = ngx_http_route_handler; 
    // clcf->handler = ngx_http_route_handler;

    ngx_http_route_loc_conf_t *rlcf = conf;
    // route directiveが呼ばれているlocationだけ有効にする
    rlcf->enable = 1;

    return NGX_CONF_OK;
}

// モジュール自体の定義
ngx_module_t ngx_http_route_module = {
    NGX_MODULE_V1,
    &ngx_http_route_module_ctx, /* module context */
    ngx_http_route_commands,    /* module directives */
    NGX_HTTP_MODULE,                 /* module type */
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NGX_MODULE_V1_PADDING
};

void
ngx_http_foo_init(ngx_http_request_t *r)
{
    //     off_t         len;
    // ngx_buf_t    *b;
    // ngx_int_t     rc;
    // ngx_chain_t  *in, out;
     if (r->request_body == NULL) {
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }

    // len = 0;

    // for (in = r->request_body->bufs; in; in = in->next) {
    //     len += ngx_buf_size(in->buf);
    // }

    // b = ngx_create_temp_buf(r->pool, NGX_OFF_T_LEN);
    // if (b == NULL) {
    //     ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
    //     return;
    // }

    // b->last = ngx_sprintf(b->pos, "%O", len);
    // b->last_buf = (r == r->main) ? 1 : 0;
    // b->last_in_chain = 1;

    // r->headers_out.status = NGX_HTTP_OK;
    // r->headers_out.content_length_n = b->last - b->pos;

    // rc = ngx_http_send_header(r);

    // if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
    //     ngx_http_finalize_request(r, rc);
    //     return;
    // }

    // out.buf = b;
    // out.next = NULL;

    // rc = ngx_http_output_filter(r, &out);
    
    ngx_buf_t *buf;
    ngx_chain_t *cl;
    ngx_str_t target_upstream;
    // ボディデータを取得
    cl = r->request_body->bufs;
    if (cl == NULL || cl->buf == NULL || cl->buf->pos == cl->buf->last) {
        ngx_http_finalize_request(r, NGX_HTTP_BAD_REQUEST);
    }

    buf = cl->buf;
    size_t len2 = buf->last - buf->pos;
    char *body = (char *)ngx_pnalloc(r->pool, len2 + 1);
    if (body == NULL) {
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
    }
    ngx_memcpy(body, buf->pos, len2);
    body[len2] = '\0';

    // 簡易的なJSON解析（実際にはlibjanssonなどを使うことを推奨）
    char *hoge_str = strstr(body, "\"hoge\":");
    if (hoge_str == NULL) {
        ngx_http_finalize_request(r, NGX_HTTP_BAD_REQUEST);
    }

    int hoge;
    if (sscanf(hoge_str, "\"hoge\": %d", &hoge) != 1) {
        ngx_http_finalize_request(r, NGX_HTTP_BAD_REQUEST);
    }

    // 偶奇を判定し、変数に設定
    if (hoge % 2 == 0) {
        // target_upstream = ngx_string("upstream1");
        target_upstream = (ngx_str_t)ngx_string("upstream1");
    } else {
        target_upstream = (ngx_str_t)ngx_string("upstream2");
    }

    // Nginx の変数に設定（$target_upstream）

    ngx_str_t name = (ngx_str_t)ngx_string("target_upstream");
    // ngx_hash_strlowだとcore dumpする(謎)    
    ngx_uint_t hash = ngx_hash_key(name.data, name.len);
    ngx_http_variable_value_t *var = ngx_http_get_variable(r, &name, hash);
    // ex = ngx_http_get_variable_index(, &name);
    if (var != NULL && !var->not_found) {
        ngx_log_error(NGX_LOG_NOTICE, r->connection->log, 0, "target_upstream(value): %s", var->data);
        var->len = target_upstream.len;
        var->data = target_upstream.data;
        var->valid = 1;
        var->no_cacheable = 0;
        var->not_found = 0;
        ngx_log_error(NGX_LOG_NOTICE, r->connection->log, 0, "target_upstream(value): %s", var->data);
    }

    // ngx_http_finalize_request(r, rc);
    return;
}

// ハンドラ関数の実装
static ngx_int_t ngx_http_route_handler(ngx_http_request_t *r)
{
    ngx_http_route_loc_conf_t *rlcf;
    rlcf = ngx_http_get_module_loc_conf(r, ngx_http_route_module);
    if (rlcf == NULL || !rlcf->enable) {
        return NGX_DECLINED;
    }

    ngx_int_t rc;
    // リクエストボディを読み取る
    ngx_log_error(NGX_LOG_NOTICE, r->connection->log, 0, "route handler");
    rc = ngx_http_read_client_request_body(r, ngx_http_foo_init);
    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "error dayo");
        return rc;
    }

    // proxy_pass を使用して upstream にリクエストを転送
    return NGX_DECLINED;
}
