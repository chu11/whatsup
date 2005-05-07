/*****************************************************************************\
 *  $Id: nodeupdown_util.h,v 1.6 2005-05-07 17:34:42 achu Exp $
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

#ifndef _NODEUPDOWN_UTIL_H
#define _NODEUPDOWN_UTIL_H

#include "nodeupdown.h"

/* 
 * _nodeupdown_handle_error_check
 * 
 * standard handle error checker
 *
 * Returns 0 on success, -1 one error
 */
int _nodeupdown_handle_error_check(nodeupdown_t handle); 

/* 
 * Nodeupdown_util_load_module
 *
 * Define a load module function to be passed to
 * nodeupdown_util_lookup_module().
 *
 * Returns 1 if module is found and loaded successfully, 0 if module
 * cannot be found, -1 on fatal error.
 */
typedef int (*Nodeupdown_util_load_module)(nodeupdown_t, char *);

/* 
 * _nodeupdown_util_low_timeout_connect
 *
 * Setup a tcp connection to 'hostname' and 'port' using a connection
 * timeout of 'connect_timeout'.
 *
 * Return file descriptor on success, -1 on error.
 */
int _nodeupdown_util_low_timeout_connect(nodeupdown_t handle, 
					 const char *hostname, 
					 int port,
					 int connect_timeout); 

/* 
 * nodeupdown_util_lookup_module
 *
 * Search the directory 'search_dir' for any one of the modules listed
 * in 'modules_list'.  Call 'load_module' on any discovered module.
 *
 * Returns 1 if a module is found and loaded, 0 if a module is not
 * found, -1 on fatal error.
 */
int _nodeupdown_util_lookup_module(nodeupdown_t handle, 
				   char *search_dir,
				   char **modules_list,
				   int modules_list_len,
				   Nodeupdown_util_load_module load_module);

/*
 * cerebrod_search_for_module
 *
 * Search the directory 'search_dir' for any module with the given signature.
 *
 * Returns 1 when a module is found, 0 when one is not, -1 on fatal error
 */
int _nodeupdown_util_search_for_module(nodeupdown_t handle,
				       char *search_dir,
				       char *signature,
				       Nodeupdown_util_load_module load_module);


#endif /* _NODEUPDOWN_UTIL_H */
