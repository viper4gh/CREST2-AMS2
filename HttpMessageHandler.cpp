// Dependencies
#include "HttpMessageHandler.h"
#include "MongooseUtils.h"
#include "SharedMemoryRenderer.h"
#include "Utils.h"
#include "Globals.h"
#include "mongoose.h"
#include <sstream>
#include "time.h"

// Constants
#define MAP_OBJECT_NAME "$pcars2$"
#define HTTP_RESPONSE_503 "{\r\n  \"status\": \"503 Service unavailable, is AMS2 running and is Shared Memory enabled?\"\r\n}"
#define HTTP_RESPONSE_409 "{\r\n  \"status\": \"409 Conflict, are CREST2 and AMS2 both at the latest version?\"\r\n}"
#define GZIP_THRESHOLD 128

static SharedMemoryRenderer sharedMemoryRenderer = SharedMemoryRenderer();

HttpMessageHandler::HttpMessageHandler(){};

// Outputs an HTTP 503 on the supplied connection
void sendServiceUnavailable(struct mg_connection *nc)    {
    // Send HTTP 503
	mg_printf(nc, "HTTP/1.1 503 Service unavailable\r\n"
		"Content-Type: application/json\r\n"
		"Cache-Control: no-cache\r\n"
        "Access-Control-Allow-Origin: *\r\n"
		"Content-Length: %d\r\n\r\n%s",
		(int)strlen(HTTP_RESPONSE_503), HTTP_RESPONSE_503);
}

// Outputs an HTTP 409 on the supplied connection
void sendConflict(struct mg_connection *nc)    {
	// Send HTTP 409
	mg_printf(nc, "HTTP/1.1 409 Conflict\r\n"
		"Content-Type: application/json\r\n"
		"Cache-Control: no-cache\r\n"
        "Access-Control-Allow-Origin: *\r\n"
		"Content-Length: %d\r\n\r\n%s",
		(int)strlen(HTTP_RESPONSE_409), HTTP_RESPONSE_409);
}

// Outputs an HTTP 200 on the supplied connection for an OPTIONS request
void sendOptions(struct mg_connection *nc)    {
    // Send HTTP 200
    mg_printf(nc, "HTTP/1.1 200 Ok\r\n"
              "Access-Control-Allow-Origin: *\r\n"
              "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
              "Access-Control-Max-Age: 86400\r\n"
              "Content-Length: 0\r\n");
    
}

// Extracts the query string from the given HTTP message
std::string getQueryString(struct http_message *hm)	{
	if (hm->query_string.len > 0)	{
		std::string queryString (hm->query_string.p, hm->query_string.len);
		return queryString;
	}else{
		return "";
	}
}

// Extracts the request method from the given HTTP message
std::string getMethod(struct http_message *hm)	{
    if (hm->method.len > 0)	{
        std::string requestMethod (hm->method.p, hm->method.len);
        return requestMethod;
    }else{
        return "";
    }
}

// Returns true if the response to the given HTTP message should
// be gzipped, based on the value of the Accept-Encoding header
// and the size of the uncompressed response
bool shouldGzipResponse(struct http_message *hm, int responseLength)	{
	return Utils::contains(FossaUtils::getHeaderValue("Accept-Encoding", hm), "gzip") && responseLength > GZIP_THRESHOLD;
}

