// Dependencies
#include "mongoose.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "HttpMessageHandler.h"
#include "Globals.h"
#include <sstream>

// Configuration properties
#define CREST2_VERSION "v0.4.3"
#define POLL_TIME_IN_MILLIS 17	
// fossa definition: maximum number of milliseconds to sleep for ns_mgr_poll/ fossa calls the winsock select() function with it, where it is a timeout:
// The select function returns the total number of socket handles that are ready and contained in the fd_set structures, zero if the time limit expired.
// It is the maximum time for select to wait
#define ESC_KEY 27
#define CREST_API_URL "/crest2/v1/api"

// Constants
#define HTTP_RESPONSE_404 "{\r\n  \"status\": \"404 Not found, please use the correct URL: " CREST_API_URL "\"\r\n}"

// Server variables
static const char *s_http_port_default = "8180";
static struct mg_serve_http_opts s_http_server_opts;

// Variables for integrity checks, definition in Globals.h
SharedMemory* localCopy;
SharedMemory* localCopyTmp;
unsigned int updateIndex;
unsigned int indexChange;
// Debug Mode
int debug_level = 0;

// Response generator
static HttpMessageHandler httpMessageHandler = HttpMessageHandler();

// Server request handler method
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		// Only handle HTTP requests on the API url
		if (mg_vcmp(&hm->uri, CREST_API_URL) == 0) {
			httpMessageHandler.handle(nc, hm);
		}else{
			// Unknown URI, return a 404
			mg_printf(nc, "HTTP/1.1 404 Not found\r\n"
				"Content-Type: application/json\r\n"
				"Cache-Control: no-cache\r\n"
                "Access-Control-Allow-Origin: *\r\n"
				"Content-Length: %d\r\n\r\n%s",
				(int)strlen(HTTP_RESPONSE_404), HTTP_RESPONSE_404);
		}
		break;
	default:
		break;
	}
}

int main()	{

	// Handling of command line arguments
	LPWSTR* szArgList;
	int argCount;
	int port = -1;
	int PollTimeout = -1;
	bool help = false;
	
	szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);

	for (int i = 1; i < argCount; i++)
	{
		//printf("%d: %ws\n", i, szArgList[i]);
		if (i + 1 != argCount)
		{
			if (wcscmp(szArgList[i], L"-p") == 0 )
			{
				port = _wtoi(szArgList[i + 1]);
				
			}
			else if (wcscmp(szArgList[i], L"-t") == 0)
			{
				PollTimeout = _wtoi(szArgList[i + 1]);
			}
		}
		if (wcscmp(szArgList[i], L"-d") == 0)
		{
			debug_level = 1;
		}
		if (wcscmp(szArgList[i], L"-dd") == 0)
		{
			debug_level = 2;
		}
		if (wcscmp(szArgList[i], L"-h") == 0)
		{
			help = true;
		}
	}

	LocalFree(szArgList);
	// End Handling of command line arguments

	if (help) {
		printf("# CREST2 - CARS2 REST API %s\n", CREST2_VERSION);
		printf("# (c) 2015 Lars Rosenquist\n");
		printf("#          updated by Viper\n\n");
		printf("Usage:\n");
		printf("CREST2.exe [-p 8180] [-t 17] [-d[d]] [-h]\n");
		printf("  -p	TCP Port, default: 8180, allowed: 1025-65534\n");
		printf("  -t	Websocket Timeout im ms, default: 17, allowed: 1-10000\n");
		printf("  -d	Debug Level 1, show some more info for Integrity Checks\n");
		printf("  -dd	Debug Level 2, show more info for Integrity Checks\n");
		printf("  -h	Help\n");
		return 0;
	}

	//convert port from int to char*
	std::string s = std::to_string(port);
	static const char* s_http_port;

	// set HTTP Port
	if (port >= 1025 && port <= 65534) {
		s_http_port = s.c_str();
	}
	else {
		s_http_port = s_http_port_default;
	}

	// set websocket timeout for winsock select function
	if (PollTimeout < 1 || PollTimeout > 10000) {
		PollTimeout = POLL_TIME_IN_MILLIS;
	}

	// Setup the server
	struct mg_mgr mgr;
	struct mg_connection* nc;
	mg_mgr_init(&mgr, NULL);
	nc = mg_bind(&mgr, s_http_port, ev_handler);

	// catch error if TCP port is already in use
	if (nc == NULL) {
		printf("\n# The port %s is already in use by another application.\n# Please change it with option \"-p\" or close the other application.\n\n Press any key to exit...\n", s_http_port);
		// wait for user input. Without it and if the application is executed by double click or via shortcut where it opens a new window, it will close directly and the user will not see the error.
		while (true) {
			if (_kbhit()) {
				return 1;
			}
		}	
	}

	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = ".";

	// initialize globals
	localCopy = new SharedMemory;
	localCopyTmp = new SharedMemory;
	updateIndex = 0;
	indexChange = 0;

	// Print some information on the console
	printf("# CREST2 - CARS2 REST API %s\n", CREST2_VERSION);
	printf("# (c) 2015 Lars Rosenquist\n");
	printf("#          updated by Viper\n\n");
	printf("# Server started on TCP port: %s\n", s_http_port);
	printf("# Websocket Timeout: %i ms\n", PollTimeout);
	if (debug_level > 0) { printf("# Debug Level: %i\n", debug_level); }
	printf("# API is available at http://localhost:%s%s \n", s_http_port, CREST_API_URL);
	printf("# Press ESC to terminate\n");

	// Keep polling until ESC is hit
	while (true) {

		mg_mgr_poll(&mgr, PollTimeout);

		if (_kbhit() && _getch() == ESC_KEY) {
			break;
		}
	}

	// We're done, free up the server and exit
	mg_mgr_free(&mgr);
	delete localCopy;
	delete localCopyTmp;

	return 0;
}
