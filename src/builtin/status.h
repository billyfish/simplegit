#ifndef GIT_STATUS_H
#define GIT_STATUS_H

#include <git2.h>

int cmd_status(git_repository *repo, int argc, char **argv);

int lg2_status(git_repository *repo, int argc, char *argv []);

int get_modified_and_deleted_files (git_repository *repo, git_strarray *add_command, git_strarray *remove_command);

#endif
