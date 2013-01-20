#include <stdio.h>
#include <string.h>

#include <http_parser.h>

#define CRLF "\r\n"

#define HTTP_REQUEST_GET ""                                             \
    "GET /bokko?q1=a&q2=b HTTP/1.1" CRLF                                \
    "Host: localhost" CRLF                                              \
    "Accept: */*" CRLF                                                  \
    "Connection: keep-alive" CRLF CRLF

struct http_request {
    enum http_method method;
    char url[128];
    int complete;
};

static void dump_url (const char *url, const struct http_parser_url *u);
static int http_parser_callback_message_begin (http_parser *parser);
static int http_parser_callback_request_url (http_parser *parser, const char *p, size_t len);
static int http_parser_callback_header_field (http_parser *parser, const char *p, size_t len);
static int http_parser_callback_header_value (http_parser *parser, const char *p, size_t len);
static int http_parser_callback_headers_complete (http_parser *parser);

static void dump_url (const char *url, const struct http_parser_url *u)
{
    char part[512];
    int i;

    if (u->field_set & (1 << UF_PATH)) {
        memcpy(part, url + u->field_data[UF_PATH].off, u->field_data[UF_PATH].len);
        part[u->field_data[UF_PATH].len] = '\0';
        printf("path:%s\n", part);
    }

    if (u->field_set & (1 << UF_QUERY)) {
        memcpy(part, url + u->field_data[UF_QUERY].off, u->field_data[UF_QUERY].len);
        part[u->field_data[UF_QUERY].len] = '\0';
        printf("query_string:%s\n", part);
    }
}

static int http_parser_callback_message_begin (http_parser *parser)
{
    struct http_request *request = parser->data;
    memset(request, 0, sizeof(struct http_request));
    return 0;
}

static int http_parser_callback_request_url (http_parser *parser, const char *p, size_t len)
{
    struct http_request *request = parser->data;
    strncat(request->url, p, len);
    printf("url:%s\n", request->url);
    return 0;
}

static int http_parser_callback_header_field (http_parser *parser, const char *p, size_t len)
{
    char buf[BUFSIZ];
    strncpy(buf, p, len);
    buf[len] = '\0';
    printf("================\n");
    printf("%s, %ld\n", buf, len);
    printf("================\n");
    return 0;
}

static int http_parser_callback_header_value (http_parser *parser, const char *p, size_t len)
{
    char buf[BUFSIZ];
    strncpy(buf, p, len);
    buf[len] = '\0';
    printf("================\n");
    printf("%s, %ld\n", buf, len);
    printf("================\n");
    return 0;
}

static int http_parser_callback_headers_complete (http_parser *parser)
{
    struct http_request *request = parser->data;
    request->method = parser->method;
    return 0;
}

int main (int argc, char *argv[]) {
    
    struct http_parser parser;
    struct http_parser_settings setting = {
        .on_message_begin    = http_parser_callback_message_begin,
        .on_url              = http_parser_callback_request_url,
        .on_header_field     = http_parser_callback_header_field,
        .on_header_value     = http_parser_callback_header_value,
        .on_headers_complete = http_parser_callback_headers_complete,
        .on_body             = NULL,
        .on_message_complete = NULL
    };
    struct http_request request;
    http_parser_init(&parser, HTTP_REQUEST);
    parser.data = &request;
    http_parser_execute(&parser, &setting, HTTP_REQUEST_GET, strlen(HTTP_REQUEST_GET));

    struct http_parser_url u;
    int len = strlen(request.url);
    int result = http_parser_parse_url(request.url, len, 0, &u);
    if (result != 0) {
        printf("Parse error: %d\n", result);
        return result;
    }
    printf("Parse ok, result: \n");
    dump_url(request.url, &u);

    return 0;
}
