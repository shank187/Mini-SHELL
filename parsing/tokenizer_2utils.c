/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_2utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdelhamid <abdelhamid@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 10:25:44 by abel-had          #+#    #+#             */
/*   Updated: 2025/07/01 15:01:17 by abdelhamid       ###   ########.fr       */
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

int big_con_part1(t_v *v, t_sp_var *va)
{
	static bool	export;
	char **bib;

	if ((!v->prev_token || !ft_strcmp(v->prev_token->value, "|")) && !ft_strcmp(v->new_buff, "export"))
		export = true;
	if (!ft_strcmp(v->new_buff, "|") || (!v->prev_token && ft_strcmp(v->new_buff, "export")))
		export = false;
	if (va->var->state == UNQUOTED && export && !v->buff && !va->var->wait_more_args)
	{
		bib = two_part_split(v->new_buff, va);
		if (bib != NULL)
		{
			if (bib[0] && !need_expandd(bib[0], &va->var->state) && bib[1] && need_expandd(bib[1], &va->var->state))
			{
				v->expanded_value = expand_env_vars(v->new_buff, va);
				add_expanded_token(v, &va->var->tokens, v->expanded_value, va);
				return (0);
			}
		}
	}
	return (1);
}

void	big_conditions(t_v *v, t_sp_var *va, char **static_buffer)
{
	if (!big_con_part1(v, va))
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
