/*
 *  $Id: nodeupdown_masterlist.c,v 1.7 2003-11-24 16:13:19 achu Exp $
 *  $Source: /g/g0/achu/temp/whatsup-cvsbackup/whatsup/src/libnodeupdown/nodeupdown_masterlist.c,v $
 *    
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>

#include "fd.h"
#include "hostlist.h"
#include "nodeupdown.h"
#include "nodeupdown_masterlist.h"
#include "nodeupdown_common.h"

void nodeupdown_masterlist_initialize_handle(nodeupdown_t handle) {
#if HAVE_HOSTSFILE
  handle->hosts = NULL;
#elif (HAVE_GENDERS || HAVE_GENDERSLLNL)
  handle->genders_handle = NULL;
#endif 
}

void nodeupdown_masterlist_free_handle_data(nodeupdown_t handle) {
#if HAVE_HOSTSFILE
  if (handle->hosts)
    (void)list_destroy(handle->hosts);
#elif (HAVE_GENDERS || HAVE_GENDERSLLNL)
  (void)genders_handle_destroy(handle->genders_handle);
#endif
}

#if HAVE_HOSTSFILE
static int _readline(nodeupdown_t handle, int fd, char *buf, int buflen) {
  int ret;

  if ((ret = fd_read_line(fd, buf, buflen)) < 0) {
    handle->errnum = NODEUPDOWN_ERR_READ;
    return -1;
  }
  
  /* overflow counts as a parse error */
  /* XXX assumes buflen >= 2 */
  if (ret == buflen && buf[buflen-2] != '\n') {
    handle->errnum = NODEUPDOWN_ERR_INTERNAL;
    return -1;
  }

  return ret;
}

static int _load_hostsfile_data(nodeupdown_t handle, const char *filename) {
  int fd, len, retval = -1;
  char buf[NODEUPDOWN_BUFFERLEN];

  if (filename == NULL)
    filename = NODEUPDOWN_MASTERLIST_DEFAULT; /* defined in config.h */

  if ((handle->hosts = list_create(free)) == NULL) {
    handle->errnum = NODEUPDOWN_ERR_OUTMEM;
    return -1;
  }

  if ((fd = open(filename, O_RDONLY)) < 0) {
    handle->errnum = NODEUPDOWN_ERR_OPEN;
    return -1;
  }

  while ((len = _readline(handle, fd, buf, NODEUPDOWN_BUFFERLEN)) > 0) {
    if (buf[0] == '#')		/* skip comments */
      continue;
    else if (strlen(buf) > MAXHOSTNAMELEN) {
      handle->errnum = NODEUPDOWN_ERR_READ;
      goto cleanup;
    }
    else if (strchr(buf, '.') != NULL) {
      handle->errnum = NODEUPDOWN_ERR_READ;
      goto cleanup;
    }
    else {
      char *temp;

      /* Strip white space from end */
      temp = &(buf[0]) + len;
      for (--temp; temp >= &(buf[0]); temp--) {
        if (isspace(*temp))
          *temp = '\0';
        else
          break;
      }

      /* skip empty lines */
      if (buf[0] == '\0')
        continue;

      if ((temp = strdup(buf)) == NULL) {
        handle->errnum = NODEUPDOWN_ERR_OUTMEM;
        goto cleanup;
      }
      
      if (list_append(handle->hosts, temp) == NULL) {
        handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
        free(temp);
        goto cleanup;
      }
    }
  }

  if (len == -1)
    goto cleanup;

  retval = 0;
 cleanup:
  close(fd);
  return retval;
}
#endif /* HAVE_HOSTSFILE */

#if (HAVE_GENDERS || HAVE_GENDERSLLNL)
static int _load_genders_data(nodeupdown_t handle, const char *filename) {
  /* determine filename */
  if (filename == NULL)
    filename = NODEUPDOWN_MASTERLIST_DEFAULT;/* defined in config.h */

  if ((handle->genders_handle = genders_handle_create()) == NULL) {
    handle->errnum = NODEUPDOWN_ERR_OUTMEM;
    return -1;
  }

  if (genders_load_data(handle->genders_handle, filename) == -1) {
    handle->errnum = NODEUPDOWN_ERR_OPEN;
    return -1;
  }

  return 0;
}
#endif /* HAVE_GENDERS || HAVE_GENDERSLLNL */

int nodeupdown_masterlist_init(nodeupdown_t handle, void *ptr) {
#if HAVE_HOSTSFILE
  return _load_hostsfile_data(handle, (const char *)ptr);
#elif (HAVE_GENDERS || HAVE_GENDERSLLNL)
  return _load_genders_data(handle, (const char *)ptr);
#else
  return 0;
#endif
}

int nodeupdown_masterlist_finish(nodeupdown_t handle) {
#if HAVE_HOSTSFILE
  /* set max_nodes */
  handle->max_nodes = list_count(handle->hosts);
  return 0;
#elif (HAVE_GENDERS || HAVE_GENDERSLLNL)
  /* set max_nodes */
  if ((handle->max_nodes = genders_getnumnodes(handle->genders_handle)) == -1) {
    handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
    return -1;
  }
  return 0;
#else
  return 0;
#endif
}

