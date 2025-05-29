/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_signals.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abel-had <abel-had@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 11:29:52 by abel-had          #+#    #+#             */
/*   Updated: 2025/05/29 09:15:34 by abel-had         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal_pid = 0;

static int	handle_sigint_part1(void)
{
	if (g_signal_pid == 2)
	{
		g_signal_pid = -2;
		return (0);
	}
	if (g_signal_pid == -2)
	{
		write(1, "\n", 1);
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
		return (0);
	}
	if (g_signal_pid == 4)
		return (0);
	return (1);
}

static void	handle_sigint(int n)
{
	(void)n;
	if (!handle_sigint_part1())
		return ;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	g_signal_pid = -1;
}

void	signals(int heredoc_mode)
{
	struct sigaction	sa;

	sa.sa_handler = &handle_sigint;
	sigemptyset(&sa.sa_mask);
	if (heredoc_mode)
		sa.sa_flags = 0;
	else
		sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
