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

enum FRUIT_ENUM {
	FOREACH_BOOK(GENERATE_ENUM)
};

static const char *BOOK_MENU[] = {
	FOREACH_BOOK(GENERATE_STRING)
};
