/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_two_part.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abel-had <abel-had@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 12:39:43 by abel-had          #+#    #+#             */
/*   Updated: 2025/07/06 12:39:46 by abel-had         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

static int	find_equal_sign(char *str)
{
	int	i;

	i = 0;
	while (str[i] && str[i] != '=')
		i++;
	if (str[i] != '=')
		return (-1);
	return (i);
}

char	*extract_key(char *str, t_sp_var *va)
{
	int		i;
	int		equal_pos;
	char	*key;

	if (!str)
		return (NULL);
	equal_pos = find_equal_sign(str);
	if (equal_pos == -1)
		return (NULL);
	key = (char *)mmallocc((equal_pos + 1) * sizeof(char),
			&va->allocs, P_GARBAGE);
	if (!key)
		return (NULL);
	i = 0;
	while (i < equal_pos)
	{
		key[i] = str[i];
		i++;
	}
	key[i] = '\0';
	return (key);
}

static char	*create_value_part(char *str, t_sp_var *va)
{
	int		i;
	int		j;
	int		k;
	char	*value;

	i = 0;
	while (str[i] && str[i] != '=')
		i++;
	if (str[i] == '=')
		i++;
	j = i;
	while (str[j])
		j++;
	value = (char *)mmallocc((j - i + 2) * sizeof(char),
			&va->allocs, P_GARBAGE);
	if (!value)
		return (NULL);
	value[0] = '=';
	k = 1;
	while (str[i])
		value[k++] = str[i++];
	value[k] = '\0';
	return (value);
}

char	**two_part_split(char *str, t_sp_var *va)
{
	char	**result;
	char	*key;

	key = extract_key(str, va);
	if (!key)
		return (NULL);
	result = (char **)mmallocc(3 * sizeof(char *),
			&va->allocs, P_GARBAGE);
	if (!result)
		return (NULL);
	result[0] = key;
	result[1] = create_value_part(str, va);
	if (!result[1])
		return (NULL);
	result[2] = NULL;
	return (result);
}
