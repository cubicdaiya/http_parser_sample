#include <stdio.h>
#include <string.h>

#include <http_parser.h>

#define CRLF "\r\n"

#define HTTP_REQUEST_GET ""                                             \
    "GET /bokko HTTP/1.1" CRLF                                          \
    "Host: localhost:8000" CRLF                                         \
    "Accept: */*" CRLF                                                  \
    "Connection: keep-alive" CRLF CRLF

struct http_request {
    enum http_method method;
    char request_url[128];
    int complete;
};

static int http_parser_callback_message_begin (http_parser *parser) {
    struct http_request *request = parser->data;
    memset(request, 0, sizeof(struct http_request));
    return 0;
}

static int http_parser_callback_request_url (http_parser *parser, const char *p, size_t len) {
    struct http_request *request = parser->data;
    strncat(request->request_url, p, len);
    return 0;
}

static int http_parser_callback_header_field (http_parser *parser, const char *p, size_t len) {
    char buf[BUFSIZ];
    strncpy(buf, p, len);
    buf[len] = '\0';
    printf("================\n");
    printf("%s, %ld\n", buf, len);
    printf("================\n");
    return 0;
}

static int http_parser_callback_header_value (http_parser *parser, const char *p, size_t len) {
    char buf[BUFSIZ];
    strncpy(buf, p, len);
    buf[len] = '\0';
    printf("================\n");
    printf("%s, %ld\n", buf, len);
    printf("================\n");
    return 0;
}

static int http_parser_callback_headers_complete (http_parser *parser) {
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
    return 0;
}
