/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codeCatalog.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 10:32:59 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/20 10:56:33 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef codeCatalog_HPP
#define codeCatalog_HPP

#include "./Libs.hpp"

std::string getStatus(std::string status, map_ss codes);
std::string getTypes(std::string extension, map_ss types);
map_ss setStatus(void);
map_ss setTypes(void);

#endif
