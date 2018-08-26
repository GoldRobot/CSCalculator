#pragma once
#include <string>
#define MESSAGE_DELIMITER "\r\n"
#define MESSAGE_TO_SERVER_LOGIN "login "
#define MESSAGE_TO_SERVER_LOGOUT "logout "
#define MESSAGE_TO_SERVER_PASSWORD "password "
#define MESSAGE_TO_SERVER_CALC "calc "
#define MESSAGE_TO_SERVER_RESULT "result "

#define MESSAGE_TO_CLIENT_RESULT "result "
#define MESSAGE_TO_CLIENT_ERR "error "
#define MESSAGE_TO_CLIENT_TIMEOUT "timeout "
#define MESSAGE_TO_CLIENT_NO_MONEY "no money "
#define MESSAGE_TO_CLIENT_BUSY "busy "
#define MESSAGE_TO_CLIENT_RIGHT "right "
#define MESSAGE_TO_CLIENT_WRONG "wrong "

enum CONNECTION_STATES
{
	CONNECTION_STATE_CONNECTED,
	CONNECTION_STATE_AURIZATION_LOGIN,
	CONNECTION_STATE_AUTORIZED,
	CONNECTION_STATE_DISCONNECTED
};

struct TYPE_OF_MESSAGES
{
	std::string DELIMITER = MESSAGE_DELIMITER;
	size_t DELIMITER_SIZE = std::string(MESSAGE_DELIMITER).size();
	struct to_server
	{
		struct text
		{
			std::string LOGIN			= MESSAGE_TO_SERVER_LOGIN;
			std::string LOGOUT			= MESSAGE_TO_SERVER_LOGOUT;
			std::string PASSWORD		= MESSAGE_TO_SERVER_PASSWORD;
			std::string CALC			= MESSAGE_TO_SERVER_CALC;
			std::string RESULT			= MESSAGE_TO_SERVER_RESULT;
		}TEXT;
		struct size
		{
			size_t LOGIN				= std::string(MESSAGE_TO_SERVER_LOGIN).size();
			size_t LOGOUT				= std::string(MESSAGE_TO_SERVER_LOGOUT).size();
			size_t PASSWORD				= std::string(MESSAGE_TO_SERVER_PASSWORD).size();
			size_t CALC					= std::string(MESSAGE_TO_SERVER_CALC).size();
			size_t RESULT				= std::string(MESSAGE_TO_SERVER_RESULT).size();
		}SIZE;
		struct code
		{
			enum
			{
				UNKNOWN,
				LOGIN,
				LOGOUT,
				PASSWORD,
				CALC,
				RESULT
			};
		}CODE;
	} TO_SERVER;
	struct to_clent
	{
		struct text
		{
			std::string RESULT		= MESSAGE_TO_CLIENT_RESULT;
			std::string ERR			= MESSAGE_TO_CLIENT_ERR;
			std::string TIMEOUT		= MESSAGE_TO_CLIENT_TIMEOUT;
			std::string NO_MONEY	= MESSAGE_TO_CLIENT_NO_MONEY;
			std::string BUSY		= MESSAGE_TO_CLIENT_BUSY;
			std::string RIGHT		= MESSAGE_TO_CLIENT_RIGHT;
			std::string WRONG		= MESSAGE_TO_CLIENT_WRONG;
		}TEXT;
		struct size
		{
			size_t RESULT			= std::string(MESSAGE_TO_CLIENT_RESULT).size();
			size_t ERR				= std::string(MESSAGE_TO_CLIENT_ERR).size();
			size_t TIMEOUT			= std::string(MESSAGE_TO_CLIENT_TIMEOUT).size();
			size_t NO_MONEY			= std::string(MESSAGE_TO_CLIENT_NO_MONEY).size();
			size_t BUSY				= std::string(MESSAGE_TO_CLIENT_BUSY).size();
			size_t RIGHT			= std::string(MESSAGE_TO_CLIENT_RIGHT).size();
			size_t WRONG			= std::string(MESSAGE_TO_CLIENT_WRONG).size();
		}SIZE;
		struct code
		{
			enum
			{
				UNKNOWN,
				RESULT,
				ERR,
				TIMEOUT,
				NO_MONEY,
				BUSY,
				RIGHT,
				WRONG
			};
		}CODE;
	} TO_CLIENT;
} MESSAGE_TYPE;