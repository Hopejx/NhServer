#pragma once
//server
enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
//消息头
struct DataHeader {
	short dataLength;// 数据长度
	short cmd;//命令
};
//登入
struct LogIn : public DataHeader {
	LogIn() {
		dataLength = sizeof(LogIn);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
};
struct LogInResult : public DataHeader {
	LogInResult() {
		dataLength = sizeof(LogInResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
//登出
struct LogOut : public DataHeader {
	LogOut() {
		dataLength = sizeof(LogOut);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogOutResult : public DataHeader {
	LogOutResult() {
		dataLength = sizeof(LogOutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin : public DataHeader {
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		socket_id = 0;
	}
	int socket_id;
};


