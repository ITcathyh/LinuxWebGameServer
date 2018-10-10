#pragma once

#include <iostream>  
#include <cstdlib>  
#include "mysql.h"  
#include <cstring>
#include <string>
#include <sstream>

using namespace std;

#define USERNAME "root"
#define PASSWORD "root"
#define DATABASE_NAME "mygame"

MYSQL * connectMysql()
{
	MYSQL *conn = mysql_init(NULL);  
  
	if (!conn)  
	{  
		return NULL;
	}  
  
	conn = mysql_real_connect(conn, "localhost", USERNAME, PASSWORD, DATABASE_NAME, 0, NULL, 0);  
  
	if (!conn)
	{
		conn = NULL;
	}
	
	return conn;
}

void connectMysql(MYSQL *conn)
{
	cout << "begin" << endl;
	conn = mysql_init(NULL);  
	
	cout << conn << endl;
  
	if (!conn)  
	{  
		conn =  NULL;
		return;
	}  
	
	conn = mysql_real_connect(conn, "localhost", USERNAME, PASSWORD, DATABASE_NAME, 0, NULL, 0);
	
	cout << conn << endl;
	cout << "end" << endl;
}

MYSQL_RES *query(char *sql)
{
	MYSQL_RES *res;  
	MYSQL *conn = connectMysql();
	
	if (!conn)  
	{  
		return NULL;
	}  
	
	if (!mysql_query(conn, sql))
	{
		res = mysql_store_result(conn);   
		mysql_close(conn);
		
		if (res) {           
			return res;
		} 	
	}        
	
	mysql_close(conn);
	return NULL;
}

int insert(char *sql)
{	  
	MYSQL *conn = connectMysql();    
	
	if (!conn)  
	{  
		return NULL;
	}  
	
	return mysql_query(conn, sql);
}

string getLongStr(long num)
{
	string str;
	stringstream stream;
	stream << num;
	stream >> str;
	
	return str;
}