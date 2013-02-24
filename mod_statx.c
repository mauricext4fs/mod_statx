/* Apr */
/* Include the required headers from httpd */
#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_config.h"
#include "http_request.h"
#include "http_log.h"
/* Include the required header from hiredis */
#include "hiredis/hiredis.h"

/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static int statx_handler(request_rec *r);

/* Define our module as an entity and assign a function for registering hooks  */

module AP_MODULE_DECLARE_DATA   statx_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,            // Per-directory configuration handler
    NULL,            // Merge handler for per-directory configurations
    NULL,            // Per-server configuration handler
    NULL,            // Merge handler for per-server configurations
    NULL,            // Any directives we may have for httpd
    register_hooks   // Our hook registering function
};


/* register_hooks: Adds a hook to the httpd process */
static void register_hooks(apr_pool_t *pool) 
{

    /* Hook the request handler */
    ap_hook_handler(statx_handler, NULL, NULL, APR_HOOK_LAST);
}

/* The handler function for our module.
 * This is where all the fun happens!
 */

static int statx_handler(request_rec *r)
{
    redisContext *redisContext;
    redisReply *reply;
    apr_table_t *GET;
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds


    /* First off, we need to check if this is a call for the "statx" handler.
     * If it is, we accept it and do our things, it not, we simply return DECLINED,
     * and Apache will try somewhere else.
     */
    if (!r->handler || strcmp(r->handler, "statx-handler")) return (DECLINED);

    //ap_set_content_type(r, "text/html");
    //ap_rputs("<!DOCTYPE html>\n", r);
    //ap_rputs("<html>\n", r);
    //ap_rputs("<head>\n", r);
    //ap_rputs("<title>Connecting to redis...</title>\n", r);
    //ap_rputs("</head>\n", r);
    //ap_rputs("<body>\n", r);
    //ap_rputs("Connecting to redis...<br>\n", r);

    // Getting a redis context, if we fall here we return directly (for now)
    //redisContext = redisConnectWithTimeout((char*)"127.0.0.1", 6379, timeout);
    redisContext = redisConnectUnix((char*)"/tmp/redis.sock");
    if (redisContext == NULL || redisContext->err) {
        if (redisContext) {
            //ap_rputs("Connection error: ", r);
            //ap_rputs(redisContext->errstr, r);
            //ap_rputs("\n", r);
            redisFree(redisContext);
        } else {
            //ap_rputs("Connection error: can't allocate redis context\n", r);
        }
        ap_log_error_(APLOG_MARK, APLOG_ERR, 0, r->server,
                                    "Cannot connect to redis server");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // Testing if the server is answering to commands.
    reply = redisCommand(redisContext, "PING");
    if(reply == NULL) {
        ap_log_error_(APLOG_MARK, APLOG_ERR, 0, r->server,
                                    "Redis server not responding!");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    //ap_rprintf(r, "Redis ping answer: %s", reply->str);
    freeReplyObject(reply);

    /* If we were reached through a GET or a POST request, be happy, else sad. */
    if ( strcmp(r->method, "GET") ) {
        //ap_rputs("You did not use POST or GET, that makes us sad :(<br/>", r);
        return OK;
    }

    // Let's parse the GET string
    ap_args_to_table(r, &GET);

    /* Get the "incr" key from the query string, if any. */
    const char *incr = apr_table_get(GET, "incr");
    //ap_rprintf(r, "incr value: %s", incr);

    if(incr) {
        redisCommand(redisContext, "INCR %s", incr);
    }

    //ap_rputs("</body>\n", r);
    //ap_rputs("</html>\n", r);

    // Disconnect from redis server.
    if(redisContext) {
        redisFree(redisContext);
    }

    return OK;
}
