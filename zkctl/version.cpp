/*
 * $Id: version.cpp,v 1.16 2008/04/14 05:25:25 wuk Exp $
 *
 * Description: Get BS version.
 *
 * $Author: wuk $
 * Copyright (c) 2007 by Baidu.com, Incorporation, P.R.C
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "version.h"

/* version */
#ifdef __VERSION_ID__
    /** �汾�� */
    static const char *VERSION_ID = __VERSION_ID__;
#else
    /** �汾�� */
    static const char *VERSION_ID = "unknown";
#endif

/* date of building */
#if defined(__DATE__) && defined(__TIME__)
    /** ����ʱ�� */
    static const char *TIME_BUILT = __DATE__ " " __TIME__;
#else
    /** ����ʱ�� */
    static const char *TIME_BUILT = "unknown";
#endif

/* evn of building */
#ifdef __BUILD_ENV__
    /** ����� */
    static const char *SERVER_BUILT = __BUILD_ENV__;
#else
    /** ����� */
    static const char *SERVER_BUILT = "unkown";
#endif
/* LANGUAGE */
#ifdef __LANGUAGE__
    /** ���� */
    static const char *LANGUAGE = __LANGUAGE__;
#else
    /** ���� */
    static const char *LANGUAGE = "unknown";
#endif



/**
 * @brief ��ӡ�汾��
 *
 * @return  void 
 * @retval   
 * @see 
 * @note 
 * @author huangjindong
 * @date 2012/01/19 22:04:42
**/
void get_version(void)
{
    printf("             Version: %s\n", VERSION_ID);
    printf("          Built date: %s\n", TIME_BUILT);
    printf("       Build machine: %s\n", SERVER_BUILT);
    printf("            Language: %s\n", LANGUAGE);
}

