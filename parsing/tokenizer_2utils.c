/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_2utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abel-had <abel-had@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 12:38:33 by abel-had          #+#    #+#             */
/*   Updated: 2025/07/06 12:38:38 by abel-had         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

int	add_token_with_type_fill(t_v *v, t_sp_var *va, char **static_buffer)
{
	if (*static_buffer)
	{
		v->buff = *static_buffer;
		*static_buffer = NULL;
	}
	if (!prepare_token(va, v))
		return (0);
	if (v->buff && ft_strlen(v->buff) >= 1
		&& v->buff[ft_strlen(v->buff) - 1] == '$'
		&& va->previous_state == UNQUOTED)
		v->buff[ft_strlen(v->buff) - 1] = '\0';
	return (1);
}

static void	set_export_flags(t_v *v, t_sp_var *va)
{
	static bool	first_cmd;
	static bool	a;

	reset_export_flags(v, va, &a, &first_cmd);
	if (v->new_token && v->new_token->type == text
		&& !a && ft_strcmp(v->new_buff, ""))
		first_cmd = true;
	if (v->prev_token && v->prev_token->type == red)
		first_cmd = false;
	if (first_cmd)
		a = true;
	if (((!v->prev_token || !ft_strcmp(v->prev_token->value, "|"))
			&& !ft_strcmp(v->new_buff, "export"))
		|| (first_cmd && !ft_strcmp(v->new_buff, "export")))
		va->export_flag = true;
	if (!v->buff)
		va->sp_flag = false;
	if (!v->buff && va->var->state == UNQUOTED
		&& ft_strlen(v->new_buff) >= 2
		&& v->new_buff[ft_strlen(v->new_buff) - 1] == '='
		&& va->export_flag && va->var->wait_more_args
		&& !need_expandd(v->new_buff, &va->var->state))
		va->sp_flag = true;
	first_cmd = false;
}

int	big_con_part1(t_v *v, t_sp_var *va, char **static_buffer)
{
	set_export_flags(v, va);
	if (handle_export_expansion(v, va, static_buffer) == 0)
		return (0);
	if (handle_buffered_expansion(v, va, static_buffer) == 0)
		return (0);
	return (1);
}

void	big_conditions(t_v *v, t_sp_var *va, char **static_buffer)
{
	if (!big_con_part1(v, va, static_buffer))
		return ;
	if (v->buff)
	{
		p_with_buffer(v, va, static_buffer);
	}
	else
	{
		p_without_buffer(v, va, static_buffer);
	}
}

int	add_token_with_type(t_sp_var *va)
{
	static char	*static_buffer = NULL;
	static bool	quote = false;
	static bool	ambiguous_red = false;
	static bool	ambiguous = false;
	t_v			*v;

	quote = va->st_quote;
	ambiguous = va->st_ambiguous;
	ambiguous_red = va->st_ambiguous_red;
	v = init_token_vars(va, &quote);
	if (!ambiguous_red)
		ambiguous_red = ft_ambiguous_red(v);
	v->ambiguous = ambiguous;
	if (!v)
		return (0);
	if (!add_token_with_type_fill(v, va, &static_buffer))
		return (0);
	va->previous_state = va->var->state;
	if (!ft_ambiguous_red(v))
		ambiguous_red = false;
	va->st_ambiguous = ambiguous;
	va->st_ambiguous_red = ambiguous_red;
	va->st_quote = quote;
	big_conditions(v, va, &static_buffer);
	return (1);
}
