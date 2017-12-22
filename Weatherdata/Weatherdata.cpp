// Weatherdata.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <iostream>
#include <sstream>
#include <boost\asio.hpp>
// #include "client_http.hpp"
// #include <json.h>
#include "json.hpp"
#include "config.h"


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
		utility::string_t Key = config::APIkey; 

		http_client client(U("http://api.worldweatheronline.com"));
		uri_builder stringBuilder(Key);
		// stringBuilder.append_query(U("&"), U("London format=json date=2008-08-01 enddate=2008-08-08"));
		return client.request(methods::GET, stringBuilder.to_string());
	})
		
		.then([=](http_response response)		// set up response handler
	{
		printf("Received response status code:%u\n", response.status_code()); // get me the response code
		
		return response.body().read_to_end(fileStream->streambuf());
		
	})		\

		.then([=](size_t)		// close file stream
	{
		return fileStream->close();
	})
		
		.then([=]()
	{

		// read a JSON file
		std::ifstream readFromFile("results.txt");
		if (readFromFile.is_open()) {

		nlohmann::json j;
		readFromFile >> j;

		// write prettified JSON to another file
		std::ofstream writeToFile("pretty.json");
		writeToFile << std::setw(4) << j << std::endl;
		
		readFromFile.close();
		writeToFile.close();
		}
		else {
			std::cout << "unable to open file";	
		}
				
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

