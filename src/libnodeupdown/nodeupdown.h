/*****************************************************************************\
 *  $Id: nodeupdown.h,v 1.36 2005-05-06 18:27:46 achu Exp $
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

#ifndef _NODEUPDOWN_H
#define _NODEUPDOWN_H

#include <stdio.h>

/*
 * Nodeupdown Error Codes
 */

#define NODEUPDOWN_ERR_SUCCESS               0
#define NODEUPDOWN_ERR_NULLHANDLE            1
#define NODEUPDOWN_ERR_CONNECT               2
#define NODEUPDOWN_ERR_TIMEOUT               3
#define NODEUPDOWN_ERR_HOSTNAME              4
#define NODEUPDOWN_ERR_ADDRESS               5
#define NODEUPDOWN_ERR_ISLOADED              6
#define NODEUPDOWN_ERR_NOTLOADED             7
#define NODEUPDOWN_ERR_OVERFLOW              8
#define NODEUPDOWN_ERR_PARAMETERS            9
#define NODEUPDOWN_ERR_NULLPTR              10
#define NODEUPDOWN_ERR_OUTMEM               11
#define NODEUPDOWN_ERR_NOTFOUND             12
#define NODEUPDOWN_ERR_CLUSTERLIST_MODULE   13
#define NODEUPDOWN_ERR_BACKEND_MODULE       14
#define NODEUPDOWN_ERR_CONFIG_MODULE        15
#define NODEUPDOWN_ERR_CONF_PARSE           16
#define NODEUPDOWN_ERR_CONF_INPUT           17
#define NODEUPDOWN_ERR_CONF_INTERNAL        18
#define NODEUPDOWN_ERR_MAGIC                19
#define NODEUPDOWN_ERR_INTERNAL             20
#define NODEUPDOWN_ERR_ERRNUMRANGE          21

typedef struct nodeupdown *nodeupdown_t;

/* 
 * nodeupdown_handle_create
 * 
 * create a nodeupdown handle
 *
 * Returns handle on success, NULL on error
 */
nodeupdown_t nodeupdown_handle_create(void);

/* nodeupdown_handle_destroy
 *
 * destroy a nodeupdown handle
 *
 * Returns 0 on success, -1 on error
 */
int nodeupdown_handle_destroy(nodeupdown_t handle);

/* 
 * nodeupdown_load_data
 *
 * loads data from the backend tool used to evaluate
 * up and down nodes
 * - if hostname is NULL, port is <= 0, or timeout_len <=0, the
 * backend tool's respective defaults will be used.
 * - 'reserved' is used for backwards compatability
 *
 * Returns 0 on success, -1 on error
 */
int nodeupdown_load_data(nodeupdown_t handle, 
                         const char *hostname, 
                         int port,
                         int timeout_len,
                         char *reserved); 

/* 
 * nodeupdown_errnum
 *
 * return the most recent error number
 *
 * Returns error number on success
 */
int nodeupdown_errnum(nodeupdown_t handle);

/* 
 * nodeupdown_strerror
 *
 * return a string message describing an error number
 *
 * Returns pointer to message on success
 */
char *nodeupdown_strerror(int errnum);

/* 
 * nodeupdown_errormsg
 * 
 * return a string message describing the most recent error
 *
 * Returns pointer to message on success
 */
char *nodeupdown_errormsg(nodeupdown_t handle);

/* 
 * nodeupdown_perror
 *
 * Output a message to standard error 
 */
void nodeupdown_perror(nodeupdown_t handle, const char *msg);

/* 
 * nodeupdown_get_up_nodes_string
 *
 * Retrieve a ranged string of up nodes and store it in the buffer
 *
 * Returns 0 on success, -1 on error
 */
int nodeupdown_get_up_nodes_string(nodeupdown_t handle, 
                                   char *buf, 
                                   int buflen);

/* 
 * nodeupdown_get_down_nodes_string
 *
 * Retrieve a ranged string of down nodes and store it in the buffer
 *
 * Returns 0 on success, -1 on error
 */
int nodeupdown_get_down_nodes_string(nodeupdown_t handle, 
                                     char *buf, 
                                     int buflen);

/* 
 * nodeupdown_get_up_nodes_list
 *
 * Retrieve a list of up nodes and store them in the list
 *
 * Returns number of nodes copied on success, -1 on error
 */
int nodeupdown_get_up_nodes_list(nodeupdown_t handle, char **list, int len);

/* 
 * nodeupdown_get_down_nodes_list
 *
 * Retrieve a list of down nodes and store them in the list
 *
 * Returns number of nodes copied on success, -1 on error
 */
int nodeupdown_get_down_nodes_list(nodeupdown_t handle, char **list, int len);

/* 
 * nodeupdown_is_node_up
 *
 * Check if a node is up
 *
 * Returns 1 if up, 0 if down, -1 on error
 */
int nodeupdown_is_node_up(nodeupdown_t handle, const char *node);

/* 
 * nodeupdown_is_node_down
 *
 * Check if a node is down
 *
 * Returns 1 if down, 0 if up, -1 on error
 */
int nodeupdown_is_node_down(nodeupdown_t handle, const char *node);

/* 
 * nodeupdown_up_count
 *
 * Returns number of nodes that are up, -1 on error
 */
int nodeupdown_up_count(nodeupdown_t handle);

/* 
 * nodeupdown_down_count
 *
 * Returns number of nodes that are down, -1 on error
 */
int nodeupdown_down_count(nodeupdown_t handle);

/* 
 * nodeupdown_nodelist_create
 *
 * Allocate an array to store node names in
 *
 * Returns number of node entries created on success, -1 on error
 */
int nodeupdown_nodelist_create(nodeupdown_t handle, char ***list);

/* 
 * nodeupdown_nodelist_clear
 *
 * Clear a previously allocated nodelist
 *
 * Returns 0 on success, -1 on error
 */
int nodeupdown_nodelist_clear(nodeupdown_t handle, char **list);

/* 
 * nodeupdown_nodelist_destroy
 *
 * Destroy a previously allocated nodelist
 *
 * Returns 0 on success, -1 on error
 */
int nodeupdown_nodelist_destroy(nodeupdown_t handle, char **list);

#endif /* _NODEUPDOWN_H */
