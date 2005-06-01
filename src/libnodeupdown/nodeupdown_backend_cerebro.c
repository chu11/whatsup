/*****************************************************************************\
 *  $Id: nodeupdown_backend_cerebro.c,v 1.9 2005-06-01 19:36:36 achu Exp $
 *****************************************************************************
 *  Copyright (C) 2003 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Albert Chu <chu11@llnl.gov>
 *  UCRL-CODE-155699
 *  
 *  This file is part of Whatsup, tools and libraries for determining up and
 *  down nodes in a cluster. For details, see http://www.llnl.gov/linux/.
 *  
 *  Whatsup is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by the 
 *  Free Software Foundation; either version 2 of the License, or (at your 
 *  option) any later version.
 *  
 *  Whatsup is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 *  for more details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with Whatsup; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
\*****************************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <sys/types.h>
#include <errno.h>

#include "nodeupdown.h"
#include "nodeupdown_module.h"
#include "nodeupdown_backend_util.h"
#include "nodeupdown/nodeupdown_backend_module.h"
#include "nodeupdown/nodeupdown_constants.h"
#include "nodeupdown/nodeupdown_devel.h"

#include <cerebro.h>
#include <cerebro/cerebro_metric_protocol.h>

static cerebro_t cerebro_handle = NULL;

char cerebro_default_hostname[NODEUPDOWN_MAXHOSTNAMELEN+1];

/*
 * cerebro_backend_default_hostname
 *
 * cerebro backend module default_hostname function
 */
char *
cerebro_backend_default_hostname(nodeupdown_t handle)
{
  memset(cerebro_default_hostname, '\0', NODEUPDOWN_MAXHOSTNAMELEN+1);
  if (gethostname(cerebro_default_hostname, NODEUPDOWN_MAXHOSTNAMELEN) < 0)
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_INTERNAL);
      return NULL;
    }
  return &cerebro_default_hostname[0];
}

/*
 * cerebro_backend_default_port
 *
 * cerebro backend module default_port function
 */
int 
cerebro_backend_default_port(nodeupdown_t handle)
{
  return CEREBRO_METRIC_SERVER_PORT;
}

/*
 * cerebro_backend_default_timeout_len
 *
 * cerebro backend module default_timeout_len function
 */
int 
cerebro_backend_default_timeout_len(nodeupdown_t handle)
{
  return CEREBRO_METRIC_UPDOWN_TIMEOUT_LEN_DEFAULT;
}

/*
 * cerebro_backend_setup
 *
 * cerebro backend module setup function
 */
int 
cerebro_backend_setup(nodeupdown_t handle)
{
  if (cerebro_handle)
    return 0;

  if (!(cerebro_handle = cerebro_handle_create()))
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      goto cleanup;
    }

  return 0;
 cleanup:
  if (cerebro_handle)
    cerebro_handle_destroy(cerebro_handle);
  cerebro_handle = NULL;
  return -1;
}

/*
 * cerebro_backend_cleanup
 *
 * cerebro backend module cleanup function
 */
int
cerebro_backend_cleanup(nodeupdown_t handle)
{
  if (!cerebro_handle)
    return 0;

  if (cerebro_handle_destroy(cerebro_handle) < 0)
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      return -1;
    }

  return 0;
}

/*
 * cerebro_backend_get_updown_data
 *
 * cerebro backend module get_updown_data function
 */
int 
cerebro_backend_get_updown_data(nodeupdown_t handle, 
                                const char *hostname,
                                unsigned int port,
                                unsigned int timeout_len,
                                char *reserved) 
{
  cerebro_nodelist_t nodelist = NULL;
  cerebro_nodelist_iterator_t itr = NULL;
  int flag, rv = -1;

  if (cerebro_set_hostname(cerebro_handle, hostname) < 0)
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      return -1;
    }

  if (cerebro_set_port(cerebro_handle, port) < 0)
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      return -1;
    }

  if (cerebro_set_timeout_len(cerebro_handle, timeout_len) < 0)
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      return -1;
    }

  if (!(nodelist = cerebro_get_metric_data(cerebro_handle, 
                                           CEREBRO_METRIC_UPDOWN_STATE)) < 0)
    {
      if (cerebro_errnum(cerebro_handle) == CEREBRO_ERR_CONNECT) 
        nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_CONNECT);
     else if (cerebro_errnum(cerebro_handle) == CEREBRO_ERR_CONNECT_TIMEOUT)
        nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_CONNECT_TIMEOUT);
      else if (cerebro_errnum(cerebro_handle) == CEREBRO_ERR_HOSTNAME)
        nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_HOSTNAME);
      else
        nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      return -1;
    }

  if (cerebro_nodelist_metric_type(nodelist) != CEREBRO_METRIC_TYPE_UNSIGNED_INT32)
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      goto cleanup;
    }
  
  if (!(itr = cerebro_nodelist_iterator_create(nodelist)))
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      goto cleanup;
    }
  
  while (!(flag = cerebro_nodelist_iterator_at_end(itr)))
    {
      char *nodename;
      u_int32_t *updown_state;
      u_int32_t size;

      if (!(nodename = cerebro_nodelist_iterator_nodename(itr)))
        {
          nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
          goto cleanup;
        }

      if (!(updown_state = cerebro_nodelist_iterator_metric_value(itr,
                                                                  &size)))
        {
          nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
          goto cleanup;
        }

      if (size != sizeof(u_int32_t))
        {
          nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
          goto cleanup;
        }

      if ((*updown_state))
        {
          if (nodeupdown_add_up_node(handle, nodename) < 0)
            goto cleanup;
        }
      else
        {
          if (nodeupdown_add_down_node(handle, nodename) < 0)
            goto cleanup;
        }

      if (cerebro_nodelist_iterator_next(itr) < 0)
        {
          nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
          goto cleanup;
        }
    }

  if (flag < 0)
    {
      nodeupdown_set_errnum(handle, NODEUPDOWN_ERR_BACKEND_MODULE);
      goto cleanup;
    }

  rv = 0;
 cleanup:
  if (itr)
    cerebro_nodelist_iterator_destroy(itr);
  if (nodelist)
    cerebro_nodelist_destroy(nodelist);
  return rv;
}

#if WITH_STATIC_MODULES
struct nodeupdown_backend_module_info cerebro_backend_module_info = 
#else  /* !WITH_STATIC_MODULES */
struct nodeupdown_backend_module_info backend_module_info = 
#endif /* !WITH_STATIC_MODULES */
  {
    "cerebro",
    &cerebro_backend_default_hostname,
    &cerebro_backend_default_port,
    &cerebro_backend_default_timeout_len,
    &cerebro_backend_setup,
    &cerebro_backend_cleanup,
    &cerebro_backend_get_updown_data
  };
