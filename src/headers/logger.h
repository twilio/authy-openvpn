//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
// logger.h
//
// Abstract
// 
// Wraps a logger object based of levels of openvpn logs 
//
// defaults to 4
// 
//
//
// History
//
//  8/1/2013    dpalacio    Created
//
//------------------------------------------------------------------------------

#ifndef __LOGGER_H__
#define __LOGGER_H__ 1


#define INFO  4 // OpenVPN regular debug level 
#define ERROR 3
#define DEBUG 5

void trace(const int level, const int line, const char *msg, ...);

#endif
