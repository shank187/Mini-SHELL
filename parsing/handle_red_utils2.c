/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_red_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abel-had <abel-had@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 18:25:32 by abel-had          #+#    #+#             */
/*   Updated: 2025/05/27 15:38:50 by abel-had         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

int	read_heredoc_lines(t_sp_var *va)
{
	int	result;

	while (1)
	{
		result = read_heredoc_line(va);
		if (result == -2)
		{
			signals(0);
			return (-2);
		}
		if (result != 0)
			break ;
		expand_heredoc_line(va);
		result = resize_heredoc_buffer(va);
		if (result != 0)
			return (result);
		va->hrv->bib[va->hrv->in++] = va->hrv->line;
	}
	if (result < 0)
		return (result);
	return (0);
}

int	process_heredoc(t_sp_var *va)
{
	int	result;

	result = init_heredoc_buffer(va);
	if (result != 0)
		return (result);
	if (g_signal_pid != -2)
		g_signal_pid = 2;
	else
		return (-2);
	signals(1);
	result = read_heredoc_lines(va);
	signals(0);
	if (result != 0)
		return (result);
	return (heredoc_to_temp_file(va));
}

int	handle_red_if1(t_cmd *cmd, t_sp_var *va)
{
	int	result;

	result = 0;
	(void) cmd;
	if (va->hrv->redir_type == REDIR_HEREDOC)
	{
		if (g_signal_pid != -2)
			result = process_heredoc(va);
		else
			return (10);
	}
	else if (va->hrv->redir_type == REDIR_IN)
	{
		va->hrv->redir->fd = open(va->hrv->redir->file, O_RDONLY);
	}
	if (va->hrv->redir->fd != -11 && va->hrv->redir->fd != -1
		&& va->vpt->current_cmd->am)
		va->hrv->redir->fd = -11;
	return (result);
}

int	handle_red_if2(t_cmd *cmd, t_sp_var *va)
{
	if (va->hrv->redir->fd == -1 || va->hrv->redir->fd == -11)
		return (errno);
	if (!cmd->in)
		cmd->in = va->hrv->redir;
	else
	{
		va->hrv->current = cmd->in;
		while (va->hrv->current->next)
			va->hrv->current = va->hrv->current->next;
		va->hrv->current->next = va->hrv->redir;
	}
	return (0);
}

int	open_outfile(t_sp_var *va)
{
	if (va->hrv->redir_type == REDIR_APPEND && !va->vpt->current_cmd->am)
		va->hrv->redir->fd = open(va->hrv->redir->file, O_WRONLY
				| O_CREAT | O_APPEND, 0644);
	else if (!va->vpt->current_cmd->am)
		va->hrv->redir->fd = open(va->hrv->redir->file, O_WRONLY
				| O_CREAT | O_TRUNC, 0644);
	if (va->hrv->redir->fd != -11 && va->hrv->redir->fd != -1
		&& va->vpt->current_cmd->am)
		va->hrv->redir->fd = -11;
	if (va->hrv->redir->fd == -1)
		return (errno);
	return (0);
}
