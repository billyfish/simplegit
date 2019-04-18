#include "status.h"

#include <stdlib.h>
#include <stdio.h>

#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "utils.h"

#include "git2/status.h"
#include "common.h"

#define GIT_STATUS_WT (GIT_STATUS_WT_MODIFIED|GIT_STATUS_WT_NEW|GIT_STATUS_WT_TYPECHANGE|GIT_STATUS_WT_DELETED)



static int make_commmand (git_status_list *status_list, const size_t status_list_size, const char *command, const size_t num_elements, const git_status_t status, git_strarray *array);




/**
 * The function that is invoked for each status entry. It prints out the
 * status to the standard output stream.
 *
 * @param path the path to the file for which this status is reported
 * @param status_flags the status of the file
 * @param payload an integer that defines which kind of path entries are
 *  interesting. It also holds holds the header print state.
 * @return
 */
static int status_cb(const char *path, unsigned int status_flags, void *payload)
{
	char *txt;
	int *mode = (int*)payload;

	if (!status_flags)
		return 0;

	txt = "unknown";

	if (status_flags & GIT_STATUS_INDEX_NEW) txt = "new file";
	else if (status_flags & GIT_STATUS_INDEX_MODIFIED) txt = "modified";
	else if (status_flags & GIT_STATUS_WT_MODIFIED) txt = "modified";
	else if (status_flags & GIT_STATUS_WT_NEW) txt = "new file";
	else if (status_flags & GIT_STATUS_WT_TYPECHANGE) txt = "typechange";
	else if (status_flags & GIT_STATUS_WT_DELETED) txt = "deleted";

	if (((*mode) & 1) == 0)
	{
		switch (*mode >> 1)
		{
		case	0:
				printf("# Changes to be committed:\n#\n");
				break;

		case	1:
				printf("#\n# Changes not staged for commit:\n#\n");
				break;

		case	2:
				printf("#\n# Untracked files:\n#\n");
				break;
		}
		*mode |= 1;
	}
	if (*mode >> 1 == 2)
	{
		if (status_flags & GIT_STATUS_WT_NEW)
			printf("#       %s\n",path);
	} else
	{
		if (!(status_flags & GIT_STATUS_WT_NEW))
			printf("#       %s: %s\n",txt,path);
	}

	return 0;
}

int cmd_status(git_repository *repo, int argc, char **argv)
{
	int rc = EXIT_FAILURE;
	int err = 0;
	int i, mode;

	git_reference *head_ref = NULL;
	git_reference *upstream_ref = NULL;

	git_status_options opt = GIT_STATUS_OPTIONS_INIT;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			fprintf(stderr,"No args supported at the moment\n");
			goto out;
		} else
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}
	}

	if ((git_repository_head(&head_ref,repo)) == GIT_OK)
	{
		const char *branch_name;

		if ((err = git_branch_name(&branch_name,head_ref)) == GIT_OK)
		{
			int num_parents = 0;
			sgit_repository_mergeheads_count(&num_parents, repo);
			if (num_parents)
			{
				printf("# On branch %s, but merging\n",branch_name);
			} else
			{
				printf("# On branch %s\n",branch_name);
			}
		}
	} else
	{
		printf("# On an unnamed branch\n");
	}

	if (!(err = git_branch_upstream(&upstream_ref, head_ref)))
	{
		const git_oid *local, *upstream;

		local = git_reference_target(head_ref);
		upstream = git_reference_target(upstream_ref);

		if (local && upstream)
		{
			const char *upstream_name;
			size_t ahead, behind;

			if ((err = git_branch_name(&upstream_name, upstream_ref)))
				goto out;

			if ((err = git_graph_ahead_behind(&ahead, &behind, repo, local, upstream)))
				goto out;

			if (ahead && behind)
			{
				printf("# Your branch and its upstream branch '%s' have diverged.\n", upstream_name);
				printf("# They have %d and %d different commits each, respectively.\n", (int)ahead, (int)behind);
			} else if (behind)
			{
				printf("# Your branch is behind its upstream branch '%s' by %d commits.\n", upstream_name, (int)behind);
			} else if (ahead)
			{
				printf("# Your branch is ahead its upstream branch '%s' by %d commits.\n", upstream_name, (int)ahead);
			}
		}
	} else if (err != GIT_ENOTFOUND)
	{
		goto out;
	}

	/* FIXME: Use git_status_list() API */
	opt.flags = GIT_STATUS_OPT_INCLUDE_UNMODIFIED |
			GIT_STATUS_OPT_INCLUDE_UNTRACKED |
			GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;
	opt.show = GIT_STATUS_SHOW_INDEX_ONLY;

	/* Index modifications */
	mode = 0<<1;
	if ((err = git_status_foreach_ext(repo,&opt,status_cb,&mode)) != GIT_OK)
		goto out;

	mode = 1<<1;

	/* Workdir vs Index modifications */
	opt.show = GIT_STATUS_SHOW_WORKDIR_ONLY;
	if ((err = git_status_foreach_ext(repo,&opt,status_cb,&mode)) != GIT_OK)
		goto out;

	/* Dito, but only new files are printed */
	mode = 2<<1;
	if ((err = git_status_foreach_ext(repo,&opt,status_cb,&mode)) != GIT_OK)
		goto out;

	printf("#\n");



	//get_modified_and_deleted_files (repo);

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (head_ref) git_reference_free(head_ref);
	return rc;
}


