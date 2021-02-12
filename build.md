## Instructions


To make the building of simplegit able to test against different libgit2 instances, 
newlib or clib2 and whether to use a statically-linked OpenSSL or use teh AmiSSL 
runtime library.

The switches for these are specified as arguments for make when building simplegit 
or lg2

The default 

 * C_RUNTIME: Setting this to *clib2* will link the tools using clib2 rather than newlib.
 * LIBGIT2_DIR: By default, the tools will use the directory *libgit2* within this directory.
With this directive, you can specify a different libgit2 directory to use.
 * USE_AMISSL: Setting this to *1* will build libgit2 using AmiSSL for the SSL parts rather 
than a statically-linked OpenSSL.
 * SGIT_NAME: With this you can change the default name of the sinplegit tool. This is useful
when you want to differentiate between versions build with clib2 or newlib, AmiSSL or 
OpenSSL, *etc.*



