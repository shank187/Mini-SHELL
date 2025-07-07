/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_5utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abel-had <abel-had@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 12:38:50 by abel-had          #+#    #+#             */
/*   Updated: 2025/07/06 12:38:53 by abel-had         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

int	handle_buffered_expansion(t_v *v, t_sp_var *va, char **static_buffer)
{
	if (va->var->state == UNQUOTED && va->export_flag && v->buff
		&& va->sp_flag && need_expandd(v->new_buff, &va->var->state))
	{
		v->expanded_value = expand_env_vars(v->new_buff, va);
		if (va->var->wait_more_args)
			*static_buffer = ft_strjoin(v->buff, v->expanded_value,
					&va->allocs);
		else
		{
			*static_buffer = ft_strjoin(v->buff, v->expanded_value,
					&va->allocs);
			add_expanded_token(v, &va->var->tokens, *static_buffer, va);
			*static_buffer = NULL;
		}
		return (0);
	}
	return (1);
}

int	handle_export_expansion(t_v *v, t_sp_var *va, char **static_buffer)
{
	char	**bib;

	if (va->var->state == UNQUOTED && va->export_flag && !v->buff)
	{
		bib = two_part_split(v->new_buff, va);
		if (bib != NULL)
		{
			if (bib[0] && !need_expandd(bib[0], &va->var->state)
				&& bib[1] && need_expandd(bib[1], &va->var->state))
			{
				v->expanded_value = expand_env_vars(v->new_buff, va);
				if (!va->var->wait_more_args)
					add_expanded_token(v, &va->var->tokens,
						v->expanded_value, va);
				else
					*static_buffer = ft_strdup(v->expanded_value,
							&va->allocs, P_GARBAGE);
				return (0);
			}
		}
	}
	return (1);
}

void	reset_export_flags(t_v *v, t_sp_var *va, bool *a, bool *first_cmd)
{
	if (!ft_strcmp(v->new_buff, "|")
		|| (!v->prev_token && ft_strcmp(v->new_buff, "export")))
	{
		va->export_flag = false;
		*a = false;
		*first_cmd = false;
	}
}
