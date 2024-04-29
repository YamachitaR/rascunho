/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckFile.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryoshio- <ryoshio-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 11:04:35 by ryoshio-          #+#    #+#             */
/*   Updated: 2024/04/27 22:17:31 by ryoshio-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CheckFile_HPP
#define CheckFile_HPP

#include "./Libs.hpp"

class CheckFile
{
    private:
        CheckFile();

        static std::set<std::string> _getValidFirstWords(void);
        static std::set<std::string> _getValidServerWords(void);
        static std::set<std::string> _getValidLocationWords(void);

        static int _isFirstWordInSet(const std::string &text, const std::set<std::string> &wordSet);
        static bool _checkServerParams(std::string element);
        static bool _checkSLocationParams(std::string text);
        static bool _checkServerParamsValue(std::string text);
        static int  _hasDuplicateListenPorts(const std::string& config);

    public:
        static bool check(std::string path);
};

bool checkBraces(const std::string &text);
bool fileExists(char *path);

#endif