static int make_commmand (git_status_list *status_list, const size_t status_list_size, const char *command, const size_t num_elements, const git_status_t status, git_strarray *array)
{
	int rc = EXIT_FAILURE;
	char **tmp_paths = calloc (num_elements + 1, sizeof (char *));

	if (tmp_paths)
	{
		size_t i;
		size_t added = 0;
		char **current_tmp_path = tmp_paths;

		for (i = 0; i < status_list_size; ++ i)
		{
			const git_status_entry *entry = git_status_byindex (status_list, i);

			if ((entry -> status & status) == status)
			{
				git_diff_delta *delta = entry -> index_to_workdir;
				const char *old_path = delta -> old_file.path;
				const char *new_path = delta -> new_file.path;
				const char *path_to_use = NULL;

				/*
				 * Work out which path to use
				 */
				if (status & GIT_STATUS_WT_RENAMED)
				{
					if (strcmp (command, "add") == 0)
					{
						path_to_use = new_path;
					}
					else if (strcmp (command, "rm") == 0)
					{
						path_to_use = old_path;
					}
				}
				else
				{
					path_to_use = old_path;
				}

				if (path_to_use)
				{
					char *copied_path_to_use = strdup (path_to_use);

					if (copied_path_to_use)
					{
						*current_tmp_path = copied_path_to_use;
						++ current_tmp_path;
						++ added;
					}
				}
			}

		}

		/*
		 * Did we get all of the files?
		 */
		if (added == num_elements)
		{
			array -> count = added;
			array -> strings = tmp_paths;

			rc = EXIT_SUCCESS;
		}
		else
		{
			free (tmp_paths);
		}

	}

	return rc;
}


int get_modified_and_deleted_files (git_repository *repo, git_strarray *add_command, git_strarray *remove_command)
{
	int rc = EXIT_FAILURE;
	git_status_list *status_list;
	git_status_options opts = GIT_STATUS_OPTIONS_INIT;

	if (git_status_list_new (&status_list, repo, &opts) == 0)
	{
		char **paths = NULL;
		const size_t num_matched = git_status_list_entrycount (status_list);
		size_t num_updated = 0;
		size_t num_removed = 0;
		size_t num_renamed = 0;
		size_t i;
		char **tmp_paths = NULL;
		git_status_t status;

		/*
		 * Start by getting the number of added, removed and renamed files
		 */
		for (i = 0; i < num_matched; ++ i)
		{
			const git_status_entry *entry = git_status_byindex (status_list, i);

			if (entry -> status & GIT_STATUS_WT_MODIFIED)
			{
				++ num_updated;
			}
			else if (entry -> status & GIT_STATUS_WT_DELETED)
			{
				++ num_removed;
			}
			else if	(entry -> status & GIT_STATUS_WT_RENAMED)
			{
				++ num_renamed;
			}
		}

		status = GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_RENAMED;
		make_commmand (status_list, num_matched, "add", num_updated + num_renamed, status, add_command);

		status = GIT_STATUS_WT_DELETED | GIT_STATUS_WT_RENAMED;
		make_commmand (status_list, num_matched, "rm", num_updated + num_renamed, status, remove_command);

		git_status_list_free (status_list);
	}

	return rc;
}

