/*****************************************************************************\
 *  $Id: nodeupdown_clusterlist.h,v 1.3 2005-04-22 20:44:02 achu Exp $
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

#ifndef _NODEUPDOWN_CLUSTERLIST_H
#define _NODEUPDOWN_CLUSTERLIST_H

#include "nodeupdown.h"

/*
 * Nodeupdown_clusterlist_setup
 *
 * Setup the clusterlist module
 *
 * Return 0 on success, -1 on error
 */
typedef int (*Nodeupdown_clusterlist_setup)(nodeupdown_t);

/*
 * Nodeupdown_clusterlist_cleanup
 *
 * Cleanup clusterlist module allocations
 *
 * Return 0 on success, -1 on error
 */
typedef int (*Nodeupdown_clusterlist_cleanup)(nodeupdown_t);

/*
 * Nodeupdown_clusterlist_complete_loading
 *
 * Complete loading process after backend get_updown_data function has
 * been done.
 *
 * Returns 0 on success, -1 on error
 */
typedef int (*Nodeupdown_clusterlist_complete_loading)(nodeupdown_t);

/*
 * Nodeupdown_clusterlist_compare_to_clusterlist
 *
 * Compare nodes currently discovered to clusterlist database to
 * determine if additional nodes are down.
 *
 * Returns 0 on success, -1 on error
 */
typedef int (*Nodeupdown_clusterlist_compare_to_clusterlist)(nodeupdown_t);

/*
 * Nodeupdown_clusterlist_is_node_in_cluster
 *
 * Determines if a node is in the cluster.
 *
 * Returns 1 is node is in the cluster, 0 if not, -1 on error
 */
typedef int (*Nodeupdown_clusterlist_is_node_in_cluster)(nodeupdown_t, const char *);

/*
 * Nodeupdown_clusterlist_is_node_discovered
 *
 * Determines if a node has been discovered.  Generally identical to
 * is_node_in_cluster, although some clusterlist modules act
 * differently.
 *
 * Returns 1 is node has been discovered, 0 if not, -1 on error
 *
 */
typedef int (*Nodeupdown_clusterlist_is_node_discovered)(nodeupdown_t, const char *);

/*
 * Nodeupdown_clusterlist_get_nodename
 *
 * Determine the nodename to use for calculations.  Typically, this
 * function will only copy the node passed in into the buffer passed
 * in.  However, in some circumstances, nodes with duplicate names
 * (perhaps aliased) need to be identified with a single nodename.
 *
 * Returns nodename in buffer, 0 on success, -1 on error
 */
typedef int (*Nodeupdown_clusterlist_get_nodename)(nodeupdown_t, const char *, char *, int);

/*
 * Nodeupdown_clusterlist_increase_max_nodes
 *
 * Increase the maximum number of nodes discovered thus far.  Some
 * modules may not need to do this, and thus this function does
 * nothing.
 *
 * Returns 0 on success, -1 on error.
 */
typedef int (*Nodeupdown_clusterlist_increase_max_nodes)(nodeupdown_t);

/*
 * struct nodeupdown_clusterlist_module_info
 *
 * contains clusterlist module information and operations.  Required
 * to be defined in each clusterlist module.
 */
struct nodeupdown_clusterlist_module_info
{
  char *clusterlist_module_name;
  Nodeupdown_clusterlist_setup setup;
  Nodeupdown_clusterlist_cleanup cleanup;
  Nodeupdown_clusterlist_complete_loading complete_loading;
  Nodeupdown_clusterlist_compare_to_clusterlist compare_to_clusterlist;
  Nodeupdown_clusterlist_is_node_in_cluster is_node_in_cluster;
  Nodeupdown_clusterlist_is_node_discovered is_node_discovered;
  Nodeupdown_clusterlist_get_nodename get_nodename;
  Nodeupdown_clusterlist_increase_max_nodes increase_max_nodes;
};

/*
 * nodeupdown_clusterlist_load_module
 *
 * Find and load the nodeupdown clusterlist module
 *
 * Returns 0 on success, -1 on error
 */
int nodeupdown_clusterlist_load_module(nodeupdown_t handle, char *clusterlist_module);

/*  
 * nodeupdown_clusterlist_unload_module
 *
 * unload the nodeupdown clusterlist module
 *
 * Returns 0 on success, -1 on error

 */
int nodeupdown_clusterlist_unload_module(nodeupdown_t handle);

/* 
 * nodeupdown_clusterlist_setup
 *
 * call clusterlist module setup function
 */
int nodeupdown_clusterlist_setup(nodeupdown_t handle);

/* 
 * nodeupdown_clusterlist_cleanup
 *
 * call clusterlist module cleanup function
 */
int nodeupdown_clusterlist_cleanup(nodeupdown_t handle);

/* 
 * nodeupdown_clusterlist_complete_loading
 *
 * call clusterlist module complete_loading function
 */
int nodeupdown_clusterlist_complete_loading(nodeupdown_t handle);

/* 
 * nodeupdown_clusterlist_compare_to_clusterlist
 *
 * call clusterlist module compare_to_clusterlist function
 */
int nodeupdown_clusterlist_compare_to_clusterlist(nodeupdown_t handle);

/* 
 * nodeupdown_clusterlist_is_node_in_cluster
 *
 * call clusterlist module is_node_in_cluster function
 */
int nodeupdown_clusterlist_is_node_in_cluster(nodeupdown_t handle, const char *node);

/* 
 * nodeupdown_clusterlist_is_node_discovered
 *
 * call clusterlist module is_node_discovered function
 */
int nodeupdown_clusterlist_is_node_discovered(nodeupdown_t handle, const char *node);

/* 
 * nodeupdown_clusterlist_get_nodename
 *
 * call clusterlist module get_nodename function
 */
int nodeupdown_clusterlist_get_nodename(nodeupdown_t handle, const char *node, char *buffer, int buflen);

/* 
 * nodeupdown_clusterlist_increase_max_nodes
 *
 * call clusterlist module increase_max_nodes function
 */
int nodeupdown_clusterlist_increase_max_nodes(nodeupdown_t handle);

#endif /* _NODEUPDOWN_CLUSTERLIST_H */
