// Weatherdata.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <iostream>
#include <sstream>
// #include <json.h>
#include "json.hpp"


using namespace utility;				// string conversion
using namespace web;					// URI 
using namespace web::http;				// HTTP commands
using namespace web::http::client;		// HTTP Client features
using namespace concurrency::streams;	// Asynch streams, like Node likes 'em

using json = nlohmann::json;

int main()
{
	auto fileStream = std::make_shared<ostream>();

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("results.txt")).then([=](ostream outFile)
	{
		*fileStream = outFile;

		http_client client(U("http://api.worldweatheronline.com"));
		uri_builder stringBuilder(U("/premium/v1/past-weather.ashx?key=abbb68603d994b7388d100544172311&q=London&format=json&date=2013-04-21&enddate=2013-04-28"));
		// stringBuilder.append_query(U("&"), U("London format=json date=2008-08-01 enddate=2008-08-08"));
		return client.request(methods::GET, stringBuilder.to_string());
	})
		
		.then([=](http_response response)		// set up response handler
	{
		printf("Received response status code:%u\n", response.status_code()); // get me the response code
		
		return response.body().read_to_end(fileStream->streambuf());
		
	})		

		.then([=](size_t)		// close file stream
	{
		return fileStream->close();
	})
		
		.then([=]()
	{
		nlohmann::json j;
		std::ofstream o("pretty.json");
		o << std::setw(4) << j << std::endl;
		printf("json created!\n");

	});

	// Wait for all the outstanding I/O to complete, handle exceptions
	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e) // class called exeption I call e, pass by ref
	{
		printf("Error exception:%s\n", e.what());
	}


	return 0;
}

