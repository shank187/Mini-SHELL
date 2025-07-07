/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_utils2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abel-had <abel-had@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 11:40:35 by abel-had          #+#    #+#             */
/*   Updated: 2025/07/06 14:17:49 by abel-had         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/minishell.h"

void	if_line(t_tools *tools, t_sp_var *v)
{
	bool	just_space;

	just_space = ft_just_space(v->line);
	if (g_signal_pid == 3 || g_signal_pid == 2)
		g_signal_pid = 0;
	status_manage(v);
	if (g_signal_pid == -1)
	{
		v->status = 1;
		g_signal_pid = 0;
	}
	process_commands(tools, v, just_space);
	clean_garbage(tools->aloc);
}

void	main_loop(t_tools *tools, t_sp_var *v, struct termios *terminal)
{
	int	i;

	while (1)
	{
		v->previous_state = -1;
		if (reset_g(v))
			return ;
		check_line(v);
		if (*v->line != '\0')
			if_line(tools, v);
		else if (v->line && v->line[0] == '\0')
			free(v->line);
		v->a = 0;
		tcsetattr(0, TCSANOW, terminal);
		i = 2;
		while (++i <= 10240)
			close(i);
	}
}
