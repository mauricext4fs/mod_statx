mod_statx
=========

Redis backed apache module for direct access without high level programming language (direct access with HTTP request). Perfect for very high traffic statistic requirements.

Currently handle up to 94 926 request a minute for INCR with unix socket.
Currently handle up to 73 404 request a minute for INCR with network socket.
PHP + redis (phpredis extension) : 52 572
PHP + fsock directly: 53 028


This is an experimental module and support only "INCR" command on a localhost redis server running
on /tmp/redis.sock.

To make it work you would need to do the following: 

1. Download the latest distribution of hiredis found at: https://github.com/redis/hiredis
2. Unpack it under hiredis
3. Compile mod_statx with: apxs -i -a -c mod_statx.c hiredis/hiredis.c hiredis/net.c
   hiredis/sds.c
4. Then if everything went good add the following line to httpd.conf:


LoadModule statx_module       modules/mod_statx.so

5. Then set a location directive for activating the handler

<code>
<LocationMatch "/statxincr">
    SetHandler statx-handler
</LocationMatch>
</code>
6. Graceful your apache.
7. Hit (your web server and whatever name of statxincr?incr=keytoincrementinredis) : http://127.0.0.1:8080/statxincr?incr=new_key

Enjoy!