// Renders the response
void renderResponse(struct mg_connection *nc, const SharedMemory* sharedData, struct http_message *hm)  {
	
	// get current time for debugging info
	char sTime[100];
	time_t TimeNow = time(0);
	strftime(sTime, 100, "%H:%M:%S", localtime(&TimeNow));

	// Odd sequence number indicates, that write into the shared memory is just happening, go on with even sequence number only
	if (sharedData->mSequenceNumber % 2)
	{
		// activate it for a kind of debugging mode
		if (debug_level > 0) {
			printf("%s: INFO - Odd sequence number detected - Data not accessable during write process by game\n",sTime);
		}
	}
	else {
		
		indexChange = sharedData->mSequenceNumber - updateIndex;
		updateIndex = sharedData->mSequenceNumber;

		//Copy the whole structure before processing it, otherwise the risk of the game writing into it during processing is too high.
		memcpy(localCopyTmp, sharedData, sizeof(SharedMemory));

		if (localCopyTmp->mSequenceNumber != updateIndex)
		{
			// More writes had happened during the read. Should be rare, but can happen.
			// activate it for a kind of debugging mode
			if (debug_level > 0) {
				printf("%s: INFO - Sequence number mismatch detected - Data not accessable during write process by game\n", sTime);
			}
			
		}
		else {
			// At this point all checks are passed without problem and the localCopy can be updated with new data. 
			// In all other error cases the data from the previous loop pass is used
			memcpy(localCopy, localCopyTmp, sizeof(SharedMemory));
		}
	}
	
	//for debugging
	if (debug_level > 1) {
		printf("%s: INFO - Sequence number increase %d, current index %d, previous index %d\n", sTime, indexChange, localCopy->mSequenceNumber, updateIndex);
	}
	
	// old way with direct access to the Shared  Memory data
	//std::string responseJson = sharedMemoryRenderer.render(sharedData, getQueryString(hm));
	// new way with using the local copy data
	std::string responseJson = sharedMemoryRenderer.render(localCopy, getQueryString(hm));
	std::string response;

	bool gzipResponse = shouldGzipResponse(hm, responseJson.size());

	if (gzipResponse) {
		response = Utils::gzipString(responseJson);
	}
	else {
		response = responseJson;
	}

	// build HTTP OK response with JSON response body
	mg_printf(nc, "HTTP/1.1 200 OK\r\n"
		"Content-Type: application/json\r\n"
		"Cache-Control: no-cache\r\n"
		"Access-Control-Allow-Origin: *\r\n");
	if (gzipResponse) {
		mg_printf(nc, "Content-Encoding: gzip\r\n");
	}
	mg_printf(nc, "Content-Length: %d\r\n\r\n",
		(int)response.size());
	mg_send(nc, response.data(), response.size());

}

// Processes the shared memory
void processSharedMemoryData(struct mg_connection *nc, const SharedMemory* sharedData, struct http_message *hm)   {
	// Ensure we're sync'd to the correct data version
	if (sharedData->mVersion != SHARED_MEMORY_VERSION)	{
		// build conflict response
		sendConflict(nc);
		printf("Data version mismatch, please make sure that your AMS2 version matches your CREST2 version\n    Expected: v%d, Current: v%d\n", SHARED_MEMORY_VERSION, sharedData->mVersion);
	}else{
		renderResponse(nc, sharedData, hm);
	}

}

// Processes the memory mapped file
void processFile(struct mg_connection *nc, HANDLE fileHandle, struct http_message *hm)    {

	const SharedMemory* sharedData = (SharedMemory*)MapViewOfFile(fileHandle, PAGE_READONLY, 0, 0, sizeof(SharedMemory));

	if (sharedData == NULL)	{
		// File found, but could not be mapped to shared memory data
		sendServiceUnavailable(nc);
	}
	else{
		// Process file
		processSharedMemoryData(nc, sharedData, hm);
		// Unmap file
		UnmapViewOfFile(sharedData);
	}

}

void handleGet(struct mg_connection *nc, struct http_message *hm)    {
    // Open the memory mapped file
    HANDLE fileHandle = OpenFileMappingA(PAGE_READONLY, FALSE, MAP_OBJECT_NAME);
    
    if (fileHandle == NULL)	{
        // File is not available, build service unavailable response
        sendServiceUnavailable(nc);
    }
    else{
        // File is available, process the file
        processFile(nc, fileHandle, hm);
        // Close the file
        CloseHandle(fileHandle);
    }
}

void HttpMessageHandler::handle(struct mg_connection *nc, struct http_message *hm)	{
    std::string requestMethod = getMethod(hm);
    if (requestMethod.compare("GET") == 0)  {
        handleGet(nc, hm);
    }else if (requestMethod.compare("OPTIONS") == 0)  {
        sendOptions(nc);
    }
}
