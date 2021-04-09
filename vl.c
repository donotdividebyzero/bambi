#include <stdio.h>
#include "vl/token.h"


int main(int argc, char **argv)
{
	struct TokenStream stream;
	token_stream_push(&stream, "a");
	token_stream_push(&stream, "b");
	token_stream_push(&stream, "\n");
	token_stream_push(&stream, "c");
	token_stream_eat(&stream, EOF);
	token_stream_print(&stream);

	struct TokenStream token;
	token.c = "a";
	token.next = NULL;
	token.type = EOF;
	return 0;
}



