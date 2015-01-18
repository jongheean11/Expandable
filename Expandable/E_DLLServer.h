#include <json/json.h>

//JSON
#define VALUE	unsigned int
#define SERVERIP	"127.0.0.1"
#define SERVERPORT	8888
#define MAXBUFSIZE	1024

const std::string PING = "PING";
const std::string PONG = "PONG";


enum TYPE{PROGRESSVALUE, PROGRESSSTATE, SHOWWINDOW};

typedef struct _PROCESS{
	unsigned int pid;
	char pname[255];
	unsigned int hwnd;
}PROCESS;

typedef struct _NOTIFICATION_ITEM{
	TYPE type;
	VALUE value;
	PROCESS infomation;
}NOTIFICATION_ITEM;

//JSON KEY STRING

static const char* type_key = "type";
static const char* value_key = "value";
static const char* infomation_key = "infomation";
//information
static const char* pid_key = "pid";
static const char* pname_key = "pname";
static const char* hwnd_key = "hwnd";

void writeJSON(__in NOTIFICATION_ITEM& item, __out std::string& buffer);
void readJSON(__in NOTIFICATION_ITEM& item, __out std::string& buffer);
void readJSON(__out NOTIFICATION_ITEM& item, __in char* buffer);

//╪рдо

bool isConnected();
bool sendData(std::string& buffer);
bool connectExpandable();
bool disconnectExpandable();
int recvData(__out char* buffer);
bool isAvailable();
void init();
void clean();