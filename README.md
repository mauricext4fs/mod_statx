mod_statx
=========

Redis backed apache module for direct access without high level programming language (direct access with HTTP request). Perfect for very high traffic statistic requirements.

Currently handle up to 73 404 request a minute for INCR.
PHP + redis (phpredis extension) : 52 572
PHP + fsock directly: 53 028


This is an experimental module and support only "INCR" command on a localhost redis server running
on 6379 (which is default).

To make it work you would need to do the following: 

Compile it with: /Users/mcourtois/apache/bin/apxs -i -a -c mod_statx.c hiredis/hiredis.c hiredis/net.c hiredis/sds.c

Then if everything went good add the following line to httpd.conf:

LoadModule statx_module       modules/mod_statx.so

and an handler like: 

AddHandler statx-handler .statx

Hit (your web server and whatever name of file.statx?incr=keytoincrementinredis) : http://127.0.0.1:8080/vlad.statx?incr=new_key

Enjoy!