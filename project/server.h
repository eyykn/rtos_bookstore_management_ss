#define FOREACH_BOOK(BOOK) \
        BOOK(Matilda_Roald_Dahl)   \
        BOOK(Educated_Tara_Westover)  \
        BOOK(Jaws_Peter_Benchley)   \
        BOOK(Becoming_Michelle_Obama)  \
		BOOK(Ghosted_Rosie_Walsh)  \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,
#define SERVER_NAME "server_name"
#define MAX_BOOKS 2
#define BOOK_COUNT 5


enum FRUIT_ENUM {
	FOREACH_BOOK(GENERATE_ENUM)
};

static const char *BOOK_MENU[] = {
	FOREACH_BOOK(GENERATE_STRING)
};

#define SEND_ORDER_MSG_TYPE (_IO_MAX+200)

typedef struct send_order_msg {
	uint16_t type;
	int orderInfo[2][4];
} send_order_msg_t;
