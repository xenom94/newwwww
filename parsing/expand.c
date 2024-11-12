/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stakhtou <stakhtou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 17:05:45 by stakhtou          #+#    #+#             */
/*   Updated: 2024/10/21 16:37:16 by stakhtou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char	*process_quotes(t_expansion *exp)
{
	if (*exp->temp == '"')
	{
		if (exp->in_single_quote == 0)
			exp->in_double_quote = !exp->in_double_quote;
		exp->new_result = ft_strjoin_char(exp->result, *exp->temp);
	}
	else if (*exp->temp == '\'')
	{
		if (exp->in_double_quote == 0)
			exp->in_single_quote = !exp->in_single_quote;
		exp->new_result = ft_strjoin_char(exp->result, *exp->temp);
	}
	else
		exp->new_result = ft_strjoin_char(exp->result, *exp->temp);
	free(exp->result);
	exp->result = exp->new_result;
	exp->temp++;
	return (exp->result);
}

char	*get_env_value(char *name, char **env)
{
	int		i;
	size_t	name_len;

	i = 0;
	name_len = ft_strlen(name);
	if (!name || !env)
		return (NULL);
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
		{
			printf("pwd: %s\n", env[i] + name_len + 1);
			return (env[i] + name_len + 1);
		}
		i++;
	}
	return (NULL);
}

char	*expand_env_variable(t_expansion *exp, char **env)
{
	exp->before_env = ft_substr(exp->temp, 0, exp->env_pos - exp->temp);
	exp->new_result = ft_strjoin(exp->result, exp->before_env);
	free(exp->result);
	free(exp->before_env);
	exp->result = exp->new_result;
	exp->env_len = 0;
	while (exp->env_pos[1 + exp->env_len] && (ft_isalnum(exp->env_pos[1
					+ exp->env_len]) || exp->env_pos[1 + exp->env_len] == '_'))
		exp->env_len++;
	exp->env_name = ft_substr(exp->env_pos + 1, 0, exp->env_len);
	exp->env_value = get_env_value(exp->env_name, env);
	free(exp->env_name);
	if (exp->env_value)
	{
		exp->new_result = ft_strjoin(exp->result, exp->env_value);
		free(exp->result);
		exp->result = exp->new_result;
	}
	exp->temp = exp->env_pos + 1 + exp->env_len;
	return (exp->result);
}

void	initialize_expansion(t_expansion *exp, const char *str)
{
	exp->result = ft_strdup("");
	exp->in_single_quote = 0;
	exp->in_double_quote = 0;
	exp->temp = (char *)str;
}

char	*expand_variables(const char *str)
{
	t_expansion	exp;

	initialize_expansion(&exp, str);
	while (*exp.temp)
	{
		if (*exp.temp == '"' || *exp.temp == '\'')
		{
			exp.result = process_quotes(&exp);
			continue ;
		}
		exp.env_pos = ft_strchr(exp.temp, '$');
		if (!exp.in_single_quote && exp.env_pos && exp.env_pos[1] != '\0')
			exp.result = expand_env_variable(&exp, g_vars.env);
		else
		{
			exp.new_result = ft_strjoin_char(exp.result, *exp.temp);
			free(exp.result);
			exp.result = exp.new_result;
			exp.temp++;
		}
	}
	exp.final_result = ft_strdup(exp.result);
	exp.unquoted_result = remove_quotes(exp.final_result);
	return (free(exp.final_result), free(exp.result), exp.unquoted_result);
}
