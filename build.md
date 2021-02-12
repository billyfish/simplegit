## Instructions

Simplegit uses libgit2 for its git functionality and 


To make the building of simplegit able to test against different libgit2 instances, 
newlib or clib2 and whether to use a statically-linked OpenSSL or use teh AmiSSL 
runtime library.

The switches for these are specified as arguments for make when building simplegit 
or lg2

The default 

 * C_RUNTIME: Setting this to *clib2* will link the tools using clib2 rather than newlib.
 * LIBGIT2_DIR: By default, the tools will use the direc```tory *libgit2* within this directory.
With this directive, you can specify a different libgit2 directory to use.
 * USE_AMISSL: Setting this to *1* will build libgit2 using AmiSSL for the SSL parts rather 
than a statically-linked OpenSSL.
 * SGIT_NAME: With this you can change the default name of the sinplegit tool. This is useful
when you want to differentiate between versions build with clib2 or newlib, AmiSSL or 
OpenSSL, *etc.*


My fork of Libgit2 has the change for the Amiga version in a separate branch so you will need to switch to the 
*amigaos4_port* branch to get it. You can get this from [https://github.com/billyfish/libgit2](https://github.com/billyfish/libgit2). Once you have checked out this repo, switch to the branch by running


```
git checkout amigaos4_port
```

