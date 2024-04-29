/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   template.tpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 09:24:34 by ryoshio-          #+#    #+#             */
/*   Updated: 2024/04/22 23:16:51 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef FT_TO_STRING_TPP
#define FT_TO_STRING_TPP

#include "./Libs.hpp"

template <typename T>

std::string to_string(const T &value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

#endif