/*
 * hbPQdyn - Harbour PostgreSQL API
 * C bindings for libpq - dynamic linking
 *
 * Copyright 2025 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#ifdef _WIN32
#include <windows.h>
#define LIB_HANDLE HMODULE
#define GET_FUNCTION GetProcAddress
#define CLOSE_LIBRARY FreeLibrary
#else
#include <dlfcn.h>
#define LIB_HANDLE void*
#define GET_FUNCTION dlsym
#define CLOSE_LIBRARY dlclose
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

//#include "libpq-fe.h"
#include "pg.h"

typedef int (*ppq_libver_t)();
typedef int	(*ppq_srvver_t)(const PGconn *conn);
typedef PGPing (*ppq_ping_t)(const char *conninfo);
typedef PGconn* (*ppq_connectdb_t)(const char *szConnInfo);
typedef void (*ppq_finish_t)(PGconn *conn);
typedef ConnStatusType (*ppq_status_t)(const PGconn *conn);
typedef PGresult* (*ppq_exec_t)(PGconn *conn, const char *query);
typedef PGresult* (*ppq_execParams_t)(PGconn *conn, const char *command, int nParams,
   const Oid *paramTypes, const char *const *paramValues, const int *paramLengths,
   const int *paramFormats, int resultFormat );
typedef ExecStatusType (*ppq_resultStatus_t)(const PGresult *res);
typedef void (*ppq_clear_t)(PGresult *res);
typedef char * (*ppq_getvalue_t)(const PGresult *res, int tup_num, int field_num);
typedef int	(*ppq_ntuples_t)(const PGresult *res);
typedef int	(*ppq_nfields_t)(const PGresult *res);
typedef char* (*ppq_fname_t)(const PGresult *res, int field_num);

static ppq_libver_t ppq_libver = NULL;
static ppq_srvver_t ppq_srvver = NULL;
static ppq_ping_t ppq_ping = NULL;
static ppq_connectdb_t ppq_connectdb = NULL;
static ppq_finish_t ppq_finish = NULL;
static ppq_status_t ppq_status = NULL;
static ppq_exec_t ppq_exec = NULL;
static ppq_execParams_t ppq_execParams = NULL;
static ppq_resultStatus_t ppq_resultStatus = NULL;
static ppq_clear_t ppq_clear = NULL;
static ppq_getvalue_t ppq_getvalue = NULL;
static ppq_ntuples_t ppq_ntuples = NULL;
static ppq_nfields_t ppq_nfields = NULL;
static ppq_fname_t ppq_fname = NULL;

LIB_HANDLE pDll = NULL;

void c_writelog( const char * sFile, const char * sTraceMsg, ... )
{
   FILE *hFile;

   if( sFile == NULL )
   {
      hFile = fopen( "ac.log", "a" );
   }
   else
   {
      hFile = fopen( sFile, "a" );
   }

   if( hFile )
   {
      va_list ap;

      va_start( ap, sTraceMsg );
      vfprintf( hFile, sTraceMsg, ap );
      va_end( ap );

      fclose( hFile );
   }
}

#ifdef _WIN32

#include <windows.h>

static BOOL AddDirectoryToPath( const char* filePath ) {

   char* lastSlash;
   char* pathOnly = NULL;
   size_t pathLength = 0;
   char* currentPath = NULL;
   char* newPath = NULL;
   BOOL result = FALSE;
   size_t neededSize = 0;

   lastSlash = strrchr( filePath, '\\' );
   if( ( lastSlash = strrchr( filePath, '\\' ) ) == 0 &&
      ( lastSlash = strrchr( filePath, '/' ) ) == 0 )
      return FALSE;

    // Выделяем память для пути без имени файла
    pathLength = lastSlash - filePath;
    pathOnly = (char*)malloc(pathLength + 1);

    // Копируем только путь
    strncpy_s( pathOnly, pathLength + 1, filePath, pathLength );
    pathOnly[pathLength] = '\0';

    // Получаем текущее значение PATH
    neededSize = GetEnvironmentVariableA( "PATH", NULL, 0 );
    if( neededSize == 0 ) {
       // PATH не установлен, создаем новый
       if( SetEnvironmentVariableA("PATH", pathOnly ) )
          result = TRUE;
       free( pathOnly );
       return result;
    }

    // Выделяем память для текущего PATH и получаем текущее значение PATH
    currentPath = (char*) malloc( neededSize );
    GetEnvironmentVariableA( "PATH", currentPath, neededSize );

    // Проверяем, не содержится ли путь уже в PATH
    if( strstr(currentPath, pathOnly) != NULL ) {
       free( pathOnly );
       free( currentPath );
       return TRUE; // Уже есть, считаем успехом
    }

    // Выделяем память для нового PATH (текущий PATH + ; + новый путь)
    size_t newPathSize = strlen(currentPath) + strlen(pathOnly) + 2; // +2 для ';' и '\0'
    newPath = (char*) malloc( newPathSize );

    // Формируем новый PATH
    snprintf( newPath, newPathSize, "%s;%s", currentPath, pathOnly );

    // Устанавливаем новое значение PATH
    if( SetEnvironmentVariableA( "PATH", newPath ) )
        result = TRUE;

    // Освобождаем память
    free( pathOnly );
    free( currentPath );
    free( newPath );

    return result;
}

static void FindAndOpenLib( const char* szDllName ) {

   if( !szDllName ) {
      pDll = LoadLibraryA( "libpq.dll" );
      if( !pDll )
         c_writelog( NULL, "Failed to load libpq.dll\n" );
      return;
   }

   AddDirectoryToPath(  szDllName );
   pDll = LoadLibraryA( szDllName );
}
#else

#include <dirent.h>
#include <fnmatch.h>

#define MAX_PATH_LEN 1024

// Функция для поиска файлов по паттерну в указанной директории
static char* FindlibraryInDir( const char* dir_path, const char* pattern ) {

    DIR* dir = opendir(dir_path);
    if (dir == NULL) {
        return NULL;
    }

    struct dirent* entry;
    char* found_path = NULL;

    while ((entry = readdir(dir)) != NULL) {
        if (fnmatch(pattern, entry->d_name, 0) == 0) {
            // Нашли подходящий файл
            found_path = malloc(MAX_PATH_LEN);
            snprintf(found_path, MAX_PATH_LEN, "%s/%s", dir_path, entry->d_name);
            break;
        }
    }

    closedir(dir);
    return found_path;
}

static void FindAndOpenLib( const char* szDllName ) {

   if( szDllName ) {
      pDll = dlopen( szDllName, RTLD_LAZY | RTLD_GLOBAL );
      if( !pDll )
         c_writelog( NULL, "Failed to load %s\n", szDllName );
      return;
   }

   // Пробуем основные варианты
   const char* try_names[] = {
      "libpq.so",      // Основное имя
      "libpq.so.5",    // Наиболее распространенная версия
      "libpq.so.6",    // Новые версии
      NULL
   };

   for (int i = 0; try_names[i] != NULL; i++) {
      pDll = dlopen( try_names[i], RTLD_LAZY | RTLD_GLOBAL );
      if( pDll )
         return;
   }

   // Если простой способ не сработал, используем расширенный поиск
   const char* search_paths[] = {
      "/usr/lib",
      "/usr/lib64",
      "/usr/local/lib",
      "/usr/local/lib64",
      "/lib",
      "/lib64",
      "/usr/lib/x86_64-linux-gnu",  // Для Debian/Ubuntu
      "/usr/lib/aarch64-linux-gnu", // Для ARM64
      NULL
   };

   const char* patterns[] = {
      "libpq.so.*",      // Основной паттерн
      "libpq-*.so",      // Альтернативные именования
      NULL
   };

   // Ищем в стандартных путях
   for (int i = 0; search_paths[i] != NULL; i++) {
      for (int j = 0; patterns[j] != NULL; j++) {
         char* lib_path = FindlibraryInDir(search_paths[i], patterns[j]);
         if (lib_path != NULL) {
            pDll = dlopen( lib_path, RTLD_LAZY | RTLD_GLOBAL );
            free(lib_path);
            if( pDll )
               return;
         }
      }
   }

   // Также проверяем пути из ld.so.conf
   FILE* ldconfig = popen("/sbin/ldconfig -p", "r");
   if (ldconfig != NULL) {
      char line[256];
      while (fgets(line, sizeof(line), ldconfig) != NULL) {
         if (strstr(line, "libpq.so") != NULL) {
            // Парсим путь из вывода ldconfig
            char* path_start = strchr(line, '>');
            if (path_start != NULL) {
                path_start += 2; // Пропускаем "> "
                char* path_end = strchr(path_start, ' ');
                if (path_end != NULL) {
                   *path_end = '\0';

                   pDll = dlopen(path_start, RTLD_LAZY | RTLD_GLOBAL);
                   if( pDll ) {
                      pclose(ldconfig);
                      return;
                   }
                }
             }
          }
      }
      pclose(ldconfig);
   }
}
#endif

int pq_Init( const char* szDllName ) {

   FindAndOpenLib( szDllName );
   if (!pDll ) {
      c_writelog( NULL, "Failed to load library\n" );
      return 1;
   }

   if( !ppq_connectdb ) {
      ppq_connectdb = (ppq_connectdb_t)GET_FUNCTION( pDll, "PQconnectdb" );
      if( !ppq_connectdb ) {
         c_writelog( NULL, "Failed to get PQconnectdb\n" );
         return 2;
      }
   }
   if( !ppq_finish ) {
      ppq_finish = (ppq_finish_t)GET_FUNCTION( pDll, "PQfinish" );
      if( !ppq_finish ) {
         c_writelog( NULL, "Failed to get PQfinish\n" );
         return 2;
      }
   }
   if( !ppq_exec ) {
      ppq_exec = (ppq_exec_t)GET_FUNCTION( pDll, "PQexec" );
      if( !ppq_exec ) {
         c_writelog( NULL, "Failed to get PQexec\n" );
         return 2;
      }
   }
   if( !ppq_execParams ) {
      ppq_execParams = (ppq_execParams_t)GET_FUNCTION( pDll, "PQexecParams" );
      if( !ppq_exec ) {
         c_writelog( NULL, "Failed to get PQexecParams\n" );
         return 2;
      }
   }
   if( !ppq_status ) {
      ppq_status = (ppq_status_t)GET_FUNCTION( pDll, "PQstatus" );
      if( !ppq_status ) {
         c_writelog( NULL, "Failed to get PQstatus\n" );
         return -1;
      }
   }
   if( !ppq_resultStatus ) {
      ppq_resultStatus = (ppq_resultStatus_t)GET_FUNCTION( pDll, "PQresultStatus" );
      if( !ppq_resultStatus ) {
         c_writelog( NULL, "Failed to get PQresultStatus\n" );
         return -1;
      }
   }

   return 0;
}

void pq_Exit() {

   if( pDll )
      CLOSE_LIBRARY( pDll );
   pDll = NULL;
}

int pq_libVersion() {

   if( !pDll )
      return -1;
   if( !ppq_libver ) {
      ppq_libver = (ppq_libver_t)GET_FUNCTION( pDll, "PQlibVersion" );
      if( !ppq_libver ) {
         c_writelog( NULL, "Failed to get PQlibVersion\n" );
         return -1;
      }
   }
   return ppq_libver();
}

int pq_srvVersion( const PGconn * conn ) {

   if( !pDll )
      return -1;
   if( !ppq_srvver ) {
      ppq_srvver = (ppq_srvver_t)GET_FUNCTION( pDll, "PQserverVersion" );
      if( !ppq_srvver ) {
         c_writelog( NULL, "Failed to get PQserverVersion\n" );
         return -1;
      }
   }
   return ppq_srvver( conn );
}

PGPing pq_ping( const char * szConnInfo ) {

   if( !pDll )
      return -1;
   if( !ppq_ping ) {
      ppq_ping = (ppq_ping_t)GET_FUNCTION( pDll, "PQping" );
      if( !ppq_ping ) {
         c_writelog( NULL, "Failed to get PQping\n" );
         return -1;
      }
   }
   return ppq_ping( szConnInfo );
}

PGconn * pq_connectDb( const char * szConnInfo ) {

   if( !pDll )
      return NULL;
   return ppq_connectdb( szConnInfo );
}

ConnStatusType pq_status( const PGconn *conn ) {

   if( !pDll )
      return CONNECTION_BAD;
   return ppq_status( conn );
}

void pq_finish( PGconn *conn ) {

   if( !pDll )
      return;
   ppq_finish( conn );
}

PGresult* pq_exec( PGconn *conn, const char *query ) {

   if( !pDll )
      return NULL;
   return ppq_exec( conn, query );
}

PGresult* pq_execParams( PGconn *conn, const char *command, int nParams,
   const Oid *paramTypes, const char *const *paramValues, const int *paramLengths,
   const int *paramFormats, int resultFormat ) {

   if( !pDll )
      return NULL;
   return ppq_execParams( conn, command, nParams, paramTypes, paramValues,
      paramLengths, paramFormats, resultFormat );
}

ExecStatusType pq_resultStatus( const PGresult *res ) {

   if( !pDll )
      return PGRES_FATAL_ERROR;
   return ppq_resultStatus( res );
}

void pq_clear( PGresult *res ) {

   if( !pDll )
      return;
   ppq_clear( res );
}

char* pq_getvalue( const PGresult *res, int tup_num, int field_num ) {

   if( !pDll )
      return NULL;
   if( !ppq_getvalue ) {
      ppq_getvalue = (ppq_getvalue_t)GET_FUNCTION( pDll, "PQgetvalue" );
      if( !ppq_getvalue ) {
         c_writelog( NULL, "Failed to get PQgetvalue\n" );
         return NULL;
      }
   }
   return ppq_getvalue( res, tup_num, field_num );
}

int pq_ntuples( PGresult *res ) {

   if( !pDll )
      return -1;
   if( !ppq_ntuples ) {
      ppq_ntuples = (ppq_ntuples_t)GET_FUNCTION( pDll, "PQntuples" );
      if( !ppq_ntuples ) {
         c_writelog( NULL, "Failed to get PQntuples\n" );
         return -1;
      }
   }

   return ppq_ntuples( res );
}

int pq_nfields( PGresult *res ) {

   if( !pDll )
      return -1;
   if( !ppq_nfields ) {
      ppq_nfields = (ppq_nfields_t)GET_FUNCTION( pDll, "PQnfields" );
      if( !ppq_ntuples ) {
         c_writelog( NULL, "Failed to get PQnfields\n" );
         return -1;
      }
   }

   return ppq_nfields( res );
}

char * pq_fname( PGresult *res, int field_num ) {

   if( !pDll )
      return NULL;
   if( !ppq_fname ) {
      ppq_fname = (ppq_fname_t)GET_FUNCTION( pDll, "PQfname" );
      if( !ppq_fname ) {
         c_writelog( NULL, "Failed to get PQfname\n" );
         return NULL;
      }
   }

   return ppq_fname( res, field_num );
}