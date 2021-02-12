## Introduction

Simplegit uses libgit2 for its git functionality and the version in the repo is quite
old now. This is an attempt to use a more current version and allows you to build 
simplegit able to test against different libgit2 instances, newlib or clib2 and whether 
to use a statically-linked OpenSSL or use the AmiSSL runtime library.



## Set up

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

For example, to build the AmiSSL version using clib2, using a libgit2 directory at the same
level as this directory, the make command line would be:

```
make -f makefile.aos4 C_RUNTIME=clib2 LIBGIT2_DIR=../libgit2/ USE_AMISSL=1 SGIT_NAME=sgit_amiga sgit
```

Currently the clib2 version appears to provide more unix compatibility so I tend to build using that.

## libgit2 

You can get this from 
[https://github.com/billyfish/libgit2](https://github.com/billyfish/libgit2). You can check out 
this repo to anyhere you wish except for libgit2 within this simplegit directory as that is 
where the original one used by simplegit will be placed. My fork of Libgit2 has the changes for 
the Amiga version in a separate branch so you will need to switch to the *amigaos4_port* branch 
to get it. Once you have checked out this repo, switch to this branch by running


```
git checkout amigaos4_port
```