int nodeupdown_masterlist_compare_gmond_to_masterlist(nodeupdown_t handle) {
#if HAVE_HOSTSFILE
  ListIterator itr = NULL;
  char *nodename;

  if ((itr = list_iterator_create(handle->hosts)) == NULL) {
    handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
    return -1;
  }

  while (nodename = list_next(itr)) {
    if ((hostlist_find(handle->up_nodes, nodename) == -1) &&
        (hostlist_find(handle->down_nodes, nodename) == -1)) {
      /* This node must also be down */
      if (hostlist_push_host(handle->down_nodes, nodename) == 0) {
        handle->errnum = NODEUPDOWN_ERR_HOSTLIST;
        goto cleanup;
      }
    }    
  }

  list_iterator_destroy(itr);
  hostlist_sort(handle->down_nodes);
  return 0;

 cleanup: 
  list_iterator_destroy(itr);
  return -1;
#elif (HAVE_GENDERS || HAVE_GENDERSLLNL)
  int i, ret, num;
  char **nlist = NULL;
  genders_t gh = handle->genders_handle;

  /* get all genders nodes */
  if ((num = genders_nodelist_create(gh, &nlist)) == -1) {
    handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
    goto cleanup;
  }
  
  if (genders_getnodes(gh, nlist, num, NULL, NULL) == -1) {
    handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
    goto cleanup;
  }
  
  for (i = 0; i < num; i++) {
    /* check if gmond knows of this genders node */
    if ((hostlist_find(handle->up_nodes, nlist[i]) == -1) &&
        (hostlist_find(handle->down_nodes, nlist[i]) == -1)) {

      /* gmond doesn't know this genders node, it must also be down */
      if (hostlist_push_host(handle->down_nodes, nlist[i]) == 0) {
        handle->errnum = NODEUPDOWN_ERR_HOSTLIST;
        goto cleanup;
      }
    }
  }

  if (genders_nodelist_destroy(gh, nlist) == -1) {
    handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
    goto cleanup;
  }

  hostlist_sort(handle->down_nodes);
  return 0;

 cleanup: 
  (void)genders_nodelist_destroy(gh, nlist);
  return -1;
#else
  return 0;
#endif
}

#if HAVE_HOSTSFILE
static int _find_str(void *x, void *key) {
  if (strcmp((char *)x, (char *)key) == 0)
    return 1;
  return 0;
}
#endif

static int _is_node_common(nodeupdown_t handle, const char *node) {
#if HAVE_HOSTSFILE
  void *ptr;
  ptr = list_find_first(handle->hosts, _find_str, (void *)node);
  if (ptr != NULL)
    return 1;
  else
    return 0;
#elif HAVE_GENDERS
  int ret;
  if ((ret = genders_isnode(handle->genders_handle, node)) == -1) {
    handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
    return -1;
  }
  return ret;
#elif HAVE_GENDERSLLNL
  int ret;
  if ((ret = genders_isnode_or_altnode(handle->genders_handle, node)) == -1) {
    handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
    return -1;
  }
  return ret;
#endif
}

int nodeupdown_masterlist_is_node_legit(nodeupdown_t handle, const char *node) {
#if (HAVE_HOSTSFILE || HAVE_GENDERS || HAVE_GENDERSLLNL)
  return _is_node_common(handle, node);
#else
  /* Have to assume it is */
  return 1;
#endif  
}

int nodeupdown_masterlist_is_node_in_cluster(nodeupdown_t handle, const char *node) {
#if (HAVE_HOSTSFILE || HAVE_GENDERS || HAVE_GENDERSLLNL)
  return _is_node_common(handle, node);
#else
  /* Without a master list of some sort, this is the best we can do */
  if (hostlist_find(handle->up_nodes, node) == -1 &&
      hostlist_find(handle->down_nodes, node) == -1)
    return 0;
  else
    return 1;
#endif  
}

int nodeupdown_masterlist_get_nodename(nodeupdown_t handle, const char *node, 
                                      char *buffer, int buflen) {
#if HAVE_GENDERSLLNL
  if (genders_to_gendname(handle->genders_handle, node, buffer, buflen) == -1) {
    handle->errnum = NODEUPDOWN_ERR_MASTERLIST;
    return -1;
  }
  return 0;
#else
  if ((strlen(node) + 1) > buflen) {
    handle->errnum = NODEUPDOWN_ERR_PARAMETERS;
    return -1;
  }
  strcpy(buffer, node);
  return 0;
#endif
}
    
int nodeupdown_masterlist_increase_max_nodes(nodeupdown_t handle) {
#if HAVE_NOMASTERLIST
  handle->max_nodes++;
  return 0;
#else
  /* If using masterlist, use list_count in finish */
  /* If using genders or gendersllnl, use genders_numnodes in finish */
  return 0;
#endif
}

