/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsmit <lsmit@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/03 21:37:01 by lsmit         #+#    #+#                 */
/*   Updated: 2022/05/11 18:37:25 by lsmit         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

unsigned long	get_time(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return ((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000));
}

void	ft_putnbr(unsigned long n)
{
	char	print[1];

	if (n >= 10)
		ft_putnbr(n / 10);
	*print = n % 10 + 48;
	printf(print, 1);
}

int	ft_atoi(char *str)
{
	int			i;
	long long	res;
	long long	n;

	i = 0;
	n = 1;
	res = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-')
		n = -1;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if ((res * 10 + (str[i] - '0')) < res)
			return (((-1 * n) - 1) / 2);
		res = res * 10 + str[i] - '0';
		i++;
	}
	return ((int)res * n);
}

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);
	else
		return (0);
}

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}
