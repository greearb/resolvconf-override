/* -*- Mode: C; indent-tabs-mode: t -*- */

/*
 * Copyright 2013, 2017 Bastien Nocera
 * Copyright 2020 Ben Greear <greearb@candelatech.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 *
 * Authors: Bastien Nocera <hadess@hadess.net>
 *
 */

#include <resolv.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>

#define __USE_GNU
#include <dlfcn.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int done_override = 0;
/* 111.222.333.444 (15 characters max) */
#define MAX_ADDR_SIZE 16
static char my_addrlist[MAXNS][MAX_ADDR_SIZE];
static int my_addrlist_len = 0;
static int my_use_tcp = 0;

/* For debugging */
#if 0
static void
print_ns (void)
{
	int i;
	for(i=0;i<_res.nscount;i++) {
		char buf[INET_ADDRSTRLEN];

		inet_ntop(AF_INET, &_res.nsaddr_list[i].sin_addr, buf, sizeof(buf));
		fprintf(stderr, "dnssrvr: %s\n", buf);
	}
}
#endif

static void
override_ns (void)
{
	int i;
	int valid_ns = 0;

	if (my_addrlist_len) {
	   int mx = MAXNS;
	   if (my_addrlist_len < mx)
	      mx = my_addrlist_len;
	   for (i = 0; i < mx; i++) {
		if (inet_pton (AF_INET, my_addrlist[i], &_res.nsaddr_list[i].sin_addr) < 1) {
			fprintf (stderr, "Ignoring invalid nameserver '%s'\n", my_addrlist[i]);
			continue;
		}

		valid_ns++;
	   }
	}
	else {
	   for (i = 0; i < MAXNS; i++) {
		char envvar[] = "NAMESERVERx";
		const char *ns;

		/* Rename to NAMESERVERx where x is the name server number,
		 * and the first one is NAMESERVER1 */
		envvar[10] = '1' + i;

		ns = getenv (envvar);
		if (ns == NULL)
			break;
		if (inet_pton (AF_INET, ns, &_res.nsaddr_list[i].sin_addr) < 1) {
			fprintf (stderr, "Ignoring invalid nameserver '%s'\n", ns);
			continue;
		}

		valid_ns++;
	   }
	}

	/* Set the number of valid nameservers */
	if (valid_ns > 0)
		_res.nscount = valid_ns;
}

static void
override_options (void)
{
	if (my_use_tcp || getenv ("FORCE_DNS_OVER_TCP") != NULL)
		_res.options |= RES_USEVC;
}

void resolvconf_override_init(const char** servers, int len, int use_tcp) {
   int i;
   
   my_use_tcp = use_tcp;
   memset(my_addrlist, 0, sizeof(my_addrlist));
   for (i = 0; i<len; i++) {
      strncpy(my_addrlist[i], servers[i], MAX_ADDR_SIZE-1);
   }
   my_addrlist_len = len;
   done_override = 0;
}

struct hostent *gethostbyname(const char *name)
{
        if (!done_override) {
	   done_override = 1;
	   if (res_init () < 0)
		return NULL;
	}
	{
	   struct hostent * (*f)() = dlsym (RTLD_NEXT, "gethostbyname");
	   struct hostent *ret =  f(name);

	   return ret;
	}
}

int getaddrinfo(const char *node, const char *service,
		const struct addrinfo *hints,
		struct addrinfo **res)
{
        if (!done_override) {
	   done_override = 1;
	   if (res_init () < 0)
	      return EAI_SYSTEM;
	}
	{
	   int (*f)() = dlsym (RTLD_NEXT, "getaddrinfo");
	   return f(node, service, hints, res);
	}
}

int __res_init(void)
{
	int (*f)() = dlsym (RTLD_NEXT, "__res_init");
	assert (f);
	int ret = f();

	override_ns ();
	override_options ();

	return ret;
}
