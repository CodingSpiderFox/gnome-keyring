/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
   Copyright (C) 2008 Stefan Walter

   The Gnome Keyring Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Keyring Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   <http://www.gnu.org/licenses/>.

   Author: Stef Walter <stef@memberwebs.com>
*/

#include "config.h"

#include "daemon/ssh-agent/gkd-ssh-openssh.h"

#include <glib.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct {
	const char *filename;
	const char *encoded;
} PUBLIC_FILES[] = {
	{ SRCDIR "/pkcs11/ssh-store/fixtures/id_rsa_test.pub",
	  "AAAAB3NzaC1yc2EAAAABIwAAAQEAoD6VKqkhay6pKHSRjAGWWfFPU8xfsi2gnOwP/B1UHDoztx3czhO+py/fTlhCnSP1jsjkrVIZcnzah2fUNFFRgS4+jROBtvbgHsS72V1E6+ZogV+mBJWWAhw0iPrmQ3Kvm38D3PByo5Y7yKO5kIG2LloYLjosJ5F4sx2xh0uz2wXNtnY1b5xhe2+VEksm9OB+FXaUkZC2fQrTNo8ZGFJQSFd8kUhIfbUDJmlYuZ+vvHM+A3Lc9rHyW4IPaRyxFQciRmb+ZQqU2uSdOXAhg17lskuX/q8yCI5Hy5eDicC222oUMdJTtYgwX4dQCU8TICWhxb3x4RCV+g7D99+tkIvv+w==" },
	{ SRCDIR "/pkcs11/ssh-store/fixtures/id_dsa_test.pub",
	  "AAAAB3NzaC1kc3MAAACBANHNmw2YHEodUj4Ae27i8Rm8uoLnpS68QEiCJx8bv9P1o0AaD0w55sH+TBzlo7vtAEDlAzIOBY3PMpy5WarELTIeXmFPzKfHL8tuxMbOPaN/wDkDZNnJZsqlyRwlQKStPcAlvLBNuMjA53u2ndMTVghtUHXETQzwxKhXf7TmvfLBAAAAFQDnF/Y8MgFCP0PpRC5ZAQo1dyDEwwAAAIEAr4iOpTeZx8i1QgQpRl+dmbBAtHTXbPiophzNJBge9lixqF0T3egN2B9wGGnumIXmnst9RPPjuu+cHCLfxhXHzLlW8MLwoiF6ZQOx9M8WcfWIl5oiGyr2e969woRf5OcMGQPOQBdws6MEtemRqq5gu6dqDqVl3xfhSZSP9LpqAI8AAACAUjiuQ3qGErsCz++qd0qrR++QA185XGXAPZqQEHcr4iKSlO17hSUYA03kOWtDaeRtJOlxjIjl9iLo3juKGFgxUfo2StScOSO2saTWFGjA4MybHCK1+mIYXRcYrq314yK2Tmbql/UGDWpcCCGXLWpSFHTaXTbJjPd6VL+TO9/8tFk=" }
};

#define COMMENT "A public key comment"

static void
test_parse_public (void)
{
	GBytes *input_bytes, *output_bytes;
	gchar *comment;
	guchar *data;
	const guchar *blob;
	gsize n_data;
	gchar *encoded;
	gsize i;

	for (i = 0; i < G_N_ELEMENTS (PUBLIC_FILES); ++i) {
		if (!g_file_get_contents (PUBLIC_FILES[i].filename, (gchar **)&data, &n_data, NULL))
			g_assert_not_reached ();

		input_bytes = g_bytes_new_take (data, n_data);
		output_bytes = gkd_ssh_openssh_parse_public_key (input_bytes, &comment);
		g_bytes_unref (input_bytes);
		g_assert (output_bytes);

		blob = g_bytes_get_data (output_bytes, &n_data);
		encoded = g_base64_encode (blob, n_data);
		g_bytes_unref (output_bytes);
		g_assert_cmpstr (encoded, ==, PUBLIC_FILES[i].encoded);
		g_free (encoded);

		g_assert_cmpstr (comment, ==, COMMENT);
		g_free (comment);
	}
}

int
main (int argc, char **argv)
{
#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init ();
#endif
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/ssh-agent/openssh/parse_public", test_parse_public);

	return g_test_run ();
}