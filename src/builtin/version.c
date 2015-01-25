#include "version.h"

#include <stdio.h>

#include <git2.h>
#include <openssl/crypto.h>

int cmd_version(git_repository *repo, int argc, char **argv)
{
	int major, minor, rev;

	git_libgit2_version(&major, &minor, &rev);
	printf("sgit version 0.%s\n", SIMPLEGIT_REVISION_STRING);
	printf("using libgit2 %d.%d.%d and %s\n", major, minor, rev, SSLeay_version(SSLEAY_VERSION));

	return EXIT_SUCCESS;
}
