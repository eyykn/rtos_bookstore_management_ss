// books available in bookstore
#define FOREACH_BOOK(BOOK) \
        BOOK(Matilda_Roald_Dahl)   \
        BOOK(Educated_Tara_Westover)  \
        BOOK(Jaws_Peter_Benchley)   \
        BOOK(Becoming_Michelle_Obama)  \
		BOOK(Ghosted_Rosie_Walsh)  \

#define GET_BOOK_NAME(STRING) #STRING,
#define SERVER_NAME "server_name"
#define MAX_BOOKS 2 // number of books a client orders
#define CLIENTNUM  3 // number of "clients" for the bookstore
#define BOOK_COUNT 5 // number of books available in the bookstore
#define MAX_STRING_LEN    256


// array of books from books available in store
static const char *BOOK_MENU[] = {
	FOREACH_BOOK(GET_BOOK_NAME)
};

// different message types
#define SEND_ORDER_MSG_TYPE (_IO_MAX+200)
#define GET_ORDER_CONF_MSG_TYPE (_IO_MAX+201)

// contents of different message types
typedef struct send_order_msg {
	uint16_t type;
	int order_info[2][10];
} send_order_msg_t;

typedef struct get_order_conf_msg {
	uint16_t type;
	int thread_num;
} get_order_conf_msg_t;
