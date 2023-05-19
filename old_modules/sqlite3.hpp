/*
* Copyright (C) 2011, William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __SQLITE3_H__
#define __SQLITE3_H__
#include <stdlib.h>
#include <string>
#include <string.h>
#include "sqlite3.h"

#define SQLITE3_CPP_VERSION_INT 100
#define SQLITE3_CPP_VERSION_CHAR "Sqlite3 C++ Wrapper 1.00"

using namespace std;

class SQLite3_Stmt;

class SQLite3_DB {
	private:
		sqlite3 *db;
	public:
		SQLite3_DB();
		SQLite3_DB(string path);
		SQLite3_DB(char *path);
		void open(string path);
		void open(char *path);
		SQLite3_Stmt *prepare(string statement);
		SQLite3_Stmt *prepare(char *statement);
		void exec(char *statement);
		void exec(string statement);
		const char *version() { return SQLITE_VERSION; }
		void close();
		~SQLite3_DB();
		friend class SQLite3_Stmt;
		friend class SQLite3_Backup;
		friend class SQLite3_Binary;
};

class SQLite3_Backup {
	private:
		sqlite3_backup *b;
	public:
		SQLite3_Backup(SQLite3_DB &to, string destname, SQLite3_DB &from, string fromname);
		SQLite3_Backup(SQLite3_DB &to, char *destname, SQLite3_DB &from, char *fromname);
		int step(int n);
		int remaining();
		int total();
		~SQLite3_Backup();
};

class SQLite3_Stmt {
	private:
		sqlite3_stmt *stmt;
	public:
		SQLite3_Stmt(char *statement, sqlite3 *db);
		SQLite3_Stmt(string statement, sqlite3 *db);
		SQLite3_Stmt(char *statement, SQLite3_DB &db);
		SQLite3_Stmt(string statement, SQLite3_DB &db);
		void reset();
		int next();
		void bind(int col, double d);
		void bind(int col, int i);
		void bind(int col, char *s, int size);
		void bind(int col, string s);
		void bind(int col); // Set Null
		void column(int col, double *d);
		void column(int col, int *i);
		void column(int col, char *s, int size);
		void column(int col, string &s);
		void column_name(int col, string &s);
		void column_name(int col, char *s, int size);
		string column_name(int col);
		int columns();
		~SQLite3_Stmt();
};

class SQLite3_Exception {
	private:
		int code;
		char *get_msg(int c);
	public:
		SQLite3_Exception(const int c);
		int error();
		void msg(string &s);
		string msg();
		~SQLite3_Exception();
};

#endif
