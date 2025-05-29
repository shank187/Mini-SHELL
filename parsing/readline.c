/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abel-had <abel-had@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 10:15:02 by abel-had          #+#    #+#             */
/*   Updated: 2025/05/28 09:45:53 by abel-had         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

int	resize_buffer(char **buf, int *cap, t_sp_var *va)
{
	char	*t;

	*cap *= 2;
	t = f_realloc(*buf, *cap, va);
	if (!t)
		return (-1);
	*buf = t;
	return (0);
}

int	process_character(t_readline *v, t_sp_var *va)
{
	if (*(v->len) + 1 >= *(v->cap))
	{
		if (resize_buffer(v->buf, v->cap, va) == -1)
			return (-1);
	}
	(*(v->buf))[(*(v->len))++] = v->c;
	if (v->c == '\n')
		return (1);
	return (0);
}

int	heredoc_read_loop(char **buf, int *len, int *cap, t_sp_var *va)
{
	t_readline	v;
	int			r;

	v.buf = buf;
	v.len = len;
	v.cap = cap;
	while (1)
	{
		r = read(0, &v.c, 1);
		if (r == 0)
			return (0);
		if (r < 0)
			return (handle_read_error(buf));
		if (process_character(&v, va) == 1)
			break ;
	}
	return (*len);
}

int	heredoc_readline(char **out, t_sp_var *va)
{
	int		cap;
	int		len;
	char	*buf;
	int		a;

	cap = 128;
	len = 0;
	buf = mmallocc(cap, &va->allocs, P_GARBAGE);
	if (!buf)
		return (-1);
	a = heredoc_read_loop(&buf, &len, &cap, va);
	if (a <= 0)
	{
		*out = NULL;
		if (a == -2)
			return (-2);
		else
			return (0);
	}
	buf[len] = '\0';
	*out = buf;
	if (!(*out))
		return (-3);
	return (len);
}
